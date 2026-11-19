#ifndef ANALISE_NACIONAL_H
#define ANALISE_NACIONAL_H

#include "grafo.h"

/* #8 */
Grafo* carregar_grafo_territorial(const char *arquivo);
Grafo* carregar_grafo_presidencial(const char *arquivo);

/* #9 */
void busca_largura_propagacao_fronteiras(Grafo *g_terr, int estado_origem);

/* #10 */
void busca_profundidade_clusters(Grafo *g_pres, float limiar_corte);

/* #11 */
void calcular_mst_eleitoral_kruskal(Grafo *g_pres);

#endif