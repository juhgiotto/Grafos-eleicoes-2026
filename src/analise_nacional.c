/*
 * =========================================================================
 * Modulo: Eixo Nacional (src/analise_nacional.c)
 * Papel : Carga e analise dos grafos interestaduais (27 UFs do Brasil).
 * Teoria: Grafo Territorial (G_Terr), Grafo Completo de Distancia Eleitoral
 *         (G_Pres), Clusters via DFS e AGM via Kruskal + Union-Find.
 * =========================================================================
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/analise_df.h"
#include "../include/grafo.h"

#define TAM_LINHA_NAC 4096    /* tamanho maximo de uma linha do CSV */
#define MAX_CAND_NAC 64       /* maximo de colunas de candidatos aceitas */
#define MAX_UFS_NAC 32        /* maximo de UFs aceitas no CSV de votos */
#define UF_EXTERIOR "ZZ"      /* voto no exterior: nao e UF, e ignorado */

/* =========================================================================
 * Utilitarios de Leitura e Carga dos Grafos (G_Terr e G_Pres)
 * ========================================================================= */

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

/* =========================================================================
 * Busca em Profundidade (DFS) — Clusters de Alinhamento Eleitoral (#10)
 * ========================================================================= */

/*
 * Objetivo     : Agrupa UFs em clusters com padrao de voto presidencial proximo.
 * Algoritmo    : Busca em Profundidade (DFS) com pilha explicita, filtrando
 *                apenas arestas com distancia euclidiana <= limiar_corte.
 * Detalhe-Chave: Cada componente conexa do grafo filtrado forma um cluster.
 */
void busca_profundidade_clusters(Grafo *g_pres, float limiar_corte) {
    int n, s, u, v, topo, comp;
    int *visitado;
    int *pilha;
    const No *a;

    if (g_pres == NULL) {
        fprintf(stderr, "DFS: grafo nulo.\n");
        return;
    }

    n = g_pres->num_vertices;
    if (n <= 0) {
        fprintf(stderr, "DFS: grafo vazio.\n");
        return;
    }

    visitado = calloc((size_t)n, sizeof(int));
    pilha = malloc((size_t)n * sizeof(int));
    if (visitado == NULL || pilha == NULL) {
        fprintf(stderr, "DFS: falha de memoria.\n");
        free(visitado);
        free(pilha);
        return;
    }

    printf("Clusters de G_Pres (limiar de distancia <= %.4f):\n", limiar_corte);
    comp = 0;
    for (s = 0; s < n; s++) {
        if (visitado[s]) {
            continue;
        }
        /* Vertice ainda nao visitado: inicia uma nova componente. */
        comp++;
        printf("Componente %d:", comp);
        topo = 0;
        visitado[s] = 1;
        pilha[topo++] = s;
        while (topo > 0) {
            u = pilha[--topo];
            printf(" %s", g_pres->rotulos[u]);
            for (a = g_pres->adj[u].cabeca; a != NULL; a = a->prox) {
                v = a->destino;
                if (!visitado[v] && a->peso <= limiar_corte) {
                    visitado[v] = 1;
                    pilha[topo++] = v;
                }
            }
        }
        printf("\n");
    }
    printf("Total de componentes: %d\n", comp);

    free(visitado);
    free(pilha);
}

/* =========================================================================
 * Arvore Geradora Minima (AGM) — Algoritmo de Kruskal + Union-Find (#11)
 * ========================================================================= */

typedef struct {
    int u, v;
    float peso;
} ArestaKruskal;

/* Peso crescente; empate desfeito por (u, v) para a saida ser deterministica. */
static int nac_comparar_arestas(const void *a, const void *b) {
    const ArestaKruskal *x = (const ArestaKruskal *)a;
    const ArestaKruskal *y = (const ArestaKruskal *)b;

    if (x->peso < y->peso) {
        return -1;
    }
    if (x->peso > y->peso) {
        return 1;
    }
    if (x->u != y->u) {
        return x->u - y->u;
    }
    return x->v - y->v;
}

/* find com compressao de caminho. */
static int nac_uf_find(int *pai, int x) {
    if (pai[x] != x) {
        pai[x] = nac_uf_find(pai, pai[x]);
    }
    return pai[x];
}

static void nac_uf_union(int *pai, int a, int b) {
    pai[nac_uf_find(pai, a)] = nac_uf_find(pai, b);
}

/*
 * Objetivo     : Conecta todas as 27 UFs com a menor soma de distancias eleitorais.
 * Algoritmo    : Kruskal: ordena arestas (qsort) e une componentes sem ciclo
 *                via Union-Find com compressao de caminho, parando em V - 1.
 * Detalhe-Chave: Complexidade O(E log E); revela pares de UFs mais proximas.
 */
void calcular_mst_eleitoral_kruskal(Grafo *g_pres) {
    int n, total, qtd, i, u, v, escolhidas;
    float peso_total;
    int *pai;
    ArestaKruskal *arestas;
    const No *a;

    if (g_pres == NULL) {
        fprintf(stderr, "Kruskal: grafo nulo.\n");
        return;
    }

    n = g_pres->num_vertices;
    if (n <= 0) {
        fprintf(stderr, "Kruskal: grafo vazio.\n");
        return;
    }

    /* 1. Coleta das arestas (sem duplicar u-v e v-u). */
    total = 0;
    for (u = 0; u < n; u++) {
        total += g_pres->adj[u].num_arestas;
    }
    arestas = malloc((size_t)(total > 0 ? total : 1) * sizeof(ArestaKruskal));
    pai = malloc((size_t)n * sizeof(int));
    if (arestas == NULL || pai == NULL) {
        fprintf(stderr, "Kruskal: falha de memoria.\n");
        free(arestas);
        free(pai);
        return;
    }

    qtd = 0;
    for (u = 0; u < n; u++) {
        for (a = g_pres->adj[u].cabeca; a != NULL; a = a->prox) {
            if (g_pres->orientado || u < a->destino) {
                arestas[qtd].u = u;
                arestas[qtd].v = a->destino;
                arestas[qtd].peso = a->peso;
                qtd++;
            }
        }
    }

    /* 2. Ordenacao por peso crescente. */
    qsort(arestas, (size_t)qtd, sizeof(ArestaKruskal), nac_comparar_arestas);

    /* 3. Union-Find: cada vertice comeca como sua propria raiz. */
    for (i = 0; i < n; i++) {
        pai[i] = i;
    }

    /* 4-5. Selecao das arestas da AGM; para ao atingir V-1. */
    escolhidas = 0;
    peso_total = 0.0f;
    printf("AGM (Kruskal) de G_Pres:\n");
    for (i = 0; i < qtd && escolhidas < n - 1; i++) {
        u = arestas[i].u;
        v = arestas[i].v;
        if (nac_uf_find(pai, u) != nac_uf_find(pai, v)) {
            nac_uf_union(pai, u, v);
            printf("  %s - %s (peso %.4f)\n", g_pres->rotulos[u], g_pres->rotulos[v],
                   arestas[i].peso);
            peso_total += arestas[i].peso;
            escolhidas++;
        }
    }

    /* 6. Resultado. */
    printf("Arestas na AGM: %d (esperado V-1 = %d)\n", escolhidas, n - 1);
    printf("Peso total: %.4f\n", peso_total);
    if (escolhidas < n - 1) {
        printf("Aviso: grafo desconexo; resultado e uma floresta geradora minima.\n");
    }

    free(pai);
    free(arestas);
}