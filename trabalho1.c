/*
    PROGRAMAÇÃO CONCORRENTE - TRABALHO 1

    Desenvolvido por:
    Lucas Wolschick             RA123658
    Rômulo Barreto Mincache     RA117477
*/

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

#define TAM_MAX 1000
typedef int matriz[TAM_MAX][TAM_MAX];
typedef struct parametros
{
    int linha;
    int contador;
    pthread_mutex_t mutex_contador;
    pthread_mutex_t mutex_cond;
    pthread_cond_t cond;
} parametros;

void gerar_matriz(matriz mat, int n_linhas, int n_colunas);
void mostrar_matriz(matriz mat, int n_linhas, int n_colunas);
int media_vizinhos(matriz mat, int n_linhas, int n_colunas, int i, int j);
void calcula_matriz(matriz mat, int n_linhas, int n_colunas);
void *calcular(void *args);

matriz m1;
int n_linhas, n_colunas;
pthread_t threads[TAM_MAX];
parametros lista_parametros[TAM_MAX];

int main()
{

    printf("Número de linhas: ");
    scanf("%d", &n_linhas);

    printf("Número de colunas: ");
    scanf("%d", &n_colunas);

    gerar_matriz(m1, n_linhas, n_colunas);
    mostrar_matriz(m1, n_linhas, n_colunas);

    for (int i = 0; i < n_linhas; i++)
    {
        lista_parametros[i].contador = 0;
        lista_parametros[i].linha = i;
        pthread_cond_init(&lista_parametros[i].cond, NULL);
        pthread_mutex_init(&lista_parametros[i].mutex_contador, NULL);
        pthread_mutex_init(&lista_parametros[i].mutex_cond, NULL);
        pthread_create(&threads[i], NULL, calcular, &lista_parametros[i]);
    }

    for (int i = 0; i < n_linhas; i++)
    {
        pthread_join(threads[i], NULL);
    }

    // calcula_matriz(m1, n_linhas, n_colunas);
    mostrar_matriz(m1, n_linhas, n_colunas);
}

int media_vizinhos(matriz mat, int n_linhas, int n_colunas, int i, int j)
{
    if ((j > 0) && (j < n_colunas - 1))
    {
        // miolo da matriz
        if ((i > 0) && (i < n_linhas - 1))
        {
            // printf("\nmiolo da matriz %d, i=%d, j=%d\n", mat[i][j], i, j);
            return (mat[i - 1][j - 1] + mat[i - 1][j] + mat[i - 1][j + 1] + mat[i][j - 1] + mat[i][j + 1] + mat[i + 1][j - 1] + mat[i + 1][j] + mat[i + 1][j + 1]) / 8;
        }
        // miolo da primeira linha
        else if (i == 0)
        {
            // printf("\nmiolo da primeira linha %d, i=%d, j=%d\n", mat[i][j], i, j);
            return (mat[i][j - 1] + mat[i][j + 1] + mat[i + 1][j - 1] + mat[i + 1][j] + mat[i + 1][j + 1]) / 5;
        }
        // miolo da última linha
        else
        {
            // printf("\nmiolo da última linha %d, i=%d, j=%d\n", mat[i][j], i, j);
            return (mat[i - 1][j - 1] + mat[i - 1][j] + mat[i - 1][j + 1] + mat[i][j - 1] + mat[i][j + 1]) / 5;
        }
    }
    else if (j == 0)
    {
        // miolo da primeira coluna
        if ((i > 0) && (i < n_linhas - 1))
        {
            // printf("\nmiolo da primeira coluna %d, i=%d, j=%d\n", mat[i][j], i, j);
            return (mat[i - 1][j] + mat[i - 1][j + 1] + mat[i][j + 1] + mat[i + 1][j] + mat[i + 1][j + 1]) / 5;
        }
        // canto [0][0]
        else if (i == 0)
        {
            // printf("\ncanto [0][0] %d, i=%d, j=%d\n", mat[i][j], i, j);
            return (mat[i][j + 1] + mat[i + 1][j] + mat[i + 1][j + 1]) / 3;
        }
        // canto [n_linhas-1][0]
        else
        {
            // printf("\ncanto [n_linhas-1][0] %d, i=%d, j=%d\n", mat[i][j], i, j);
            return (mat[i - 1][j] + mat[i - 1][j + 1] + mat[i][j + 1]) / 3;
        }
    }

    else
    {
        // miolo da última coluna
        if ((i > 0) && (i < n_linhas - 1))
        {
            // printf("\nmiolo da última coluna %d, i=%d, j=%d\n", mat[i][j], i, j);
            return (mat[i - 1][j - 1] + mat[i - 1][j] + mat[i][j - 1] + mat[i + 1][j - 1] + mat[i + 1][j]) / 5;
        }
        // canto[0][n_colunas-1]
        else if (i == 0)
        {
            // printf("\ncanto[0][n_colunas-1] %d, i=%d, j=%d\n", mat[i][j], i, j);
            return (mat[i][j - 1] + mat[i + 1][j - 1] + mat[i + 1][j]) / 3;
        }
        // canto[n_linhas-1][n_colunas-1]
        else
        {
            // printf("\ncanto[n_linhas-1][n_colunas-1] %d, i=%d, j=%d\n", mat[i][j], i, j);
            return (mat[i - 1][j - 1] + mat[i - 1][j] + mat[i][j - 1]) / 3;
        }
    }
}

void calcula_matriz(matriz mat, int n_linhas, int n_colunas)
{
    for (int i = 0; i < n_linhas; i++)
    {
        for (int j = 0; j < n_colunas; j++)
        {
            mat[i][j] = media_vizinhos(mat, n_linhas, n_colunas, i, j);
        }
    }
}

void *calcular(void *args)
{
    parametros *p = args;
    int i = p->linha;

    // for (int j = 0; j < n_colunas; j++)
    // {
    for (;;)
    {
        pthread_mutex_lock(&lista_parametros[p->linha-1].mutex_cond);
        if (p->linha > 0)
        {
            while (1)
            {
                pthread_mutex_lock(&lista_parametros[p->linha - 1].mutex_contador);
                int contador_cima = lista_parametros[p->linha - 1].contador;
                pthread_mutex_unlock(&lista_parametros[p->linha - 1].mutex_contador);

                if (contador_cima - 1 > p->contador)
                    break;

                else if (contador_cima == n_colunas)
                    break;

                else
                    pthread_cond_wait(&lista_parametros[p->linha - 1].cond, &lista_parametros[p->linha-1].mutex_cond);
            }
        }
        pthread_mutex_unlock(&lista_parametros[p->linha-1].mutex_cond);

        m1[i][p->contador] = media_vizinhos(m1, n_linhas, n_colunas, i, p->contador);
        pthread_mutex_lock(&p->mutex_contador);
        p->contador++;
        pthread_mutex_unlock(&p->mutex_contador);

        pthread_mutex_lock(&p->mutex_cond);
        pthread_cond_signal(&p->cond);
        pthread_mutex_unlock(&p->mutex_cond);

        if (p->contador == n_colunas)
            return NULL;
    }
    // }
}

void gerar_matriz(matriz mat, int n_linhas, int n_colunas)
{

    srand(time(NULL));

    for (int i = 0; i < n_linhas; i++)
    {
        for (int j = 0; j < n_colunas; j++)
        {
            mat[i][j] = rand() % 10;
        }
    }
}

void mostrar_matriz(matriz mat, int n_linhas, int n_colunas)
{

    printf("\nMatriz gerada:\n");
    for (int i = 0; i < n_linhas; i++)
    {
        for (int j = 0; j < n_colunas; j++)
            printf("%d ", mat[i][j]);
        printf("\n");
    }
    printf("\n");
}