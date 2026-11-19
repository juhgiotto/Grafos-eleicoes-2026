#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/grafo.h"

/* strdup nao e ANSI C (e POSIX): copia propria de string. */
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

int adicionar_aresta(Grafo *g, int origem, int destino, float peso) {
    No *removido;

    if (!g || origem < 0 || origem >= g->num_vertices ||
        destino < 0 || destino >= g->num_vertices) {
        return -1;
    }

    if (aresta_existe(&g->adj[origem], destino)) {
        return 0;
    }

    if (inserir_no(&g->adj[origem], destino, peso) != 0) {
        return -1;
    }

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
