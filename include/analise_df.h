#ifndef ANALISE_DF_H
#define ANALISE_DF_H

#include "grafo.h"

/*
 * (comentário original de construir_grafo_bipartido_df, mantenha como está)
 */
Grafo *construir_grafo_bipartido_df(const char *caminho);

/*
 * Eixo nacional (src/analise_nacional.c; formatos em docs/formato_csv.md).
 * Ambos retornam um Grafo não-orientado que o chamador libera com
 * liberar_grafo, ou NULL em caso de erro.
 *   carregar_g_terr: contiguidade territorial (aresta = fronteira, peso 1).
 *   carregar_g_pres: grafo completo; peso = distância euclidiana entre os
 *                    vetores de percentuais de voto das duas UFs.
 */
Grafo *carregar_g_terr(const char *caminho_fronteiras);
Grafo *carregar_g_pres(const char *caminho_votos);

#endif /* ANALISE_DF_H */