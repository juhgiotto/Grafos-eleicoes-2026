#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/grafo.h"
#include "../include/analise_df.h"
#include "../include/analise_nacional.h"

#define TAM_ENTRADA 128

/* Remove '\n' e '\r' do final de uma string lida com fgets. */
static void limpar_quebra(char *s) {
    size_t n = strlen(s);
    while (n > 0 && (s[n - 1] == '\n' || s[n - 1] == '\r')) {
        s[--n] = '\0';
    }
}

/*
 * Le um inteiro da entrada padrao. Retorna 1 em sucesso e 0 em EOF/invalido.
 */
static int ler_inteiro(const char *prompt, int *saida) {
    char buf[TAM_ENTRADA];
    char *fim;
    long val;

    printf("%s", prompt);
    if (!fgets(buf, sizeof(buf), stdin)) {
        return 0;
    }
    limpar_quebra(buf);
    if (buf[0] == '\0') {
        return -1;
    }
    val = strtol(buf, &fim, 10);
    if (*fim != '\0') {
        return -1;
    }
    *saida = (int)val;
    return 1;
}

/*
 * Le um float da entrada padrao. Retorna 1 em sucesso, 0 em EOF, -1 se invalido.
 */
static int ler_float(const char *prompt, float *saida) {
    char buf[TAM_ENTRADA];
    char *fim;
    double val;

    printf("%s", prompt);
    if (!fgets(buf, sizeof(buf), stdin)) {
        return 0;
    }
    limpar_quebra(buf);
    if (buf[0] == '\0') {
        return -1;
    }
    val = strtod(buf, &fim);
    if (*fim != '\0' || val != val) {
        return -1;
    }
    *saida = (float)val;
    return 1;
}

static void executar_eixo_df(const char *csv_df) {
    Grafo *g_bip;
    Grafo *g_sim;
    float limiar;
    int status;

    printf("\n=== EIXO DF ===\n");
    g_bip = construir_grafo_bipartido_df(csv_df);
    if (!g_bip) {
        fprintf(stderr, "Falha ao carregar o grafo bipartido do DF.\n");
        return;
    }

    printf("\n--- Candidatos dominantes por RA ---\n");
    candidatos_dominantes_por_ra(g_bip);

    limiar = 0.95f;
    status = ler_float("\nInforme o limiar de similaridade de cosseno (0.0 a 1.0): ",
                       &limiar);
    if (status == -1) {
        printf("Entrada invalida; usando limiar padrao 0.95.\n");
        limiar = 0.95f;
    }

    g_sim = gerar_grafo_similaridade_ra(g_bip, limiar);
    if (g_sim) {
        printf("\n--- Grafo de similaridade entre RAs (limiar >= %.2f) ---\n",
               limiar);
        imprimir_grafo(g_sim);
        liberar_grafo(g_sim);
    } else {
        fprintf(stderr, "Falha ao gerar grafo de similaridade entre RAs.\n");
    }

    liberar_grafo(g_bip);
}

static void executar_eixo_nacional(const char *csv_fronteiras,
                                   const char *csv_presidente) {
    Grafo *g_terr;
    Grafo *g_pres;
    char uf[TAM_ENTRADA];
    int origem;
    float limiar;
    int status;

    printf("\n=== EIXO NACIONAL ===\n");
    g_terr = carregar_g_terr(csv_fronteiras);
    g_pres = carregar_g_pres(csv_presidente);
    if (!g_terr || !g_pres) {
        fprintf(stderr, "Falha ao carregar os grafos nacionais.\n");
        liberar_grafo(g_terr);
        liberar_grafo(g_pres);
        return;
    }

    origem = -1;
    printf("\nInforme a sigla da UF de origem para a BFS (ex.: DF, SP, AC): ");
    if (fgets(uf, sizeof(uf), stdin)) {
        limpar_quebra(uf);
        origem = buscar_vertice(g_terr, uf);
        if (origem == -1) {
            printf("UF '%s' nao encontrada em G_Terr; chamando BFS com -1.\n", uf);
        }
    }
    busca_largura_propagacao_fronteiras(g_terr, origem);

    limiar = 15.0f;
    status = ler_float("\nInforme o limiar de distancia euclidiana para clusters (DFS): ",
                       &limiar);
    if (status == -1) {
        printf("Entrada invalida; usando limiar padrao 15.0.\n");
        limiar = 15.0f;
    }
    printf("\n");
    busca_profundidade_clusters(g_pres, limiar);

    printf("\n");
    calcular_mst_eleitoral_kruskal(g_pres);

    liberar_grafo(g_terr);
    liberar_grafo(g_pres);
}

static void imprimir_uso(const char *prog) {
    fprintf(stderr,
            "Uso: %s <csv_governador_df> <csv_fronteiras> <csv_presidente>\n"
            "Exemplo:\n"
            "  %s dados/votacao_governador_df_2026.csv "
            "dados/fronteiras_estados_2026.csv "
            "dados/votacao_presidente_estados_2026.csv\n",
            prog, prog);
}

int main(int argc, char *argv[]) {
    int opcao;
    int status;

    if (argc != 4) {
        imprimir_uso(argv[0]);
        return 1;
    }

    while (1) {
        printf("\n=== Analise Eleitoral com Grafos (2026) ===\n");
        printf("1 - Eixo DF (dominantes por RA e similaridade)\n");
        printf("2 - Eixo Nacional (BFS fronteiras, DFS clusters e Kruskal)\n");
        printf("3 - Executar ambos os eixos\n");
        printf("0 - Sair\n");

        status = ler_inteiro("Escolha uma opcao: ", &opcao);
        if (status == 0) {
            break;
        }
        if (status == -1) {
            printf("Opcao invalida.\n");
            continue;
        }

        if (opcao == 0) {
            break;
        } else if (opcao == 1) {
            executar_eixo_df(argv[1]);
        } else if (opcao == 2) {
            executar_eixo_nacional(argv[2], argv[3]);
        } else if (opcao == 3) {
            executar_eixo_df(argv[1]);
            executar_eixo_nacional(argv[2], argv[3]);
        } else {
            printf("Opcao invalida.\n");
        }
    }

    printf("Encerrando.\n");
    return 0;
}
