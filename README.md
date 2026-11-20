# Grafos — Eleições 2026

Projeto em C (padrão ANSI C89) para análise de dados eleitorais usando grafos em listas de adjacência dinâmicas, dividido em dois eixos:

- **Eixo DF (`src/analise_df.c`)**: grafo bipartido `RA × Candidato`, candidatos dominantes por Região Administrativa (com tratamento de empate), similaridade de cosseno e projeção no grafo de similaridade entre RAs.
- **Eixo Nacional (`src/analise_nacional.c`)**: grafo de contiguidade territorial (`G_Terr`) com BFS por camadas de fronteira e grafo de distância eleitoral presidencial (`G_Pres`) com clusters via DFS e Árvore Geradora Mínima via Kruskal.

## Compilação

```bash
make
```

Isso gera o executável `eleicoes_grafos` com `-ansi -pedantic -Wall -Wextra -Wdeclaration-after-statement`.

## Execução

O programa recebe os caminhos dos três CSVs pela linha de comando:

```bash
./eleicoes_grafos <csv_governador_df> <csv_fronteiras> <csv_presidente>
```

### Exemplo com os dados completos

```bash
./eleicoes_grafos \
  dados/votacao_governador_df_2026.csv \
  dados/fronteiras_estados_2026.csv \
  dados/votacao_presidente_estados_2026.csv
```

### Exemplo com o dataset sintético do DF e verificação com Valgrind

```bash
printf "3\n0.95\nDF\n15.0\n0\n" | valgrind --leak-check=full --show-leak-kinds=all ./eleicoes_grafos \
  tests/dados_sinteticos/votacao_df_sintetico.csv \
  dados/fronteiras_estados_2026.csv \
  dados/votacao_presidente_estados_2026.csv
```

## Testes unitários

```bash
make test
make valgrind
```
