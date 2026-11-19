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
 * Calcula a similaridade de cosseno entre dois vetores de tamanho `n`.
 *
 * Justificativa da métrica:
 *   Cada RA é representada por um vetor de percentuais de voto na mesma ordem
 *   de candidatos. A similaridade de cosseno mede o ângulo entre esses perfis
 *   (intervalo [0, 1] para percentuais não-negativos), capturando a proporção
 *   relativa entre candidatos mesmo quando a soma dos percentuais varia entre
 *   RAs (ex.: votos em outros candidatos ou brancos/nulos), sendo adequada para
 *   a projeção ponderada do grafo bipartido sobre o conjunto de RAs.
 *
 * Parâmetros:
 *   a, b: vetores de tamanho `n`.
 *   n: dimensão dos vetores (deve ser > 0).
 * Retorno:
 *   Similaridade de cosseno em [0.0, 1.0] (ou [-1.0, 1.0] em geral), ou 0.0f
 *   se `a` ou `b` forem NULL, se `n <= 0` ou se algum vetor tiver norma zero.
 */
float similaridade_cosseno(const float *a, const float *b, int n);

/*
 * Extrai o vetor de percentuais de uma RA sobre os `num_candidatos` índices
 * em `candidatos` (na mesma ordem), preenchendo `saida` (tamanho `num_candidatos`).
 * Retorna 0 em sucesso ou -1 em caso de parâmetros inválidos.
 */
int extrair_vetor_ra(const Grafo *g, int ra, const int *candidatos,
                     int num_candidatos, float *saida);

/*
 * Projeta o grafo bipartido RA x Candidato em um grafo monopartido de RAs,
 * não-orientado, ligando pares de RAs cuja similaridade de cosseno seja
 * maior ou igual a `limiar_similaridade`.
 *
 * O grafo retornado possui cópias próprias dos rótulos das RAs, sendo
 * totalmente independente de `g_bipartido` (deve ser liberado com
 * liberar_grafo).
 *
 * Parâmetros:
 *   g_bipartido: grafo bipartido RA x Candidato.
 *   limiar_similaridade: valor mínimo de similaridade para criar aresta.
 * Retorno:
 *   Ponteiro para o novo Grafo de RAs, ou NULL em caso de erro.
 */
Grafo *gerar_grafo_similaridade_ra(const Grafo *g_bipartido,
                                   float limiar_similaridade);

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