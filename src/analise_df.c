#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/analise_df.h"
#include "../include/grafo.h"

#define TAM_LINHA 4096        /* tamanho máximo de uma linha do CSV */
#define MAX_CANDIDATOS 64     /* máximo de colunas de candidatos aceitas */
#define SEPARADOR ";"
#define EPSILON_EMPATE 1e-4   /* tolerância para considerar empate entre percentuais */

/* Remove '\n' e '\r' do final da linha (suporta LF e CRLF). */
static void remover_quebra_de_linha(char *linha) {
    size_t n = strlen(linha);
    while (n > 0 && (linha[n - 1] == '\n' || linha[n - 1] == '\r')) {
        linha[--n] = '\0';
    }
}

/*
 * Descarta o restante de uma linha maior que o buffer.
 * Retorna 1 se a linha lida estava truncada (sem '\n' e fora do fim de arquivo).
 */
static int linha_truncada(FILE *f, const char *linha) {
    int c;

    if (strchr(linha, '\n') != NULL || feof(f)) {
        return 0;
    }
    while ((c = fgetc(f)) != EOF && c != '\n') {
        /* consome o resto da linha */
    }
    return 1;
}

/* Converte `texto` em percentual válido [0, 100]. Retorna 0 se OK, -1 se inválido. */
static int parse_percentual(const char *texto, float *saida) {
    char *fim = NULL;
    double v;

    if (!texto || *texto == '\0') {
        return -1;
    }
    v = strtod(texto, &fim);
    /* `v != v` detecta NaN (isfinite e C99); infinito cai em v > 100. */
    if (fim == texto || *fim != '\0' || v != v || v < 0.0 || v > 100.0) {
        return -1;
    }
    *saida = (float)v;
    return 0;
}

/*
 * Separa o cabeçalho em `nomes` (ponteiros para dentro de `linha`, que é
 * modificada). Retorna a quantidade de candidatos (>= 1) ou -1 se inválido.
 */
static int ler_cabecalho(char *linha, char *nomes[MAX_CANDIDATOS]) {
    char *tok;
    int n = 0;

    tok = strtok(linha, SEPARADOR);
    if (!tok) {
        return -1;
    }
    /* Ignora BOM UTF-8 eventual antes de "RA". */
    if ((unsigned char)tok[0] == 0xEF && (unsigned char)tok[1] == 0xBB &&
        (unsigned char)tok[2] == 0xBF) {
        tok += 3;
    }
    if (strcmp(tok, "RA") != 0) {
        return -1;
    }

    while ((tok = strtok(NULL, SEPARADOR)) != NULL) {
        if (n == MAX_CANDIDATOS || *tok == '\0') {
            return -1;
        }
        nomes[n++] = tok;
    }
    return n > 0 ? n : -1;
}

/* Retorna o índice do vértice com `rotulo`, criando-o se não existir (-1 em erro). */
static int obter_ou_criar_vertice(Grafo *g, const char *rotulo) {
    int idx = buscar_vertice(g, rotulo);
    if (idx != -1) {
        return idx;
    }
    return adicionar_vertice(g, rotulo);
}

Grafo *construir_grafo_bipartido_df(const char *caminho) {
    FILE *f;
    Grafo *g;
    char linha[TAM_LINHA];
    /* `cabecalho` precisa viver até o fim, pois `candidatos[]` aponta para ele. */
    char cabecalho[TAM_LINHA];
    char *candidatos[MAX_CANDIDATOS];
    float pesos[MAX_CANDIDATOS];
    char *nome_ra;
    char *tok;
    int num_candidatos;
    int num_linha;
    int lidos;
    int valida;
    int ra;
    int cand;
    int i;

    if (!caminho) {
        return NULL;
    }

    f = fopen(caminho, "r");
    if (!f) {
        fprintf(stderr, "Erro: nao foi possivel abrir '%s'\n", caminho);
        return NULL;
    }

    /* Cabeçalho (os nomes dos candidatos vêm dele). */
    if (!fgets(linha, sizeof(linha), f) || linha_truncada(f, linha)) {
        fprintf(stderr, "Erro: cabecalho ausente ou grande demais em '%s'\n", caminho);
        fclose(f);
        return NULL;
    }
    remover_quebra_de_linha(linha);

    strcpy(cabecalho, linha);
    num_candidatos = ler_cabecalho(cabecalho, candidatos);
    if (num_candidatos < 0) {
        fprintf(stderr, "Erro: cabecalho invalido em '%s' (esperado 'RA;candidato;...')\n",
                caminho);
        fclose(f);
        return NULL;
    }

    g = criar_grafo(32, 0);
    if (!g) {
        fprintf(stderr, "Erro: falha ao alocar o grafo\n");
        fclose(f);
        return NULL;
    }

    num_linha = 1;
    while (fgets(linha, sizeof(linha), f)) {
        num_linha++;

        if (linha_truncada(f, linha)) {
            fprintf(stderr, "Aviso: linha %d ignorada (maior que %d caracteres)\n",
                    num_linha, TAM_LINHA - 1);
            continue;
        }
        remover_quebra_de_linha(linha);
        if (linha[0] == '\0') {
            continue; /* linha em branco */
        }

        /* Tokeniza: strtok colapsa campos vazios, o que muda a contagem e a
         * linha é rejeitada abaixo (nunca desalinha colunas silenciosamente). */
        nome_ra = strtok(linha, SEPARADOR);
        lidos = 0;
        valida = (nome_ra != NULL && *nome_ra != '\0');

        while (valida && (tok = strtok(NULL, SEPARADOR)) != NULL) {
            if (lidos == num_candidatos || parse_percentual(tok, &pesos[lidos]) != 0) {
                valida = 0;
                break;
            }
            lidos++;
        }
        if (!valida || lidos != num_candidatos) {
            fprintf(stderr, "Aviso: linha %d ignorada (malformada)\n", num_linha);
            continue;
        }

        ra = obter_ou_criar_vertice(g, nome_ra);
        if (ra == -1) {
            fprintf(stderr, "Erro: falha ao criar o vertice da RA '%s'\n", nome_ra);
            liberar_grafo(g);
            fclose(f);
            return NULL;
        }

        for (i = 0; i < num_candidatos; i++) {
            cand = obter_ou_criar_vertice(g, candidatos[i]);
            if (cand == -1 || adicionar_aresta(g, ra, cand, pesos[i]) != 0) {
                fprintf(stderr, "Erro: falha ao ligar '%s' a '%s'\n", nome_ra, candidatos[i]);
                liberar_grafo(g);
                fclose(f);
                return NULL;
            }
        }
    }

    fclose(f);
    return g;
}

/*
 * Classifica os vértices de um grafo bipartido em duas partições (0 = RA,
 * 1 = Candidato) via BFS, assumindo que o primeiro vértice com arestas
 * pertence à partição 0 (RA).
 * Retorna 0 em caso de sucesso, ou -1 se o grafo não for bipartido ou em
 * falha de alocação.
 */
static int classificar_vertices_bipartido(const Grafo *g, int *tipo) {
    int *fila;
    int ini;
    int fim;
    int i;
    int u;
    const No *atual;

    fila = malloc((size_t)g->num_vertices * sizeof(int));
    if (!fila) {
        return -1;
    }

    for (i = 0; i < g->num_vertices; i++) {
        tipo[i] = -1;
    }

    for (i = 0; i < g->num_vertices; i++) {
        if (tipo[i] != -1 || g->adj[i].num_arestas == 0) {
            continue;
        }
        tipo[i] = 0;
        ini = 0;
        fim = 0;
        fila[fim++] = i;

        while (ini < fim) {
            u = fila[ini++];
            for (atual = g->adj[u].cabeca; atual != NULL; atual = atual->prox) {
                if (tipo[atual->destino] == -1) {
                    tipo[atual->destino] = 1 - tipo[u];
                    fila[fim++] = atual->destino;
                } else if (tipo[atual->destino] == tipo[u]) {
                    free(fila);
                    return -1;
                }
            }
        }
    }

    free(fila);
    return 0;
}

/*
 * Imprime o candidato dominante (ou todos os empatados, se houver empate
 * dentro de EPSILON_EMPATE) para o vértice `ra`.
 */
static void imprimir_vencedor_ra(const Grafo *g, int ra) {
    const No *atual;
    float max_peso;
    int empates;
    int primeiro;

    atual = g->adj[ra].cabeca;
    if (!atual) {
        return;
    }

    max_peso = atual->peso;
    for (atual = atual->prox; atual != NULL; atual = atual->prox) {
        if (atual->peso > max_peso) {
            max_peso = atual->peso;
        }
    }

    empates = 0;
    for (atual = g->adj[ra].cabeca; atual != NULL; atual = atual->prox) {
        if (fabs((double)(atual->peso - max_peso)) < EPSILON_EMPATE) {
            empates++;
        }
    }

    if (empates == 1) {
        for (atual = g->adj[ra].cabeca; atual != NULL; atual = atual->prox) {
            if (fabs((double)(atual->peso - max_peso)) < EPSILON_EMPATE) {
                printf("%s -> %s (%.2f%%)\n", g->rotulos[ra],
                       g->rotulos[atual->destino], max_peso);
                break;
            }
        }
    } else {
        printf("%s -> Empate: ", g->rotulos[ra]);
        primeiro = 1;
        for (atual = g->adj[ra].cabeca; atual != NULL; atual = atual->prox) {
            if (fabs((double)(atual->peso - max_peso)) < EPSILON_EMPATE) {
                if (!primeiro) {
                    printf(", ");
                }
                printf("%s", g->rotulos[atual->destino]);
                primeiro = 0;
            }
        }
        printf(" (%.2f%%)\n", max_peso);
    }
}

void candidatos_dominantes_por_ra(const Grafo *g) {
    int *tipo;
    int i;

    if (!g || g->num_vertices <= 0) {
        return;
    }

    tipo = malloc((size_t)g->num_vertices * sizeof(int));
    if (!tipo) {
        fprintf(stderr, "Erro: falha ao alocar vetor de classificacao de vertices\n");
        return;
    }

    if (classificar_vertices_bipartido(g, tipo) != 0) {
        fprintf(stderr, "Erro: grafo invalido (nao bipartido ou falha de memoria)\n");
        free(tipo);
        return;
    }

    for (i = 0; i < g->num_vertices; i++) {
        if (tipo[i] == 0 && g->adj[i].num_arestas > 0) {
            imprimir_vencedor_ra(g, i);
        }
    }

    free(tipo);
}

/*
 * Métrica escolhida: Similaridade de Cosseno.
 * Justificativa: representa cada RA como um vetor de percentuais na mesma
 * ordem de candidatos e mede o cosseno do ângulo entre dois perfis eleitorais.
 * É invariante a pequenas diferenças na soma total dos percentuais entre RAs
 * (como votos brancos/nulos ou candidatos menores) e produz pesos contínuos
 * em [0, 1] adequados para projetar o grafo bipartido em um grafo de RAs.
 */
float similaridade_cosseno(const float *a, const float *b, int n) {
    double produto_escalar;
    double norma_a;
    double norma_b;
    double denom;
    int i;

    if (!a || !b || n <= 0) {
        return 0.0f;
    }

    produto_escalar = 0.0;
    norma_a = 0.0;
    norma_b = 0.0;

    for (i = 0; i < n; i++) {
        produto_escalar += (double)a[i] * (double)b[i];
        norma_a += (double)a[i] * (double)a[i];
        norma_b += (double)b[i] * (double)b[i];
    }

    if (norma_a <= 0.0 || norma_b <= 0.0) {
        return 0.0f;
    }

    denom = sqrt(norma_a) * sqrt(norma_b);
    if (denom <= 0.0) {
        return 0.0f;
    }

    return (float)(produto_escalar / denom);
}

int extrair_vetor_ra(const Grafo *g, int ra, const int *candidatos,
                     int num_candidatos, float *saida) {
    const No *atual;
    int i;

    if (!g || !candidatos || !saida || num_candidatos <= 0 ||
        ra < 0 || ra >= g->num_vertices) {
        return -1;
    }

    for (i = 0; i < num_candidatos; i++) {
        saida[i] = 0.0f;
    }

    for (atual = g->adj[ra].cabeca; atual != NULL; atual = atual->prox) {
        for (i = 0; i < num_candidatos; i++) {
            if (atual->destino == candidatos[i]) {
                saida[i] = atual->peso;
                break;
            }
        }
    }

    return 0;
}

Grafo *gerar_grafo_similaridade_ra(const Grafo *g_bipartido,
                                   float limiar_similaridade) {
    int *tipo;
    int *ras;
    int *cands;
    float *vetores;
    Grafo *g_ra;
    int num_ras;
    int num_cands;
    int i;
    int j;
    float sim;

    if (!g_bipartido || g_bipartido->num_vertices <= 0) {
        return NULL;
    }

    tipo = malloc((size_t)g_bipartido->num_vertices * sizeof(int));
    ras = malloc((size_t)g_bipartido->num_vertices * sizeof(int));
    cands = malloc((size_t)g_bipartido->num_vertices * sizeof(int));
    if (!tipo || !ras || !cands) {
        free(tipo);
        free(ras);
        free(cands);
        return NULL;
    }

    if (classificar_vertices_bipartido(g_bipartido, tipo) != 0) {
        free(tipo);
        free(ras);
        free(cands);
        return NULL;
    }

    num_ras = 0;
    num_cands = 0;
    for (i = 0; i < g_bipartido->num_vertices; i++) {
        if (g_bipartido->adj[i].num_arestas == 0) {
            continue;
        }
        if (tipo[i] == 0) {
            ras[num_ras++] = i;
        } else if (tipo[i] == 1) {
            cands[num_cands++] = i;
        }
    }
    free(tipo);

    if (num_ras == 0 || num_cands == 0) {
        free(ras);
        free(cands);
        return NULL;
    }

    vetores = malloc((size_t)(num_ras * num_cands) * sizeof(float));
    g_ra = criar_grafo(num_ras, 0);
    if (!vetores || !g_ra) {
        free(vetores);
        liberar_grafo(g_ra);
        free(ras);
        free(cands);
        return NULL;
    }

    for (i = 0; i < num_ras; i++) {
        if (adicionar_vertice(g_ra, g_bipartido->rotulos[ras[i]]) == -1 ||
            extrair_vetor_ra(g_bipartido, ras[i], cands, num_cands,
                             &vetores[i * num_cands]) != 0) {
            free(vetores);
            liberar_grafo(g_ra);
            free(ras);
            free(cands);
            return NULL;
        }
    }

    for (i = 0; i < num_ras; i++) {
        for (j = i + 1; j < num_ras; j++) {
            sim = similaridade_cosseno(&vetores[i * num_cands],
                                       &vetores[j * num_cands],
                                       num_cands);
            if (sim >= limiar_similaridade) {
                if (adicionar_aresta(g_ra, i, j, sim) != 0) {
                    free(vetores);
                    liberar_grafo(g_ra);
                    free(ras);
                    free(cands);
                    return NULL;
                }
            }
        }
    }

    free(vetores);
    free(ras);
    free(cands);
    return g_ra;
}
