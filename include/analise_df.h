#ifndef ANALISE_DF_H
#define ANALISE_DF_H

/*
 * =========================================================================
 * Modulo: Eixo DF e Carga Nacional (include/analise_df.h)
 * Papel : Interface das analises do Distrito Federal e leitores de CSV.
 * Teoria: Grafo Bipartido (RA x Candidato), Similaridade de Cosseno e
 *         Projecao Monopartida em grafo de similaridade entre RAs.
 * =========================================================================
 */

#include "grafo.h"

/* Constroi o grafo bipartido nao-orientado RA x Candidato a partir do CSV do DF. */
Grafo *construir_grafo_bipartido_df(const char *caminho);

/* Identifica e imprime o candidato vencedor (ou empate) em cada RA do DF. */
void candidatos_dominantes_por_ra(const Grafo *g);

/* Calcula a similaridade de cosseno em [0, 1] entre dois vetores de votos. */
float similaridade_cosseno(const float *a, const float *b, int n);

/* Extrai o vetor de percentuais de uma RA na mesma ordem de candidatos. */
int extrair_vetor_ra(const Grafo *g, int ra, const int *candidatos,
                     int num_candidatos, float *saida);

/* Projeta o grafo bipartido em um grafo monopartido de RAs via limiar de cosseno. */
Grafo *gerar_grafo_similaridade_ra(const Grafo *g_bipartido,
                                   float limiar_similaridade);

/* Leitores do Eixo Nacional (implementados em src/analise_nacional.c). */
Grafo *carregar_g_terr(const char *caminho_fronteiras);
Grafo *carregar_g_pres(const char *caminho_votos);

#endif /* ANALISE_DF_H */
