/*
 * Ponteiros + Alocação Dinâmica (vetor e matriz) com raylib
 * ---------------------------------------------------------------
 * Este programa desenha:
 *   1) Uma matriz dinâmica (grade de células coloridas) alocada com malloc,
 *      onde cada linha é um ponteiro para um vetor de inteiros.
 *   2) Um vetor dinâmico de bolinhas (struct Bola) que se movem na tela,
 *      manipuladas via ponteiros.
 *
 * Conceitos praticados:
 *   - malloc / free
 *   - ponteiro para ponteiro (int **) para representar matriz
 *   - vetor de structs alocado dinamicamente
 *   - passagem de ponteiros para funções (evita cópias, permite alterar
 *     o dado original)
 *   - aritmética de ponteiros ( *(p + i) é equivalente a p[i] )
 *
 * Compilar no MSYS2 UCRT64:
 *   gcc main.c -o jogo.exe $(pkg-config --cflags --libs raylib)
 */
#include <stdio.h>
#include <raylib.h>
#include <stdlib.h>
#include <time.h>

#define LARGURA_JANELA 800
#define ALTURA_JANELA  600
#define TAM_CELULA     40

typedef struct {
    Vector2 pos;
    Vector2 vel;
    float   raio;
    Color   cor;
} Bola;

/* ---------------------------------------------------------------
 * cria uma MATRIZ dinâmica de inteiros (linhas x colunas)
 * Retorna um ponteiro para ponteiro (int **): cada posição do
 * vetor externo aponta para um vetor de inteiros (uma linha).
 * --------------------------------------------------------------- */
int **criarMatriz(int linhas, int colunas) {
    int **matriz = (int **)malloc(linhas * sizeof(int *));
    if (matriz == NULL) return NULL;

    for (int i = 0; i < linhas; i++) {
        matriz[i] = (int *)malloc(colunas * sizeof(int));
        for (int j = 0; j < colunas; j++) {
            matriz[i][j] = 0;  // começa tudo não visitado 
        }
    }
    return matriz;
}

/* libera a memória da matriz: primeiro cada linha, depois o vetor de linhas */
void liberarMatriz(int **matriz, int linhas) {
    for (int i = 0; i < linhas; i++) {
        free(matriz[i]);   // libera cada linha
    }
    free(matriz);           // libera o vetor de ponteiros
}

/* desenha a matriz na tela, célula por célula */
void desenharMatriz(int **matriz, int linhas, int colunas) {
    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            Color cor;
            if (matriz[i][j] == 1) {
                cor = (Color){80, 120, 200, 255};  // visitada 
            } else {
                cor = (Color){15, 30, 55, 255};    // não visitada 
            }
            DrawRectangle(j * TAM_CELULA, i * TAM_CELULA,
                           TAM_CELULA - 2, TAM_CELULA - 2, cor);
        }
    }
}

/* Marca uma célula como visitada se a bola passar por cima */
void marcarCelulaVisitada(int **matriz, int linhas, int colunas, float x, float y, int *contador) {
    int coluna = (int)(x / TAM_CELULA);
    int linha = (int)(y / TAM_CELULA);
    
    // verifica se a célula está dentro da matriz
    if (linha >= 0 && linha < linhas && coluna >= 0 && coluna < colunas) {
        // Se a célula ainda não foi visitada (valor 0)
        if (matriz[linha][coluna] == 0) {
            matriz[linha][coluna] = 1;  // marca como visitada
            (*contador)++;              // incrementa o contador
        }
    }
}

/* ---------------------------------------------------------------
 * cria o vetor dinâmico de bolas
 * --------------------------------------------------------------- */
Bola *criarBolas(int quantidade) {
    Bola *bolas = (Bola *)malloc(quantidade * sizeof(Bola));
    if (bolas == NULL) return NULL;

    for (int i = 0; i < quantidade; i++) {
        // usar (bolas + i) é o mesmo que &bolas[i]
        Bola *b = (bolas + i);
        b->pos = (Vector2){ GetRandomValue(50, LARGURA_JANELA - 50),
                             GetRandomValue(50, ALTURA_JANELA - 50) };
        b->vel = (Vector2){ (float)GetRandomValue(-4, 4),
                             (float)GetRandomValue(-4, 4) };
        b->raio = (float)GetRandomValue(10, 25);
        b->cor  = (Color){ GetRandomValue(100,255), GetRandomValue(100,255),
                            GetRandomValue(100,255), 255 };
    }
    return bolas;
}

/* atualiza a posição de UMA bola: recebe um PONTEIRO para a struct */
void atualizarBola(Bola *b) {
    b->pos.x += b->vel.x;
    b->pos.y += b->vel.y;

    // rebate nas bordas
    if (b->pos.x - b->raio < 0 || b->pos.x + b->raio > LARGURA_JANELA)
        b->vel.x *= -1;
    if (b->pos.y - b->raio < 0 || b->pos.y + b->raio > ALTURA_JANELA)
        b->vel.y *= -1;
}

int main(void) {
    srand((unsigned int)time(NULL));

    InitWindow(LARGURA_JANELA, ALTURA_JANELA,
               "Ponteiros e Alocacao Dinamica - raylib");
    SetTargetFPS(60);

    int linhas   = ALTURA_JANELA / TAM_CELULA;
    int colunas  = LARGURA_JANELA / TAM_CELULA;
    int **grade  = criarMatriz(linhas, colunas);
    int celulasVisitadas = 0;

    int quantidadeBolas = 12;
    Bola *bolas = criarBolas(quantidadeBolas);

    while (!WindowShouldClose()) {

        // percorre o vetor usando aritmética de ponteiros
        for (int i = 0; i < quantidadeBolas; i++) {
            atualizarBola(bolas + i);
        }

        // Marcar células visitadas pelas bolas
        for (int i = 0; i < quantidadeBolas; i++) {
            marcarCelulaVisitada(grade, linhas, colunas, 
                                 bolas[i].pos.x, bolas[i].pos.y, 
                                 &celulasVisitadas);
        }

        // Verificar teclas para adicionar/remover bolas
        if (IsKeyPressed(KEY_SPACE)) {
            quantidadeBolas++;
            bolas = (Bola *)realloc(bolas, quantidadeBolas * sizeof(Bola));
            if (bolas != NULL) {
                Bola *nova = &bolas[quantidadeBolas - 1];
                nova->pos = (Vector2){ GetRandomValue(50, LARGURA_JANELA - 50),
                                        GetRandomValue(50, ALTURA_JANELA - 50) };
                nova->vel = (Vector2){ (float)GetRandomValue(-4, 4),
                                        (float)GetRandomValue(-4, 4) };
                nova->raio = (float)GetRandomValue(10, 25);
                nova->cor  = (Color){ GetRandomValue(100,255), GetRandomValue(100,255),
                                       GetRandomValue(100,255), 255 };
            }
        }

        if (IsKeyPressed(KEY_BACKSPACE) && quantidadeBolas > 0) {
            quantidadeBolas--;
            bolas = (Bola *)realloc(bolas, quantidadeBolas * sizeof(Bola));
        }

        BeginDrawing();
            ClearBackground(RAYWHITE);

            desenharMatriz(grade, linhas, colunas);

            for (int i = 0; i < quantidadeBolas; i++) {
                DrawCircleV(bolas[i].pos, bolas[i].raio, bolas[i].cor);
            }

            DrawText("Matriz (int**) e vetor de structs (Bola*) alocados com malloc",
                     10, 10, 18, WHITE);
            DrawText("Pressione ESC para sair", 10, ALTURA_JANELA - 25, 16, WHITE);

            char textoBolas[50];
            sprintf(textoBolas, "Bolas: %d | ESPACO adiciona | BACKSPACE remove", quantidadeBolas);
            DrawText(textoBolas, 10, ALTURA_JANELA - 50, 16, WHITE);

            char textoVisitadas[50];
            sprintf(textoVisitadas, "Celulas visitadas: %d / %d", celulasVisitadas, linhas * colunas);
            DrawText(textoVisitadas, 10, ALTURA_JANELA - 75, 16, WHITE);

        EndDrawing();
    }

    // libera TODA a memória alocada dinamicamente antes de encerrar
    free(bolas);
    liberarMatriz(grade, linhas);

    CloseWindow();
    return 0;
}