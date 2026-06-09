#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "../includes/estruturas.h"

// Função para alocar dinamicamente o gerenciador do tabuleiro e a matriz
Tabuleiro* criar_tabuleiro(int l, int c) {
    // 1. Aloca a estrutura principal do tabuleiro
    Tabuleiro* tab = (Tabuleiro*)malloc(sizeof(Tabuleiro));
    if (tab == NULL) {
        return NULL; // Proteção caso falte memória no sistema
    }
    
    tab->linhas = l;
    tab->colunas = c;
    tab->lista_entidades = NULL; // Inicializa a lista encadeada vazia

    // 2. Aloca o vetor de ponteiros para as linhas da matriz
    tab->grade = (TipoEntidade**)malloc(l * sizeof(TipoEntidade*));
    if (tab->grade == NULL) {
        free(tab);
        return NULL;
    }

    // 3. Aloca cada coluna de cada linha. 
    // Usamos calloc para garantir que todas as posições comecem zeradas (VAZIO)
    for (int i = 0; i < l; i++) {
        tab->grade[i] = (TipoEntidade*)calloc(c, sizeof(TipoEntidade));
        
        // Se falhar no meio do caminho, limpa o que já foi alocado antes de sair
        if (tab->grade[i] == NULL) {
            for (int j = 0; j < i; j++) {
                free(tab->grade[j]);
            }
            free(tab->grade);
            free(tab);
            return NULL;
        }
    }

    return tab;
}

// Função para limpar absolutamente toda a memória antes de fechar o jogo
void destruir_tabuleiro(Tabuleiro* tab) {
    if (tab == NULL) return;

    // 1. Libera cada linha da matriz dinâmica
    for (int i = 0; i < tab->linhas; i++) {
        free(tab->grade[i]);
    }
    // 2. Libera o vetor de ponteiros da matriz
    free(tab->grade);

    // 3. Libera todas as entidades que restaram na lista encadeada
    Entidade* atual = tab->lista_entidades;
    while (atual != NULL) {
        Entidade* proximo = atual->prox;
        free(atual); // Libera o nó atual
        atual = proximo; // Avança para o próximo
    }

    // 4. Por fim, libera a própria estrutura do tabuleiro
    free(tab);
}

// Função auxiliar para inserir uma nova entidade no início da lista encadeada (Eficiência O(1))
void inserir_na_lista(Tabuleiro* tab, int id, TipoEntidade tipo, int x, int y) {
    Entidade* nova = (Entidade*)malloc(sizeof(Entidade));
    if (nova == NULL) return;

    nova->id = id;
    nova->tipo = tipo;
    nova->x = x;
    nova->y = y;
    nova->ativo = 1; // Nasce vivo
    nova->prox = tab->lista_entidades; // Aponta para a antiga cabeça da lista
    tab->lista_entidades = nova;       // A nova entidade passa a ser a cabeça
}

// Função principal para calcular proporções e espalhar os elementos no mapa
void inicializar_elementos(Tabuleiro* tab) {
    int total_casas = tab->linhas * tab->colunas;
    
    // Cálculo das quantidades com base nos percentuais (arredondamento inteiro simples)
    int qtd_bamor = (total_casas * 12) / 100;
    int qtd_tui   = (total_casas * 12) / 100;
    int qtd_pm    = (total_casas * 5) / 100;
    int qtd_obst  = (total_casas * 10) / 100;

    // Garante pelo menos 1 de cada se o mapa for muito pequeno
    if (qtd_bamor == 0) qtd_bamor = 1;
    if (qtd_tui == 0) qtd_tui = 1;
    if (qtd_pm == 0) qtd_pm = 1;
    if (qtd_obst == 0) qtd_obst = 1;

    int id_contador = 1;
    int x, y;

    // Função interna para sortear uma vaga vazia na matriz
    #define SORTEAR_POSICAO_VAZIA() \
        do { \
            x = rand() % tab->linhas; \
            y = rand() % tab->colunas; \
        } while (tab->grade[x][y] != VAZIO)

    // 1. Posicionar Obstáculos (Apenas na Matriz, pois não se movem)
    for (int i = 0; i < qtd_obst; i++) {
        SORTEAR_POSICAO_VAZIA();
        // Alterna entre Muro e Rio para ter dois tipos como pede o roteiro
        tab->grade[x][y] = (i % 2 == 0) ? OBSTACULO_MURO : OBSTACULO_RIO;
    }

    // 2. Posicionar Facção 1: Bamor
    for (int i = 0; i < qtd_bamor; i++) {
        SORTEAR_POSICAO_VAZIA();
        tab->grade[x][y] = BAMOR;
        inserir_na_lista(tab, id_contador++, BAMOR, x, y);
    }

    // 3. Posicionar Facção 2: TUI
    for (int i = 0; i < qtd_tui; i++) {
        SORTEAR_POSICAO_VAZIA();
        tab->grade[x][y] = TUI;
        inserir_na_lista(tab, id_contador++, TUI, x, y);
    }

    // 4. Posicionar Facção 3: PM
    for (int i = 0; i < qtd_pm; i++) {
        SORTEAR_POSICAO_VAZIA();
        tab->grade[x][y] = PM;
        inserir_na_lista(tab, id_contador++, PM, x, y);
    }
    
    #undef SORTEAR_POSICAO_VAZIA
}

void exibir_tabuleiro(Tabuleiro* tab) {
    // Desenha o topo da borda do estádio/mapa
    for (int j = 0; j < tab->colunas + 2; j++) printf("==");
    printf("\n");

    for (int i = 0; i < tab->linhas; i++) {
        printf("||"); // Borda esquerda
        for (int j = 0; j < tab->colunas; j++) {
            switch (tab->grade[i][j]) {
                case VAZIO:          printf("  "); break;
                case BAMOR:          printf("B "); break; // B de Bamor
                case TUI:            printf("T "); break; // T de TUI
                case PM:             printf("P "); break; // P de PM
                case OBSTACULO_MURO: printf("X "); break; // X de Muro
                case OBSTACULO_RIO:  printf("~ "); break; // ~ de Rio
            }
        }
        printf("||\n"); // Borda direita
    }

    for (int j = 0; j < tab->colunas + 2; j++) printf("==");
    printf("\n");
}