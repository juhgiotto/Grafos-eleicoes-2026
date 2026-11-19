
# Formatos do eixo nacional (G_Terr e G_Pres)

Lidos por `carregar_g_terr` e `carregar_g_pres` (`src/analise_nacional.c`).

## Fronteiras: `dados/fronteiras_estados_2026.csv`

| Aspecto | Regra |
|---|---|
| Separador de campos | `;` |
| Separador de vizinhos | `,` |
| Cabeçalho | `UF;FRONTEIRAS` (descartado) |
| Linha de dados | `<UF>;<viz 1>,<viz 2>,...` (ex.: `DF;GO,MG`) |

- 27 UFs; a relação é simétrica (conferido: 51 fronteiras).
- `G_Terr`: um vértice por UF e uma aresta não-orientada de peso 1 por fronteira.
- Vizinho inexistente no arquivo: aviso em `stderr` e fronteira ignorada.

> Observação: o enunciado sugere `uf,vizinho1;vizinho2;...`; foi mantido o formato do
> arquivo real (`;` entre UF e lista, `,` entre vizinhos).

## Votos: `dados/votacao_presidente_estados_2026.csv`

Formato **largo**, como o do DF: `UF;<candidato 1>;...` e `<UF>;<% 1>;...`, percentual em
`[0, 100]` com ponto decimal. A linha `ZZ` (exterior) não é UF e é ignorada.

## Métrica de divergência de `G_Pres`

Grafo completo (27 UFs → 351 arestas). Peso da aresta entre as UFs A e B:

`d(A, B) = sqrt( soma_c ( pct_A[c] - pct_B[c] )^2 )`

(distância euclidiana entre os vetores de percentuais). Quanto maior, mais divergentes
são os votos das duas UFs. Linhas malformadas ou UF repetida são ignoradas com aviso.