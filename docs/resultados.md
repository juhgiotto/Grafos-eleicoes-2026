# Resultados com Dados Reais — Eleições 2026

## Comando de execução (WSL / Linux)

```bash
make && printf "3\n0.98\nDF\n15.0\n0\n" | valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 ./eleicoes_grafos \
  dados/votacao_governador_df_2026.csv \
  dados/fronteiras_estados_2026.csv \
  dados/votacao_presidente_estados_2026.csv
```

## Análise dos resultados

### Eixo DF (`dados/votacao_governador_df_2026.csv`)
- **Candidatos dominantes por RA**: **Celina Leão** lidera em 14 das 16 RAs (destaque para Samambaia com 55,02%, Recanto das Emas com 54,17% e Ceilândia com 53,35%), enquanto **Leandro Grass** lidera em **Asa Norte** (46,62%) e **Plano Piloto** (42,62%).
- **Grafo de similaridade entre RAs (limiar `0.98`)**:
  - A maioria das RAs forma um bloco densamente conectado (12 a 14 vizinhos com similaridade >= 0,98).
  - **Asa Norte** (2 arestas: Plano Piloto e Cruzeiro) e **Plano Piloto** (5 arestas) apresentam menor conectividade nesse limiar, refletindo o perfil de voto distinto das demais RAs.

### Eixo Nacional (`dados/fronteiras_estados_2026.csv` + `dados/votacao_presidente_estados_2026.csv`)
- **BFS em `G_Terr` (origem `DF`)**: atinge todas as 27 UFs em 5 camadas (`Camada 0: DF` → `Camada 1: GO, MG` → `Camada 2: BA, ES, MS, MT, RJ, SP, TO` → ... → `Camada 5: RN, RS`).
- **DFS de clusters em `G_Pres` (limiar `15.0`)**: todas as 27 UFs permanecem em uma única componente conexa (`Total de componentes: 1`).
- **AGM via Kruskal em `G_Pres`**: 26 arestas (`V - 1`), peso total `91.8964`. Os pares eleitoralmente mais próximos são `MA - PE` (`0.6567`), `CE - PE` (`0.7011`) e `PE - SE` (`1.1024`), enquanto `DF - GO` (`10.7042`) fecha a árvore.

### Valgrind
- `1,478 allocs, 1,478 frees, 63,155 bytes allocated`
- `in use at exit: 0 bytes in 0 blocks`
- `ERROR SUMMARY: 0 errors from 0 contexts`

## Saída capturada do terminal

```text
=== Analise Eleitoral com Grafos (2026) ===
1 - Eixo DF (dominantes por RA e similaridade)
2 - Eixo Nacional (BFS fronteiras, DFS clusters e Kruskal)
3 - Executar ambos os eixos
0 - Sair
Escolha uma opcao: 
=== EIXO DF ===

--- Candidatos dominantes por RA ---
Aguas Claras -> CELINA LEAO HIZIM FERREIRA (48.09%)
Asa Norte -> LEANDRO ANTÔNIO GRASS PEIXOTO (46.62%)
Ceilandia -> CELINA LEAO HIZIM FERREIRA (53.35%)
Cruzeiro -> CELINA LEAO HIZIM FERREIRA (43.77%)
Gama -> CELINA LEAO HIZIM FERREIRA (49.46%)
Guara -> CELINA LEAO HIZIM FERREIRA (47.74%)
Lago Sul -> CELINA LEAO HIZIM FERREIRA (48.77%)
Nucleo Bandeirante -> CELINA LEAO HIZIM FERREIRA (52.64%)
Paranoa -> CELINA LEAO HIZIM FERREIRA (45.90%)
Planaltina -> CELINA LEAO HIZIM FERREIRA (51.02%)
Plano Piloto -> LEANDRO ANTÔNIO GRASS PEIXOTO (42.62%)
Recanto das Emas -> CELINA LEAO HIZIM FERREIRA (54.17%)
Samambaia -> CELINA LEAO HIZIM FERREIRA (55.02%)
Santa Maria -> CELINA LEAO HIZIM FERREIRA (51.32%)
Sobradinho -> CELINA LEAO HIZIM FERREIRA (50.30%)
Taguatinga -> CELINA LEAO HIZIM FERREIRA (52.82%)

Informe o limiar de similaridade de cosseno (0.0 a 1.0): 
--- Grafo de similaridade entre RAs (limiar >= 0.98) ---
[0] Aguas Claras (14 arestas): -> Taguatinga (0.99) -> Sobradinho (1.00) -> Santa Maria (1.00) -> Samambaia (0.99) -> Recanto das Emas (0.99) -> Plano Piloto (0.98) -> Planaltina (1.00) -> Paranoa (1.00) -> Nucleo Bandeirante (1.00) -> Lago Sul (1.00) -> Guara (1.00) -> Gama (1.00) -> Cruzeiro (1.00) -> Ceilandia (0.99)
[1] Asa Norte (2 arestas): -> Plano Piloto (1.00) -> Cruzeiro (0.98)
[2] Ceilandia (12 arestas): -> Taguatinga (1.00) -> Sobradinho (1.00) -> Santa Maria (1.00) -> Samambaia (1.00) -> Recanto das Emas (1.00) -> Planaltina (1.00) -> Paranoa (0.99) -> Nucleo Bandeirante (1.00) -> Lago Sul (0.99) -> Guara (0.99) -> Gama (1.00) -> Aguas Claras (0.99)
[3] Cruzeiro (12 arestas): -> Taguatinga (0.98) -> Sobradinho (0.99) -> Santa Maria (0.98) -> Plano Piloto (0.99) -> Planaltina (0.99) -> Paranoa (1.00) -> Nucleo Bandeirante (0.98) -> Lago Sul (1.00) -> Guara (1.00) -> Gama (0.99) -> Asa Norte (0.98) -> Aguas Claras (1.00)
[4] Gama (13 arestas): -> Taguatinga (1.00) -> Sobradinho (1.00) -> Santa Maria (1.00) -> Samambaia (0.99) -> Recanto das Emas (0.99) -> Planaltina (1.00) -> Paranoa (1.00) -> Nucleo Bandeirante (1.00) -> Lago Sul (1.00) -> Guara (1.00) -> Cruzeiro (0.99) -> Ceilandia (1.00) -> Aguas Claras (1.00)
[5] Guara (14 arestas): -> Taguatinga (0.99) -> Sobradinho (1.00) -> Santa Maria (0.99) -> Samambaia (0.99) -> Recanto das Emas (0.99) -> Plano Piloto (0.98) -> Planaltina (1.00) -> Paranoa (1.00) -> Nucleo Bandeirante (0.99) -> Lago Sul (1.00) -> Gama (1.00) -> Cruzeiro (1.00) -> Ceilandia (0.99) -> Aguas Claras (1.00)
[6] Lago Sul (13 arestas): -> Taguatinga (1.00) -> Sobradinho (1.00) -> Santa Maria (1.00) -> Samambaia (0.99) -> Recanto das Emas (0.99) -> Planaltina (1.00) -> Paranoa (1.00) -> Nucleo Bandeirante (1.00) -> Guara (1.00) -> Gama (1.00) -> Cruzeiro (1.00) -> Ceilandia (0.99) -> Aguas Claras (1.00)
[7] Nucleo Bandeirante (13 arestas): -> Taguatinga (1.00) -> Sobradinho (1.00) -> Santa Maria (1.00) -> Samambaia (1.00) -> Recanto das Emas (1.00) -> Planaltina (1.00) -> Paranoa (0.99) -> Lago Sul (1.00) -> Guara (0.99) -> Gama (1.00) -> Cruzeiro (0.98) -> Ceilandia (1.00) -> Aguas Claras (1.00)
[8] Paranoa (14 arestas): -> Taguatinga (0.99) -> Sobradinho (1.00) -> Santa Maria (0.99) -> Samambaia (0.98) -> Recanto das Emas (0.99) -> Plano Piloto (0.98) -> Planaltina (1.00) -> Nucleo Bandeirante (0.99) -> Lago Sul (1.00) -> Guara (1.00) -> Gama (1.00) -> Cruzeiro (1.00) -> Ceilandia (0.99) -> Aguas Claras (1.00)
[9] Planaltina (13 arestas): -> Taguatinga (1.00) -> Sobradinho (1.00) -> Santa Maria (1.00) -> Samambaia (0.99) -> Recanto das Emas (1.00) -> Paranoa (1.00) -> Nucleo Bandeirante (1.00) -> Lago Sul (1.00) -> Guara (1.00) -> Gama (1.00) -> Cruzeiro (0.99) -> Ceilandia (1.00) -> Aguas Claras (1.00)
[10] Plano Piloto (5 arestas): -> Paranoa (0.98) -> Guara (0.98) -> Cruzeiro (0.99) -> Asa Norte (1.00) -> Aguas Claras (0.98)
[11] Recanto das Emas (12 arestas): -> Taguatinga (1.00) -> Sobradinho (0.99) -> Santa Maria (1.00) -> Samambaia (1.00) -> Planaltina (1.00) -> Paranoa (0.99) -> Nucleo Bandeirante (1.00) -> Lago Sul (0.99) -> Guara (0.99) -> Gama (0.99) -> Ceilandia (1.00) -> Aguas Claras (0.99)
[12] Samambaia (12 arestas): -> Taguatinga (1.00) -> Sobradinho (0.99) -> Santa Maria (1.00) -> Recanto das Emas (1.00) -> Planaltina (0.99) -> Paranoa (0.98) -> Nucleo Bandeirante (1.00) -> Lago Sul (0.99) -> Guara (0.99) -> Gama (0.99) -> Ceilandia (1.00) -> Aguas Claras (0.99)
[13] Santa Maria (13 arestas): -> Taguatinga (1.00) -> Sobradinho (1.00) -> Samambaia (1.00) -> Recanto das Emas (1.00) -> Planaltina (1.00) -> Paranoa (0.99) -> Nucleo Bandeirante (1.00) -> Lago Sul (1.00) -> Guara (0.99) -> Gama (1.00) -> Cruzeiro (0.98) -> Ceilandia (1.00) -> Aguas Claras (1.00)
[14] Sobradinho (13 arestas): -> Taguatinga (1.00) -> Santa Maria (1.00) -> Samambaia (0.99) -> Recanto das Emas (0.99) -> Planaltina (1.00) -> Paranoa (1.00) -> Nucleo Bandeirante (1.00) -> Lago Sul (1.00) -> Guara (1.00) -> Gama (1.00) -> Cruzeiro (0.99) -> Ceilandia (1.00) -> Aguas Claras (1.00)
[15] Taguatinga (13 arestas): -> Sobradinho (1.00) -> Santa Maria (1.00) -> Samambaia (1.00) -> Recanto das Emas (1.00) -> Planaltina (1.00) -> Paranoa (0.99) -> Nucleo Bandeirante (1.00) -> Lago Sul (1.00) -> Guara (0.99) -> Gama (1.00) -> Cruzeiro (0.98) -> Ceilandia (1.00) -> Aguas Claras (0.99)

=== EIXO NACIONAL ===

Informe a sigla da UF de origem para a BFS (ex.: DF, SP, AC): Camada 0: DF
Camada 1: GO, MG
Camada 2: BA, ES, MS, MT, RJ, SP, TO
Camada 3: AL, AM, MA, PA, PE, PI, PR, RO, SE
Camada 4: AC, AP, CE, PB, RR, SC
Camada 5: RN, RS

Informe o limiar de distancia euclidiana para clusters (DFS):
Clusters de G_Pres (limiar de distancia <= 15.0000):
Componente 1: AC ES AM AL CE BA PI MA PB PE RN SE PA AP DF GO MG RJ SP TO MS MT PR RO RR RS SC
Total de componentes: 1

AGM (Kruskal) de G_Pres:
  MA - PE (peso 0.6567)
  CE - PE (peso 0.7011)
  PE - SE (peso 1.1024)
  AC - MT (peso 1.1576)
  RO - SC (peso 1.5795)
  AM - PA (peso 2.0313)
  RJ - SP (peso 2.1301)
  PB - RN (peso 2.3137)
  ES - RS (peso 2.3410)
  ES - RJ (peso 2.4668)
  MG - TO (peso 2.6361)
  CE - PB (peso 2.7153)
  DF - SP (peso 2.8045)
  AM - AP (peso 2.8244)
  BA - MA (peso 3.2251)
  MS - RS (peso 3.3307)
  AP - MG (peso 3.6818)
  MS - PR (peso 3.9302)
  MT - RO (peso 4.0785)
  RO - RR (peso 4.7480)
  RJ - TO (peso 4.8499)
  AC - PR (peso 5.4108)
  AL - PA (peso 6.3118)
  BA - PI (peso 6.5684)
  AL - RN (peso 7.5964)
  DF - GO (peso 10.7042)
Arestas na AGM: 26 (esperado V-1 = 26)
Peso total: 91.8964

=== Analise Eleitoral com Grafos (2026) ===
1 - Eixo DF (dominantes por RA e similaridade)
2 - Eixo Nacional (BFS fronteiras, DFS clusters e Kruskal)
3 - Executar ambos os eixos
0 - Sair
Escolha uma opcao: Encerrando.
==645== 
==645== HEAP SUMMARY:
==645==     in use at exit: 0 bytes in 0 blocks
==645==   total heap usage: 1,478 allocs, 1,478 frees, 63,155 bytes allocated
==645==
==645== All heap blocks were freed -- no leaks are possible
==645==
==645== For lists of detected and suppressed errors, rerun with: -s
==645== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```
