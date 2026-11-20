#ifndef ANALISE_NACIONAL_H
#define ANALISE_NACIONAL_H

/*
 * =========================================================================
 * Modulo: Eixo Nacional (include/analise_nacional.h)
 * Papel : Interface das analises interestaduais (27 UFs do Brasil).
 * Teoria: Grafo de Contiguidade (BFS), Grafo Completo de Distancia Eleitoral,
 *         Componentes Conexas (DFS com limiar) e AGM (Kruskal + Union-Find).
 * =========================================================================
 */

#include "grafo.h"

/* #8 - Carga dos grafos nacionais */
Grafo* carregar_grafo_territorial(const char *arquivo);
Grafo* carregar_grafo_presidencial(const char *arquivo);

/* #9 - Propagacao de fronteiras em camadas via BFS */
void busca_largura_propagacao_fronteiras(Grafo *g_terr, int estado_origem);

/* #10 - Agrupamento de UFs por alinhamento eleitoral via DFS */
void busca_profundidade_clusters(Grafo *g_pres, float limiar_corte);

/* #11 - Arvore Geradora Minima de proximidade eleitoral via Kruskal */
void calcular_mst_eleitoral_kruskal(Grafo *g_pres);

#endif /* ANALISE_NACIONAL_H */
