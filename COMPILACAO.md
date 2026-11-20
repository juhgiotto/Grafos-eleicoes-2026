# Tutorial de Compilação e Execução — Eleições 2026

Este guia cobre os três cenários possíveis para compilar e executar o projeto (`eleicoes_grafos`) e os testes unitários no Windows ou Linux.

---

## Cenário 1: Já tem o Ubuntu (WSL) instalado

1. Abra o terminal do **Ubuntu (WSL)** e vá até a pasta do projeto:
   ```bash
   cd /mnt/c/Users/lfsrl/Documents/Grafos-eleicoes-2026
   ```
   *(Ajuste o caminho se o projeto estiver em outra pasta do seu computador.)*

2. Garanta que `gcc`, `make` e `valgrind` estão instalados:
   ```bash
   sudo apt update && sudo apt install -y build-essential valgrind
   ```

3. Compile o programa e os testes:
   ```bash
   make clean && make
   make test
   ```

4. Execute o programa principal com os dados oficiais:
   ```bash
   ./eleicoes_grafos dados/votacao_governador_df_2026.csv dados/fronteiras_estados_2026.csv dados/votacao_presidente_estados_2026.csv
   ```

5. *(Opcional)* Rode o `valgrind` para verificar vazamentos de memória:
   ```bash
   make valgrind
   ```

---

## Cenário 2: Não tem o Ubuntu instalado e quer instalar (WSL)

Recomendado se quiser usar `make` e `valgrind` sem complicação no Windows.

1. Abra o **PowerShell** ou **Prompt de Comando (CMD)** e execute:
   ```powershell
   wsl --install
   ```
   *(Se o Windows pedir para reiniciar o computador após instalar o WSL, reinicie.)*

2. Após reiniciar, o **Ubuntu** abrirá automaticamente (ou procure por "Ubuntu" no Menu Iniciar) e pedirá para você criar um **nome de usuário** e **senha** para o Linux.

3. Com o terminal do Ubuntu aberto, entre na pasta do projeto (no WSL, o disco `C:` fica em `/mnt/c/`):
   ```bash
   cd /mnt/c/Users/lfsrl/Documents/Grafos-eleicoes-2026
   ```

4. Instale o compilador, o `make` e o `valgrind`:
   ```bash
   sudo apt update && sudo apt install -y build-essential valgrind
   ```

5. Compile e execute normalmente:
   ```bash
   make
   ./eleicoes_grafos dados/votacao_governador_df_2026.csv dados/fronteiras_estados_2026.csv dados/votacao_presidente_estados_2026.csv
   ```

---

## Cenário 3: Não quer instalar o Ubuntu (Windows nativo)

Você pode compilar diretamente no Windows usando o **GCC (MinGW-w64)**.
> **Observação:** O `valgrind` não existe nativamente no Windows; para checar vazamentos de memória com `valgrind`, é necessário usar Linux/WSL (Cenários 1 ou 2).

1. Abra o **PowerShell** e instale o GCC via `winget`:
   ```powershell
   winget install BrechtSanders.WinLibs.POSIX.UCRT
   ```

2. **Feche e reabra** o PowerShell / CMD / Git Bash para atualizar o `PATH`, e verifique se o `gcc` foi reconhecido:
   ```powershell
   gcc --version
   ```

3. Entre na pasta do projeto e compile manualmente (caso não tenha `make` instalado no Windows):
   ```powershell
   cd C:\Users\lfsrl\Documents\Grafos-eleicoes-2026
   gcc -ansi -pedantic -Wall -Wextra -Wdeclaration-after-statement -g -Iinclude src/main.c src/grafo.c src/analise_df.c src/analise_nacional.c -o eleicoes_grafos.exe -lm
   ```

4. Execute o programa no PowerShell ou CMD:
   ```powershell
   .\eleicoes_grafos.exe dados/votacao_governador_df_2026.csv dados/fronteiras_estados_2026.csv dados/votacao_presidente_estados_2026.csv
   ```

5. *(Opcional)* Para compilar e rodar os testes unitários no Windows nativo:
   ```powershell
   gcc -ansi -pedantic -Wall -Wextra -Wdeclaration-after-statement -g -Iinclude src/grafo.c tests/test_grafo.c -o test_grafo.exe
   gcc -ansi -pedantic -Wall -Wextra -Wdeclaration-after-statement -g -Iinclude src/grafo.c src/analise_df.c tests/test_analise_df.c -o test_analise_df.exe -lm
   .\test_grafo.exe
   .\test_analise_df.exe
   ```

---

## Resumo do Menu Interativo

Ao rodar `eleicoes_grafos`, o programa exibe:
- **`1`** — **Eixo DF**: mostra os candidatos dominantes por RA e gera o grafo de similaridade (pede o limiar de cosseno, ex.: `0.98`).
- **`2`** — **Eixo Nacional**: carrega `G_Terr` e `G_Pres`, executa BFS (pede a UF de origem, ex.: `DF`), DFS de clusters (pede o limiar de distância, ex.: `15.0`) e AGM via Kruskal.
- **`3`** — Executa **ambos** os eixos em sequência.
- **`0`** — Encerra o programa liberando toda a memória.

Para testar com o **dataset sintético do DF**, substitua apenas o primeiro arquivo por `tests/dados_sinteticos/votacao_df_sintetico.csv`.
