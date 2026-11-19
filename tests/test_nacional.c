#include <assert.h>
#include <stdio.h>
#include "grafo.h"
#include "analise_nacional.h"

static int contar_arestas(const Grafo *g) {
    int total = 0;
    for (int i = 0; i < g->num_vertices; i++) total += g->adj[i].num_arestas;
    return g->orientado ? total : total / 2;
}

int main(void) {
    /* ---- #8 ---- */
    Grafo *terr = carregar_grafo_territorial("dados/processados/fronteiras_estados_2026.csv");
    Grafo *pres = carregar_grafo_presidencial("dados/processados/votacao_presidente_estados_2026.csv");
    assert(terr && pres);
    assert(pres->num_vertices == 5);
    assert(contar_arestas(pres) == 10);
    imprimir_grafo(terr);

    /* ---- #9 aqui ---- */

    /* ---- #10 aqui ---- */

    /* ---- #11 aqui ---- */

    liberar_grafo(terr);
    liberar_grafo(pres);
    printf("OK\n");
    return 0;
}