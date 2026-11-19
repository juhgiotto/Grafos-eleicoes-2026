#include <assert.h>
#include <math.h>
#include <stdio.h>

#include "../include/analise_df.h"
#include "../include/grafo.h"

#define CSV_SINTETICO "tests/dados_sinteticos/votacao_df_sintetico.csv"
#define CSV_INVALIDO "tests/dados_sinteticos/votacao_df_invalido.csv"
#define CSV_CABECALHO_RUIM "tests/dados_sinteticos/votacao_df_cabecalho_invalido.csv"

/* Retorna 1 se existe aresta de `a` para `b` com peso ~ `peso`. */
static int tem_aresta(const Grafo *g, const char *a, const char *b, float peso) {
    int ia = buscar_vertice(g, a);
    int ib = buscar_vertice(g, b);
    const No *n;

    if (ia < 0 || ib < 0) {
        return 0;
    }
    for (n = g->adj[ia].cabeca; n != NULL; n = n->prox) {
        if (n->destino == ib) {
            return fabs((double)(n->peso - peso)) < 1e-4;
        }
    }
    return 0;
}

static void test_arquivo_inexistente(void) {
    assert(construir_grafo_bipartido_df("tests/nao_existe.csv") == NULL);
    assert(construir_grafo_bipartido_df(NULL) == NULL);
}

static void test_cabecalho_invalido(void) {
    assert(construir_grafo_bipartido_df(CSV_CABECALHO_RUIM) == NULL);
}

static void test_dataset_sintetico(void) {
    static const char *ras[5] = {"Aguas Claras", "Asa Norte", "Ceilandia", "Cruzeiro", "Gama"};
    static const char *cands[3] = {"CELINA LEAO HIZIM FERREIRA", "LEANDRO ANTÔNIO GRASS PEIXOTO",
                                   "PAULA MORENO PARO BELMONTE"};
    /* Tabela esperada (percentuais por RA x candidato). */
    static const float esperado[5][3] = {
        {48.09f, 35.45f, 7.81f}, {36.68f, 46.62f, 6.47f}, {53.35f, 31.70f, 8.92f},
        {43.77f, 38.77f, 7.06f}, {49.46f, 34.67f, 9.48f}
    };
    Grafo *g;
    int total_arestas_ras = 0;
    int i, j, k, ra, c;

    g = construir_grafo_bipartido_df(CSV_SINTETICO);
    assert(g != NULL);
    assert(g->orientado == 0);
    assert(g->num_vertices == 8); /* 5 RAs + 3 candidatos */

    for (i = 0; i < 5; i++) {
        ra = buscar_vertice(g, ras[i]);
        assert(ra >= 0);
        assert(g->adj[ra].num_arestas == 3);
        total_arestas_ras += g->adj[ra].num_arestas;
        for (j = 0; j < 3; j++) {
            assert(tem_aresta(g, ras[i], cands[j], esperado[i][j]));
            assert(tem_aresta(g, cands[j], ras[i], esperado[i][j])); /* simétrica */
        }
    }
    assert(total_arestas_ras == 15);

    /* Bipartição: nenhum candidato ligado a candidato, nenhuma RA ligada a RA. */
    for (j = 0; j < 3; j++) {
        c = buscar_vertice(g, cands[j]);
        assert(c >= 0 && g->adj[c].num_arestas == 5);
        for (k = 0; k < 3; k++) {
            assert(!tem_aresta(g, cands[j], cands[k], 0.0f) || j == k);
        }
    }

    printf("--- Grafo bipartido sintetico (5 RAs x 3 candidatos, 15 arestas) ---\n");
    imprimir_grafo(g);
    liberar_grafo(g);
}

static void test_linhas_malformadas_ignoradas(void) {
    Grafo *g = construir_grafo_bipartido_df(CSV_INVALIDO);
    assert(g != NULL);

    /* Só "Valida Um" e "Valida Dois" sobrevivem: 2 RAs + 2 candidatos. */
    assert(g->num_vertices == 4);
    assert(buscar_vertice(g, "Valida Um") >= 0);
    assert(buscar_vertice(g, "Valida Dois") >= 0);
    assert(buscar_vertice(g, "Falta Campo") == -1);
    assert(buscar_vertice(g, "Texto") == -1);
    assert(buscar_vertice(g, "Acima de 100") == -1);
    assert(buscar_vertice(g, "Negativo") == -1);
    assert(buscar_vertice(g, "Campo Vazio") == -1);
    assert(buscar_vertice(g, "Campos Demais") == -1);

    assert(tem_aresta(g, "Valida Um", "CAND A", 10.5f));
    assert(tem_aresta(g, "Valida Um", "CAND B", 20.25f));
    assert(tem_aresta(g, "Valida Dois", "CAND A", 30.0f));
    assert(tem_aresta(g, "Valida Dois", "CAND B", 40.0f));

    liberar_grafo(g);
}

static void test_candidatos_dominantes_sintetico(void) {
    Grafo *g;
    Grafo *vazio;
    int pp;
    int ac;
    int tag;
    int cei;
    int sob;
    int cand_a;
    int cand_b;

    /* Casos degenerados: NULL e grafo vazio nao devem falhar. */
    candidatos_dominantes_por_ra(NULL);
    vazio = criar_grafo(4, 0);
    assert(vazio != NULL);
    candidatos_dominantes_por_ra(vazio);
    liberar_grafo(vazio);

    /* Dataset sintetico com 5 RAs e empate 40% x 40% em Sobradinho. */
    g = criar_grafo(8, 0);
    assert(g != NULL);

    pp = adicionar_vertice(g, "Plano Piloto");
    cand_a = adicionar_vertice(g, "A");
    cand_b = adicionar_vertice(g, "B");
    ac = adicionar_vertice(g, "Aguas Claras");
    tag = adicionar_vertice(g, "Taguatinga");
    cei = adicionar_vertice(g, "Ceilandia");
    sob = adicionar_vertice(g, "Sobradinho");

    assert(adicionar_aresta(g, pp, cand_a, 50.0f) == 0);
    assert(adicionar_aresta(g, pp, cand_b, 30.0f) == 0);
    assert(adicionar_aresta(g, ac, cand_a, 60.0f) == 0);
    assert(adicionar_aresta(g, ac, cand_b, 25.0f) == 0);
    assert(adicionar_aresta(g, tag, cand_a, 30.0f) == 0);
    assert(adicionar_aresta(g, tag, cand_b, 55.0f) == 0);
    assert(adicionar_aresta(g, cei, cand_a, 20.0f) == 0);
    assert(adicionar_aresta(g, cei, cand_b, 65.0f) == 0);
    assert(adicionar_aresta(g, sob, cand_a, 40.0f) == 0);
    assert(adicionar_aresta(g, sob, cand_b, 40.0f) == 0);

    printf("--- Candidatos dominantes por RA (sintetico 5 RAs) ---\n");
    candidatos_dominantes_por_ra(g);
    liberar_grafo(g);
}

static void test_similaridade_cosseno(void) {
    static const char *cands_nomes[3] = {
        "CELINA LEAO HIZIM FERREIRA",
        "LEANDRO ANTÔNIO GRASS PEIXOTO",
        "PAULA MORENO PARO BELMONTE"
    };
    float zero[3] = {0.0f, 0.0f, 0.0f};
    float u[3] = {1.0f, 0.0f, 0.0f};
    float v[3] = {0.0f, 1.0f, 0.0f};
    float vet_ac[3];
    float vet_an[3];
    float vet_cei[3];
    float vet_gama[3];
    int cands_idx[3];
    Grafo *g;
    int i;
    float sim_ac_gama;
    float sim_ac_an;
    float sim_an_cei;

    /* Casos extremos: NULL, n <= 0, vetor nulo, identicos e ortogonais. */
    assert(fabs((double)similaridade_cosseno(NULL, u, 3)) < 1e-6);
    assert(fabs((double)similaridade_cosseno(u, NULL, 3)) < 1e-6);
    assert(fabs((double)similaridade_cosseno(u, u, 0)) < 1e-6);
    assert(fabs((double)similaridade_cosseno(zero, u, 3)) < 1e-6);
    assert(fabs((double)similaridade_cosseno(u, zero, 3)) < 1e-6);
    assert(fabs((double)(similaridade_cosseno(u, u, 3) - 1.0f)) < 1e-6);
    assert(fabs((double)similaridade_cosseno(u, v, 3)) < 1e-6);

    /* Conferencia com calculo manual no dataset sintetico (tolerancia 0.001). */
    g = construir_grafo_bipartido_df(CSV_SINTETICO);
    assert(g != NULL);

    for (i = 0; i < 3; i++) {
        cands_idx[i] = buscar_vertice(g, cands_nomes[i]);
        assert(cands_idx[i] >= 0);
    }

    assert(extrair_vetor_ra(g, buscar_vertice(g, "Aguas Claras"), cands_idx, 3, vet_ac) == 0);
    assert(extrair_vetor_ra(g, buscar_vertice(g, "Asa Norte"), cands_idx, 3, vet_an) == 0);
    assert(extrair_vetor_ra(g, buscar_vertice(g, "Ceilandia"), cands_idx, 3, vet_cei) == 0);
    assert(extrair_vetor_ra(g, buscar_vertice(g, "Gama"), cands_idx, 3, vet_gama) == 0);

    sim_ac_gama = similaridade_cosseno(vet_ac, vet_gama, 3);
    sim_ac_an = similaridade_cosseno(vet_ac, vet_an, 3);
    sim_an_cei = similaridade_cosseno(vet_an, vet_cei, 3);

    /*
     * Valores calculados a mao (tolerancia 0.001):
     * - Aguas Claras x Gama:   3681.6217 / (60.2524 * 61.1406) = 0.999391
     * - Aguas Claras x Asa N.: 3467.1509 / (60.2524 * 59.6717) = 0.964341
     * - Asa Norte x Ceilandia: 3492.4444 / (59.6717 * 62.6951) = 0.933528
     */
    assert(fabs((double)(sim_ac_gama - 0.999391f)) < 0.001);
    assert(fabs((double)(sim_ac_an - 0.964341f)) < 0.001);
    assert(fabs((double)(sim_an_cei - 0.933528f)) < 0.001);

    liberar_grafo(g);
}

int main(void) {
    test_arquivo_inexistente();
    test_cabecalho_invalido();
    test_dataset_sintetico();
    test_linhas_malformadas_ignoradas();
    test_candidatos_dominantes_sintetico();
    test_similaridade_cosseno();

    printf("Todos os testes de analise_df passaram!\n");
    return 0;
}
