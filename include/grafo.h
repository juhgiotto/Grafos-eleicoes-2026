#ifndef GRAFO_H
#define GRAFO_H

/* Nó da lista de adjacência representando uma aresta direcionada. */
typedef struct No {
    int destino;      /* Índice do vértice de destino */
    float peso;       /* Peso associado à aresta */
    struct No *prox;  /* Ponteiro para o próximo nó da lista */
} No;

/* Lista de adjacência associada a um vértice. */
typedef struct Lista {
    No *cabeca;       /* Ponteiro para o primeiro nó da lista */
    int num_arestas;  /* Quantidade de arestas partindo deste vértice */
} Lista;

/* Estrutura principal do grafo baseado em listas de adjacência. */
typedef struct Grafo {
    Lista *adj;        /* Vetor de listas de adjacência */
    char **rotulos;    /* Vetor de rótulos (strings) dos vértices */
    int num_vertices;  /* Quantidade atual de vértices no grafo */
    int capacidade;    /* Capacidade alocada para vértices */
    int orientado;     /* 0 = não-orientado, diferente de 0 = orientado */
} Grafo;

/*
 * Cria e inicializa um novo grafo vazio.
 * Parâmetros:
 *   capacidade_inicial: número inicial de vértices suportados (deve ser > 0).
 *   orientado: 0 para grafo não-orientado; diferente de 0 para orientado.
 * Retorno:
 *   Ponteiro para o Grafo alocado, ou NULL em caso de capacidade inválida
 *   ou falha de alocação de memória.
 */
Grafo *criar_grafo(int capacidade_inicial, int orientado);

/*
 * Adiciona um novo vértice com o rótulo informado (redimensiona se necessário).
 * Parâmetros:
 *   g: ponteiro para o grafo.
 *   rotulo: string não-nula identificando o vértice.
 * Retorno:
 *   Índice (>= 0) do vértice inserido em caso de sucesso, ou -1 em caso de erro
 *   (parâmetros inválidos, rótulo já existente ou falha de alocação).
 */
int adicionar_vertice(Grafo *g, const char *rotulo);

/*
 * Busca o índice de um vértice a partir do seu rótulo.
 * Parâmetros:
 *   g: ponteiro para o grafo.
 *   rotulo: string com o rótulo a ser procurado.
 * Retorno:
 *   Índice (>= 0) do vértice encontrado, ou -1 se não existir ou se os
 *   parâmetros forem inválidos (NULL).
 */
int buscar_vertice(const Grafo *g, const char *rotulo);

/*
 * Adiciona uma aresta entre os vértices de origem e destino com o peso dado.
 * Em grafos não-orientados, adiciona também a aresta simétrica.
 * Parâmetros:
 *   g: ponteiro para o grafo.
 *   origem: índice do vértice de origem.
 *   destino: índice do vértice de destino.
 *   peso: peso da aresta.
 * Retorno:
 *   0 em caso de sucesso, ou -1 em caso de erro (ponteiro nulo, índices fora
 *   dos limites [0, num_vertices - 1] ou falha de alocação).
 */
int adicionar_aresta(Grafo *g, int origem, int destino, float peso);

/*
 * Imprime os vértices (com rótulos) e suas listas de adjacência na saída padrão.
 * Parâmetros:
 *   g: ponteiro para o grafo (se NULL, não realiza operação).
 * Retorno:
 *   Nenhum.
 */
void imprimir_grafo(const Grafo *g);

/*
 * Libera toda a memória alocada pelo grafo (nós, listas, rótulos e struct).
 * Parâmetros:
 *   g: ponteiro para o grafo (aceita NULL com segurança).
 * Retorno:
 *   Nenhum.
 */
void liberar_grafo(Grafo *g);

/* BFS por camadas a partir de 'origem' (issue #9). */
void busca_largura_propagacao_fronteiras(Grafo *g, int origem);

#endif /* GRAFO_H */
