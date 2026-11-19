#include <assert.h>
#include <stdio.h>

#include "../include/grafo.h"

/* Verifica se existe aresta para `destino` com o `peso` esperado na lista. */
static int contem_aresta(const Lista *lista, int destino, float peso) {
    const No *atual = lista->cabeca;
    while (atual != NULL) {
        if (atual->destino == destino && atual->peso == peso) {
            return 1;
        }
        atual = atual->prox;
    }
    return 0;
}

static void test_grafo_vazio(void) {
    Grafo *g;

    assert(criar_grafo(0, 0) == NULL);
    assert(criar_grafo(-5, 1) == NULL);

    g = criar_grafo(4, 0);
    assert(g != NULL);
    assert(g->num_vertices == 0);
    assert(g->capacidade == 4);
    assert(g->orientado == 0);

    imprimir_grafo(g);
    imprimir_grafo(NULL);
    liberar_grafo(g);
    liberar_grafo(NULL);
}

static void test_insercao_e_busca_vertice(void) {
    Grafo *g = criar_grafo(4, 1);
    assert(g != NULL);

    assert(adicionar_vertice(g, "DF") == 0);
    assert(adicionar_vertice(g, "GO") == 1);
    assert(adicionar_vertice(g, "MG") == 2);
    assert(g->num_vertices == 3);

    assert(buscar_vertice(g, "DF") == 0);
    assert(buscar_vertice(g, "GO") == 1);
    assert(buscar_vertice(g, "MG") == 2);
    assert(buscar_vertice(g, "SP") == -1);
    assert(buscar_vertice(g, NULL) == -1);
    assert(buscar_vertice(NULL, "DF") == -1);

    liberar_grafo(g);
}

static void test_vertice_duplicado(void) {
    Grafo *g = criar_grafo(4, 0);
    assert(g != NULL);

    assert(adicionar_vertice(g, "DF") == 0);
    assert(adicionar_vertice(g, "DF") == -1);
    assert(g->num_vertices == 1);
    assert(adicionar_vertice(g, NULL) == -1);

    liberar_grafo(g);
}

static void test_aresta_nao_orientada_e_repetida(void) {
    Grafo *g;
    int df, go, mg;

    g = criar_grafo(4, 0);
    assert(g != NULL);

    df = adicionar_vertice(g, "DF");
    go = adicionar_vertice(g, "GO");
    mg = adicionar_vertice(g, "MG");
    assert(df == 0 && go == 1 && mg == 2);

    assert(adicionar_aresta(g, df, go, 12.5f) == 0);
    assert(g->adj[df].num_arestas == 1);
    assert(g->adj[go].num_arestas == 1);
    assert(contem_aresta(&g->adj[df], go, 12.5f));
    assert(contem_aresta(&g->adj[go], df, 12.5f));

    /* Aresta repetida não deve duplicar nós */
    assert(adicionar_aresta(g, df, go, 12.5f) == 0);
    assert(adicionar_aresta(g, go, df, 12.5f) == 0);
    assert(g->adj[df].num_arestas == 1);
    assert(g->adj[go].num_arestas == 1);

    /* Índices inválidos e ponteiro nulo */
    assert(adicionar_aresta(g, -1, go, 1.0f) == -1);
    assert(adicionar_aresta(g, df, 3, 1.0f) == -1);
    assert(adicionar_aresta(NULL, df, go, 1.0f) == -1);

    liberar_grafo(g);
}

static void test_aresta_orientada(void) {
    Grafo *g;
    int a, b;

    g = criar_grafo(4, 1);
    assert(g != NULL);

    a = adicionar_vertice(g, "A");
    b = adicionar_vertice(g, "B");
    assert(adicionar_aresta(g, a, b, 3.0f) == 0);

    assert(g->adj[a].num_arestas == 1);
    assert(g->adj[b].num_arestas == 0);
    assert(contem_aresta(&g->adj[a], b, 3.0f));
    assert(!contem_aresta(&g->adj[b], a, 3.0f));

    liberar_grafo(g);
}

static void test_crescimento_dinamico(void) {
    Grafo *g;
    char nome[16];
    int i, idx;

    g = criar_grafo(2, 0);
    assert(g != NULL);

    for (i = 0; i < 100; i++) {
        sprintf(nome, "V%02d", i); /* snprintf e C99; "V%02d" cabe em 16 bytes */
        idx = adicionar_vertice(g, nome);
        assert(idx == i);
    }

    assert(g->num_vertices == 100);
    assert(g->capacidade >= 100);
    assert(buscar_vertice(g, "V00") == 0);
    assert(buscar_vertice(g, "V50") == 50);
    assert(buscar_vertice(g, "V99") == 99);

    assert(adicionar_aresta(g, 0, 99, 1.5f) == 0);
    assert(contem_aresta(&g->adj[0], 99, 1.5f));
    assert(contem_aresta(&g->adj[99], 0, 1.5f));

    liberar_grafo(g);
}

int main(void) {
    test_grafo_vazio();
    test_insercao_e_busca_vertice();
    test_vertice_duplicado();
    test_aresta_nao_orientada_e_repetida();
    test_aresta_orientada();
    test_crescimento_dinamico();

    printf("Todos os testes de grafo passaram!\n");
    return 0;
}
