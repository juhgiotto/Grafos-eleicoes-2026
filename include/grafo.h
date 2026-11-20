#ifndef GRAFO_H
#define GRAFO_H

/*
 * =========================================================================
 * Modulo: Base de Grafos (include/grafo.h)
 * Papel : Interface publica da estrutura de dados central do projeto.
 * Teoria: Grafo ponderado em Listas de Adjacencia Dinamicas (orientado ou
 *         nao-orientado), com rotulos em string e busca em largura (BFS).
 * =========================================================================
 */

typedef struct No {
    int destino;      /* Indice do vertice destino */
    float peso;       /* Peso da aresta (% de votos, similaridade ou distancia) */
    struct No *prox;  /* Proximo no na lista encadeada */
} No;

typedef struct Lista {
    No *cabeca;       /* Inicio da lista de adjacencia */
    int num_arestas;  /* Grau de saida (ou grau total no grafo nao-orientado) */
} Lista;

typedef struct Grafo {
    Lista *adj;        /* Vetor dinamico de listas de adjacencia */
    char **rotulos;    /* Nomes dos vertices (RAs, candidatos ou UFs) */
    int num_vertices;  /* Quantidade atual de vertices */
    int capacidade;    /* Capacidade alocada (dobra via realloc) */
    int orientado;     /* 0 = nao-orientado, != 0 = orientado */
} Grafo;

Grafo *criar_grafo(int capacidade_inicial, int orientado);
int adicionar_vertice(Grafo *g, const char *rotulo);
int buscar_vertice(const Grafo *g, const char *rotulo);
int adicionar_aresta(Grafo *g, int origem, int destino, float peso);
void imprimir_grafo(const Grafo *g);
void liberar_grafo(Grafo *g);

/* Busca em Largura (BFS) por camadas de fronteira a partir de 'origem'. */
void busca_largura_propagacao_fronteiras(Grafo *g, int origem);

/* Arvore Geradora Minima (AGM) via algoritmo de Kruskal + Union-Find. */
void calcular_mst_eleitoral_kruskal(Grafo *g);

#endif /* GRAFO_H */
