#ifndef ANALISE_DF_H
#define ANALISE_DF_H

#include "grafo.h"

/*
 * Lê o CSV de votação para Governador do DF e monta o grafo bipartido
 * RA x Candidato, não-orientado e ponderado pelo percentual de votos.
 *
 * Formato esperado (ver docs/formato_csv.md):
 *   - separador ';', terminadores LF ou CRLF;
 *   - cabeçalho: "RA;<candidato 1>;<candidato 2>;...";
 *   - linhas de dados: "<nome da RA>;<pct 1>;<pct 2>;..." (pct em [0, 100],
 *     com ponto decimal).
 *
 * Linhas malformadas (quantidade de campos diferente da do cabeçalho, RA
 * vazia, percentual não numérico ou fora de [0, 100]) são ignoradas e
 * reportadas em stderr.
 *
 * Parâmetros:
 *   caminho: caminho do arquivo CSV.
 * Retorno:
 *   Ponteiro para o Grafo (o chamador deve liberar com liberar_grafo), ou
 *   NULL se o arquivo não existir/não puder ser aberto, se o cabeçalho for
 *   inválido ou em caso de falha de alocação.
 */
Grafo *construir_grafo_bipartido_df(const char *caminho);

/*
 * Identifica e imprime o candidato dominante (maior percentual de votos) em
 * cada Região Administrativa (RA) do grafo bipartido.
 *
 * Identificação de RAs:
 *   Usa coloração bipartite (BFS a partir do vértice 0, assumindo que o
 *   vértice 0 é uma RA, conforme construído por construir_grafo_bipartido_df).
 *   Vértices na partição 0 com ao menos uma aresta são tratados como RAs.
 *
 * Critério de empate:
 *   Empates (diferença de peso < 1e-4 em relação ao máximo da RA) são
 *   reportados explicitamente listando todos os candidatos empatados.
 *
 * Parâmetros:
 *   g: ponteiro para o grafo bipartido RA x Candidato (ignora NULL).
 */
void candidatos_dominantes_por_ra(const Grafo *g);

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