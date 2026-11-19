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