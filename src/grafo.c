/*
 * =========================================================================
 * Modulo: Base de Grafos (src/grafo.c)
 * Papel : Implementacao da estrutura de grafo usada por todo o projeto.
 * Teoria: Listas de Adjacencia Dinamicas (crescimento via realloc) e
 *         Busca em Largura (BFS) com fila circular FIFO.
 * =========================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/grafo.h"

/* =========================================================================
 * Auxiliares Internos
 * ========================================================================= */

/* strdup nao faz parte do padrao ANSI C89; copia manual via memcpy. */
static char *duplicar_string(const char *s) {
    size_t n = strlen(s) + 1;
    char *copia = (char *)malloc(n);
    if (copia) {
        memcpy(copia, s, n);
    }
    return copia;
}

/* Verifica se já existe uma aresta para `destino` na lista informada. */
static int aresta_existe(const Lista *lista, int destino) {
    const No *atual = lista->cabeca;
    while (atual != NULL) {
        if (atual->destino == destino) {
            return 1;
        }
        atual = atual->prox;
    }
    return 0;
}

/* Aloca e insere um novo nó no início da lista de adjacência. */
static int inserir_no(Lista *lista, int destino, float peso) {
    No *novo = (No *)malloc(sizeof(No));
    if (!novo) {
        return -1;
    }
    novo->destino = destino;
    novo->peso = peso;
    novo->prox = lista->cabeca;
    lista->cabeca = novo;
    lista->num_arestas++;
    return 0;
}

/* =========================================================================
 * Criacao, Insercao e Busca de Vertices
 * ========================================================================= */

Grafo *criar_grafo(int capacidade_inicial, int orientado) {
    Grafo *g;

    if (capacidade_inicial <= 0) {
        return NULL;
    }

    g = (Grafo *)malloc(sizeof(Grafo));
    if (!g) {
        return NULL;
    }

    g->adj = (Lista *)calloc((size_t)capacidade_inicial, sizeof(Lista));
    g->rotulos = (char **)calloc((size_t)capacidade_inicial, sizeof(char *));
    if (!g->adj || !g->rotulos) {
        free(g->adj);
        free(g->rotulos);
        free(g);
        return NULL;
    }

    g->num_vertices = 0;
    g->capacidade = capacidade_inicial;
    g->orientado = orientado ? 1 : 0;
    return g;
}

int buscar_vertice(const Grafo *g, const char *rotulo) {
    int i;

    if (!g || !rotulo) {
        return -1;
    }

    for (i = 0; i < g->num_vertices; i++) {
        if (g->rotulos[i] && strcmp(g->rotulos[i], rotulo) == 0) {
            return i;
        }
    }
    return -1;
}

int adicionar_vertice(Grafo *g, const char *rotulo) {
    int nova_cap;
    int i;
    int idx;
    Lista *novo_adj;
    char **novos_rotulos;
    char *copia;

    if (!g || !rotulo) {
        return -1;
    }

    if (buscar_vertice(g, rotulo) != -1) {
        return -1;
    }

    if (g->num_vertices == g->capacidade) {
        /* Dobra a capacidade quando o vetor de vertices lota (crescimento amortizado O(1)) */
        nova_cap = (g->capacidade > 0) ? g->capacidade * 2 : 1;

        novo_adj = (Lista *)realloc(g->adj, (size_t)nova_cap * sizeof(Lista));
        if (!novo_adj) {
            return -1;
        }
        g->adj = novo_adj;

        novos_rotulos = (char **)realloc(g->rotulos, (size_t)nova_cap * sizeof(char *));
        if (!novos_rotulos) {
            return -1;
        }
        g->rotulos = novos_rotulos;

        for (i = g->capacidade; i < nova_cap; i++) {
            g->adj[i].cabeca = NULL;
            g->adj[i].num_arestas = 0;
            g->rotulos[i] = NULL;
        }
        g->capacidade = nova_cap;
    }

    copia = duplicar_string(rotulo);
    if (!copia) {
        return -1;
    }

    idx = g->num_vertices;
    g->rotulos[idx] = copia;
    g->adj[idx].cabeca = NULL;
    g->adj[idx].num_arestas = 0;
    g->num_vertices++;
    return idx;
}

/* =========================================================================
 * Insercao de Arestas, Impressao e Liberacao de Memoria
 * ========================================================================= */

/*
 * Objetivo     : Conecta dois vertices com uma aresta ponderada.
 * Estrutura    : Insercao O(1) na cabeca da lista apos checagem de duplicata.
 * Detalhe-Chave: Em grafos nao-orientados, garante simetria inserindo nos
 *                dois sentidos (com rollback se a segunda alocacao falhar).
 */
int adicionar_aresta(Grafo *g, int origem, int destino, float peso) {
    No *removido;

    if (!g || origem < 0 || origem >= g->num_vertices ||
        destino < 0 || destino >= g->num_vertices) {
        return -1;
    }

    /* Evita arestas paralelas/duplicadas entre o mesmo par de vertices */
    if (aresta_existe(&g->adj[origem], destino)) {
        return 0;
    }

    if (inserir_no(&g->adj[origem], destino, peso) != 0) {
        return -1;
    }

    /* Garante simetria no grafo nao-orientado */
    if (!g->orientado && origem != destino &&
        !aresta_existe(&g->adj[destino], origem)) {
        if (inserir_no(&g->adj[destino], origem, peso) != 0) {
            removido = g->adj[origem].cabeca;
            g->adj[origem].cabeca = removido->prox;
            g->adj[origem].num_arestas--;
            free(removido);
            return -1;
        }
    }

    return 0;
}

void imprimir_grafo(const Grafo *g) {
    int i;
    const No *atual;

    if (!g) {
        return;
    }

    for (i = 0; i < g->num_vertices; i++) {
        printf("[%d] %s (%d arestas):", i, g->rotulos[i], g->adj[i].num_arestas);
        atual = g->adj[i].cabeca;
        while (atual != NULL) {
            printf(" -> %s (%.2f)", g->rotulos[atual->destino], atual->peso);
            atual = atual->prox;
        }
        printf("\n");
    }
}

/*
 * Objetivo     : Desaloca toda a memoria dinamica associada ao grafo.
 * Ordem        : Nos das listas -> vetor adj -> strings de rotulos -> struct.
 * Detalhe-Chave: Aceita NULL com seguranca; validado com Valgrind (0 leaks).
 */
void liberar_grafo(Grafo *g) {
    int i;
    No *atual;
    No *prox;

    if (!g) {
        return;
    }

    if (g->adj) {
        for (i = 0; i < g->num_vertices; i++) {
            atual = g->adj[i].cabeca;
            while (atual != NULL) {
                prox = atual->prox;
                free(atual);
                atual = prox;
            }
        }
        free(g->adj);
    }

    if (g->rotulos) {
        for (i = 0; i < g->num_vertices; i++) {
            free(g->rotulos[i]);
        }
        free(g->rotulos);
    }

    free(g);
}

/* =========================================================================
 * Busca em Largura (BFS) — Propagacao por Fronteiras (Eixo Nacional)
 * ========================================================================= */

/* Fila circular FIFO de inteiros usada pela BFS. */
typedef struct {
    int *dados;
    int inicio;
    int tamanho;
    int capacidade;
} FilaInt;

static int fila_criar(FilaInt *f, int capacidade) {
    f->dados = malloc((size_t)capacidade * sizeof(int));
    f->inicio = 0;
    f->tamanho = 0;
    f->capacidade = capacidade;
    return f->dados != NULL;
}

static int fila_vazia(const FilaInt *f) {
    return f->tamanho == 0;
}

static void fila_enfileirar(FilaInt *f, int v) {
    f->dados[(f->inicio + f->tamanho) % f->capacidade] = v;
    f->tamanho++;
}

static int fila_desenfileirar(FilaInt *f) {
    int v = f->dados[f->inicio];
    f->inicio = (f->inicio + 1) % f->capacidade;
    f->tamanho--;
    return v;
}

static void fila_liberar(FilaInt *f) {
    free(f->dados);
    f->dados = NULL;
}

/*
 * Objetivo     : Calcula a distancia em saltos de fronteira a partir de uma UF.
 * Algoritmo    : Busca em Largura (BFS) com fila circular FIFO e vetor nivel[].
 * Detalhe-Chave: Complexidade O(V + E); agrupa UFs por camadas de distancia.
 */
void busca_largura_propagacao_fronteiras(Grafo *g, int origem) {
    int n, i, k, u, v, max_nivel, primeiro;
    int *visitado;
    int *nivel;
    FilaInt fila;
    const No *a;

    if (g == NULL) {
        fprintf(stderr, "BFS: grafo nulo.\n");
        return;
    }

    n = g->num_vertices;
    if (n <= 0 || origem < 0 || origem >= n) {
        fprintf(stderr, "BFS: origem invalida (%d).\n", origem);
        return;
    }

    visitado = calloc((size_t)n, sizeof(int));
    nivel = malloc((size_t)n * sizeof(int));
    if (visitado == NULL || nivel == NULL) {
        fprintf(stderr, "BFS: falha de memoria.\n");
        free(visitado);
        free(nivel);
        return;
    }

    if (!fila_criar(&fila, n)) {
        fprintf(stderr, "BFS: falha de memoria.\n");
        free(visitado);
        free(nivel);
        return;
    }

    for (i = 0; i < n; i++) {
        nivel[i] = -1;
    }

    /* A origem entra com nível 0. */
    visitado[origem] = 1;
    nivel[origem] = 0;
    fila_enfileirar(&fila, origem);
    max_nivel = 0;

    while (!fila_vazia(&fila)) {
        u = fila_desenfileirar(&fila);
        for (a = g->adj[u].cabeca; a != NULL; a = a->prox) {
            v = a->destino;
            if (!visitado[v]) {
                visitado[v] = 1;
                nivel[v] = nivel[u] + 1;
                if (nivel[v] > max_nivel) {
                    max_nivel = nivel[v];
                }
                fila_enfileirar(&fila, v);
            }
        }
    }

    /* Imprime as camadas. */
    for (k = 0; k <= max_nivel; k++) {
        printf("Camada %d:", k);
        primeiro = 1;
        for (v = 0; v < n; v++) {
            if (nivel[v] == k) {
                printf("%s%s", primeiro ? " " : ", ", g->rotulos[v]);
                primeiro = 0;
            }
        }
        printf("\n");
    }

    /* Estados desconectados da origem. */
    primeiro = 1;
    for (v = 0; v < n; v++) {
        if (nivel[v] == -1) {
            printf("%s%s", primeiro ? "Nao alcancados: " : ", ", g->rotulos[v]);
            primeiro = 0;
        }
    }
    if (!primeiro) {
        printf("\n");
    }

    fila_liberar(&fila);
    free(visitado);
    free(nivel);
}
