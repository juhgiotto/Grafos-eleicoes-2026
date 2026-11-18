# Formato do CSV de votação para Governador do DF

Arquivo lido por `construir_grafo_bipartido_df` (`src/analise_df.c`):
`dados/votacao_governador_df_2026.csv`.

## Estrutura

Formato **largo**: uma linha por RA e uma coluna por candidato.

| Aspecto | Regra |
|---|---|
| Separador | `;` |
| Terminador de linha | LF ou CRLF (ambos aceitos) |
| Codificação | UTF-8 (BOM inicial tolerado) |
| Decimal | ponto (`48.09`) |
| Cabeçalho | `RA;<candidato 1>;<candidato 2>;...` |
| Linha de dados | `<nome da RA>;<% candidato 1>;<% candidato 2>;...` |
| Percentual | número em `[0, 100]` |

Exemplo (dataset sintético, `tests/dados_sinteticos/votacao_df_sintetico.csv`):

```
RA;CELINA LEAO HIZIM FERREIRA;LEANDRO ANTÔNIO GRASS PEIXOTO;PAULA MORENO PARO BELMONTE
Aguas Claras;48.09;35.45;7.81
Asa Norte;36.68;46.62;6.47
```

> Observação: o formato sugerido no enunciado (`ra,candidato,percentual`, formato
> longo) não foi adotado porque o arquivo real fornecido já está no formato largo.
> Cada célula `(RA, candidato)` do formato largo equivale a uma linha do formato longo.

## O cabeçalho é lido, não ignorado

Como cada candidato é uma coluna, o cabeçalho fornece os **nomes dos candidatos**
(rótulos dos vértices). Ele é validado: a primeira coluna deve ser `RA` e deve haver
ao menos um candidato (máximo 64). Cabeçalho inválido faz a função retornar `NULL`.

## Grafo gerado

- Não-orientado e ponderado; peso da aresta = percentual de votos.
- Vértices: um por RA e um por candidato (rótulo = texto do CSV).
- Uma aresta `(RA, candidato)` por célula válida; como o grafo é não-orientado, cada
  aresta aparece nas listas dos dois vértices.
- Bipartido: nunca há aresta RA–RA nem candidato–candidato.

## Tratamento de erros

| Situação | Comportamento |
|---|---|
| Arquivo inexistente / sem permissão | mensagem em `stderr`, retorna `NULL` |
| Cabeçalho ausente ou inválido | mensagem em `stderr`, retorna `NULL` |
| Falha de alocação | libera o que foi alocado, retorna `NULL` |
| Linha com nº de campos diferente do cabeçalho (inclui campo vazio) | aviso em `stderr` com o nº da linha, linha ignorada |
| Nome da RA vazio | linha ignorada |
| Percentual não numérico, negativo ou > 100 | linha ignorada |
| Linha em branco | ignorada em silêncio |
| Linha com mais de 4095 caracteres | aviso, linha ignorada |
| RA repetida | as arestas já existentes são mantidas (a repetição não altera pesos) |

Uma linha malformada é descartada **inteira**: nenhum vértice ou aresta dela entra no
grafo.
