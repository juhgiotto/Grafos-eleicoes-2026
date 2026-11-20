# Documentação dos Dados — Eleições 2026

## Origem e arquivos brutos (fora do Git via `.gitignore`)

Os dados consolidados em `dados/` derivam das seguintes fontes oficiais:

1. **TSE — Dados Abertos (`dadosabertos.tse.jus.br`)**:
   - `votacao_secao_2026_BR.csv`: votação por seção eleitoral para Presidente (1º turno, 2026).
   - `votacao_secao_2026_DF.csv`: votação por seção eleitoral no Distrito Federal (1º turno, 2026).
   - `consulta_cand_2026.csv`: metadados de candidatos (nome na urna / nome completo).
2. **IBGE — Malha Territorial**:
   - Contiguidade interestadual das 27 Unidades da Federação (UFs).
3. **TRE-DF / IPEDF**:
   - Mapeamento Zonas Eleitorais × Regiões Administrativas (RAs) do Distrito Federal.

Data de consolidação no repositório: **15/11/2026** (commit `960504c`).

## CSVs consolidados versionados em `dados/`

| Arquivo | Dimensões | Soma dos percentuais por linha | Observações |
|---|---|---|---|
| `dados/votacao_governador_df_2026.csv` | 16 RAs × 11 candidatos | 99,99% – 100,01% | Separador `;`, decimal `.` |
| `dados/votacao_presidente_estados_2026.csv` | 28 linhas (27 UFs + `ZZ`) × 13 candidatos | 99,97% – 100,02% | Linha `ZZ` (exterior) ignorada por `carregar_g_pres` |
| `dados/fronteiras_estados_2026.csv` | 27 UFs | — | `UF;FRONTEIRAS` com vizinhos separados por `,` |

Para detalhes do esquema de colunas e regras de validação, consulte [formato_csv.md](formato_csv.md).
