#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/analise_df.h"
#include "../include/grafo.h"

#define TAM_LINHA_NAC 4096    /* tamanho máximo de uma linha do CSV */
#define MAX_CAND_NAC 64       /* máximo de colunas de candidatos aceitas */
#define MAX_UFS_NAC 32        /* máximo de UFs aceitas no CSV de votos */
#define UF_EXTERIOR "ZZ"      /* voto no exterior: não é UF, é ignorado */

/* Remove '\n' e '\r' do final da linha (suporta LF e CRLF). */
static void nac_remover_quebra(char *linha) {
    size_t n = strlen(linha);
    while (n > 0 && (linha[n - 1] == '\n' || linha[n - 1] == '\r')) {
        linha[--n] = '\0';
    }
}

/* Converte `texto` em percentual válido [0, 100]. Retorna 0 se OK, -1 se inválido. */
static int nac_parse_percentual(const char *texto, float *saida) {
    char *fim = NULL;
    double v;

    if (!texto || *texto == '\0') {
        return -1;
    }
    v = strtod(texto, &fim);
    if (fim == texto || *fim != '\0' || v != v || v < 0.0 || v > 100.0) {
        return -1;
    }
    *saida = (float)v;
    return 0;
}

/* Pula o BOM UTF-8 eventual no início de `s`. */
static char *nac_pular_bom(char *s) {
    if ((unsigned char)s[0] == 0xEF && (unsigned char)s[1] == 0xBB &&
        (unsigned char)s[2] == 0xBF) {
        return s + 3;
    }
    return s;
}

/*
 * G_Terr: um vértice por UF, uma aresta (peso 1) por fronteira.
 * Formato: "UF;FRONTEIRAS" no cabeçalho e "AC;AM,RO" nas linhas.
 */
Grafo *carregar_g_terr(const char *caminho) {
    FILE *f;
    Grafo *g;
    char linha[TAM_LINHA_NAC];
    char *lista;
    char *viz;
    int pass;
    int a;
    int b;

    if (!caminho) {
        return NULL;
    }
    g = criar_grafo(32, 0);
    if (!g) {
        return NULL;
    }

    /* Passo 0: cria os vértices. Passo 1: cria as arestas (todas as UFs já existem). */
    for (pass = 0; pass < 2; pass++) {
        f = fopen(caminho, "r");
        if (!f) {
            fprintf(stderr, "Erro: nao foi possivel abrir '%s'\n", caminho);
            liberar_grafo(g);
            return NULL;
        }
        if (!fgets(linha, sizeof(linha), f)) { /* cabeçalho */
            fclose(f);
            liberar_grafo(g);
            return NULL;
        }
        while (fgets(linha, sizeof(linha), f)) {
            nac_remover_quebra(linha);
            lista = strchr(linha, ';');
            if (linha[0] == '\0' || !lista) {
                continue; /* linha em branco ou sem ';' */
            }
            *lista++ = '\0';
            if (pass == 0) {
                if (buscar_vertice(g, linha) == -1 && adicionar_vertice(g, linha) == -1) {
                    fclose(f);
                    liberar_grafo(g);
                    return NULL;
                }
                continue;
            }
            a = buscar_vertice(g, linha);
            for (viz = strtok(lista, ","); viz != NULL; viz = strtok(NULL, ",")) {
                b = buscar_vertice(g, viz);
                if (b == -1) {
                    fprintf(stderr, "Aviso: vizinho '%s' de '%s' nao existe\n", viz, linha);
                    continue;
                }
                /* adicionar_aresta ignora a repetição (A-B e B-A) e já é simétrica. */
                if (a != b && adicionar_aresta(g, a, b, 1.0f) != 0) {
                    fclose(f);
                    liberar_grafo(g);
                    return NULL;
                }
            }
        }
        fclose(f);
    }
    return g;
}

/*
 * G_Pres: um vértice por UF, aresta entre todos os pares com peso =
 * distância euclidiana entre os vetores de percentuais das duas UFs.
 * Formato: "UF;<candidato 1>;..." e "AC;<pct 1>;...". A linha "ZZ" é ignorada.
 */
Grafo *carregar_g_pres(const char *caminho) {
    static float pct[MAX_UFS_NAC][MAX_CAND_NAC];
    FILE *f;
    Grafo *g;
    char linha[TAM_LINHA_NAC];
    char *tok;
    char *uf;
    int num_cand = 0;
    int num_ufs = 0;
    int lidos;
    int valida;
    int num_linha = 1;
    int i;
    int j;
    int k;
    double soma;
    double d;

    if (!caminho) {
        return NULL;
    }
    f = fopen(caminho, "r");
    if (!f) {
        fprintf(stderr, "Erro: nao foi possivel abrir '%s'\n", caminho);
        return NULL;
    }
    g = criar_grafo(32, 0);
    if (!g) {
        fclose(f);
        return NULL;
    }

    /* Cabeçalho: só interessa a quantidade de candidatos. */
    if (!fgets(linha, sizeof(linha), f)) {
        fprintf(stderr, "Erro: cabecalho ausente em '%s'\n", caminho);
        fclose(f);
        liberar_grafo(g);
        return NULL;
    }
    nac_remover_quebra(linha);
    tok = strtok(nac_pular_bom(linha), ";");
    if (!tok || strcmp(tok, "UF") != 0) {
        fprintf(stderr, "Erro: cabecalho invalido em '%s' (esperado 'UF;candidato;...')\n",
                caminho);
        fclose(f);
        liberar_grafo(g);
        return NULL;
    }
    while (strtok(NULL, ";") != NULL) {
        num_cand++;
    }
    if (num_cand < 1 || num_cand > MAX_CAND_NAC) {
        fprintf(stderr, "Erro: numero de candidatos invalido em '%s'\n", caminho);
        fclose(f);
        liberar_grafo(g);
        return NULL;
    }

    while (fgets(linha, sizeof(linha), f)) {
        num_linha++;
        nac_remover_quebra(linha);
        if (linha[0] == '\0') {
            continue;
        }
        tok = strtok(linha, ";");
        if (!tok || strcmp(tok, UF_EXTERIOR) == 0) {
            continue;
        }
        uf = tok;
        lidos = 0;
        valida = (num_ufs < MAX_UFS_NAC && buscar_vertice(g, uf) == -1);
        while (valida && (tok = strtok(NULL, ";")) != NULL) {
            if (lidos == num_cand || nac_parse_percentual(tok, &pct[num_ufs][lidos]) != 0) {
                valida = 0;
            } else {
                lidos++;
            }
        }
        if (!valida || lidos != num_cand) {
            fprintf(stderr, "Aviso: linha %d ignorada (malformada, UF repetida ou limite)\n",
                    num_linha);
            continue;
        }
        if (adicionar_vertice(g, uf) == -1) {
            fclose(f);
            liberar_grafo(g);
            return NULL;
        }
        num_ufs++;
    }
    fclose(f);

    /* Grafo completo: peso = distância euclidiana entre os vetores de percentuais. */
    for (i = 0; i < num_ufs; i++) {
        for (j = i + 1; j < num_ufs; j++) {
            soma = 0.0;
            for (k = 0; k < num_cand; k++) {
                d = (double)pct[i][k] - (double)pct[j][k];
                soma += d * d;
            }
            if (adicionar_aresta(g, i, j, (float)sqrt(soma)) != 0) {
                liberar_grafo(g);
                return NULL;
            }
        }
    }
    return g;
}