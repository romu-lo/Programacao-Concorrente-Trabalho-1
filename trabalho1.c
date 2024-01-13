/*
    PROGRAMAÇÃO CONCORRENTE - TRABALHO 1

    Desenvolvido por:
    Lucas Wolschick             RA123658
    Rômulo Barreto Mincache     RA117477

    Gera uma matriz aleatória com inteiros entre 0 e 9
    e calcula a média de cada entrada da matriz.

    A média é calculada com base nos vizinhos da entrada,
    da esquerda para a direita e de cima para baixo.

    O cálculo da média é realizado com threads. O número
    de threads criadas é igual ao número de linhas fornecido
    pelo usuário.
*/

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

/// O tamanho máximo da matriz.
#define TAM_MAX 1000

/// Uma matriz de inteiros.
typedef int matriz[TAM_MAX][TAM_MAX];

/// Estrutura que contém os parâmetros de uma thread.
typedef struct parametros parametros;
struct parametros
{
    /// A linha a ser computada pela thread.
    int linha;

    /// Contador de progresso da thread.
    ///
    /// O valor indica a próxima coluna a ser processada pela thread,
    /// ou é igual a n_linhas caso a thread já concluiu o processamento.
    ///
    /// `contador` é protegido pelo mutex `mutex_contador`.
    int contador;

    /// Mutex que protege o membro contador.
    pthread_mutex_t mutex_contador;

    /// Variável de condição que indica que progresso foi feito pela thread.
    ///
    /// A thread dona dessa variável a sinaliza toda vez que realiza progresso no
    /// seu processamento. As demais threads que dependem do progresso dessa thread
    /// devem esperar essa condição.
    ///
    /// `cond`é protegido pelo mutex `mutex_cond`.
    pthread_cond_t cond;

    /// Mutex que protege a variável de condição.
    pthread_mutex_t mutex_cond;
};

/// A matriz principal.
matriz mat;

/// Número de linhas e colunas da matriz.
int n_linhas, n_colunas;

/// Vetor das threads criadas.
pthread_t threads[TAM_MAX];

/// Vetor contendo os parâmetros passados às threads.
parametros lista_parametros[TAM_MAX];

/// @brief Gera uma matriz aleatória com `n_linhas` linhas e `n_colunas` colunas.
/// @param mat O destino da matriz a ser gerada.
/// @param n_linhas Número de linhas.
/// @param n_colunas Número de colunas.
void gerar_matriz(matriz mat, int n_linhas, int n_colunas);

/// @brief Imprime a matriz fornecida na tela.
/// @param mat A matriz a ser impressa.
/// @param n_linhas Número de linhas da matriz.
/// @param n_colunas Número de colunas da matriz.
void mostrar_matriz(matriz mat, int n_linhas, int n_colunas);

/// @brief Computa a média dos vizinhos de uma entrada fornecida da matriz.
/// @param mat A matriz fornecida para o cálculo.
/// @param n_linhas Número de linhas da matriz.
/// @param n_colunas Número de colunas da matriz.
/// @param i Linha da entrada a ser computada.
/// @param j Coluna da entrada a ser computada.
/// @return A média dos vizinhos da entrada.
int media_vizinhos(matriz mat, int n_linhas, int n_colunas, int i, int j);

/// @brief Rotina de cálculo de uma linha da matriz resultante por uma thread.
/// @param args Um ponteiro para uma instância do tipo `parametros`.
void *calcular(void *args);

/// @brief Lê um número entre 1 e 1000 da entrada padrão.
/// @return O número lido.
int le_numero(void);

/// @brief Remove caracteres no buffer da entrada padrão.
void flush_in(void);

int main(void)
{
    printf(
        "Trabalho 1 - Programacao Concorrente\n"
        "Lucas Wolschick (RA123658)\n"
        "Romulo Barreto Mincache (RA117477)\n\n");

    printf("Numero de linhas: ");
    n_linhas = le_numero();

    printf("Numero de colunas: ");
    n_colunas = le_numero();

    gerar_matriz(mat, n_linhas, n_colunas);
    mostrar_matriz(mat, n_linhas, n_colunas);

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

    for (int i = 0; i < n_linhas; i++)
    {
        pthread_mutex_destroy(&lista_parametros[i].mutex_cond);
        pthread_mutex_destroy(&lista_parametros[i].mutex_contador);
        pthread_cond_destroy(&lista_parametros[i].cond);
    }

    mostrar_matriz(mat, n_linhas, n_colunas);
}

int le_numero(void)
{
    int num;
    int pronto = 0;

    while (!pronto)
    {
        int lido = scanf(" %d", &num);
        if (lido != 1 || num <= 0 || num > 1000)
        {
            printf("O numero deve ser inteiro entre 1 e 1000: ");
            flush_in();
        }
        else
        {
            pronto = 1;
        }
    }

    return num;
}

void flush_in(void)
{
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF)
    {
    }
}

int media_vizinhos(matriz mat, int n_linhas, int n_colunas, int i, int j)
{
    if ((j > 0) && (j < n_colunas - 1))
    {
        // miolo da matriz
        if ((i > 0) && (i < n_linhas - 1))
        {
            return (mat[i - 1][j - 1] + mat[i - 1][j] + mat[i - 1][j + 1] + mat[i][j - 1] + mat[i][j + 1] + mat[i + 1][j - 1] + mat[i + 1][j] + mat[i + 1][j + 1]) / 8;
        }
        // miolo da primeira linha
        else if (i == 0)
        {
            return (mat[i][j - 1] + mat[i][j + 1] + mat[i + 1][j - 1] + mat[i + 1][j] + mat[i + 1][j + 1]) / 5;
        }
        // miolo da última linha
        else
        {
            return (mat[i - 1][j - 1] + mat[i - 1][j] + mat[i - 1][j + 1] + mat[i][j - 1] + mat[i][j + 1]) / 5;
        }
    }
    else if (j == 0)
    {
        // miolo da primeira coluna
        if ((i > 0) && (i < n_linhas - 1))
        {
            return (mat[i - 1][j] + mat[i - 1][j + 1] + mat[i][j + 1] + mat[i + 1][j] + mat[i + 1][j + 1]) / 5;
        }
        // canto [0][0]
        else if (i == 0)
        {
            return (mat[i][j + 1] + mat[i + 1][j] + mat[i + 1][j + 1]) / 3;
        }
        // canto [n_linhas-1][0]
        else
        {
            return (mat[i - 1][j] + mat[i - 1][j + 1] + mat[i][j + 1]) / 3;
        }
    }

    else
    {
        // miolo da última coluna
        if ((i > 0) && (i < n_linhas - 1))
        {
            return (mat[i - 1][j - 1] + mat[i - 1][j] + mat[i][j - 1] + mat[i + 1][j - 1] + mat[i + 1][j]) / 5;
        }
        // canto[0][n_colunas-1]
        else if (i == 0)
        {
            return (mat[i][j - 1] + mat[i + 1][j - 1] + mat[i + 1][j]) / 3;
        }
        // canto[n_linhas-1][n_colunas-1]
        else
        {
            return (mat[i - 1][j - 1] + mat[i - 1][j] + mat[i][j - 1]) / 3;
        }
    }
}

void *calcular(void *args)
{
    parametros *p = args;
    int i = p->linha;

    for (;;)
    {
        // Caso não sejamos a primeira linha, devemos
        // aguardar a linha acima ter feito progresso
        // o suficiente  para podermos fazer o  nosso
        // próprio progresso.
        if (i > 0)
        {
            parametros *p_cima = &lista_parametros[i - 1];

            pthread_mutex_lock(&p_cima->mutex_cond);
            while (1)
            {
                // Obtemos o valor do contador da linha acima...
                pthread_mutex_lock(&p_cima->mutex_contador);
                int contador_cima = p_cima->contador;
                pthread_mutex_unlock(&p_cima->mutex_contador);

                if (contador_cima - 1 > p->contador || contador_cima == n_colunas)
                {
                    // Caso ela esteja suficientemente à frente ou já tenha terminado,
                    // não precisamos mais esperar.
                    break;
                }
                else
                {
                    // Senão, aguardamos a linha acima fazer progresso para continuar.
                    pthread_cond_wait(&p_cima->cond, &p_cima->mutex_cond);
                }
            }
            pthread_mutex_unlock(&p_cima->mutex_cond);
        }

        mat[i][p->contador] = media_vizinhos(mat, n_linhas, n_colunas, i, p->contador);

        // Incrementamos o nosso progresso após o cálculo e
        // em seguida sinalizamos a thread abaixo, caso esteja
        // nos aguardando.
        pthread_mutex_lock(&p->mutex_contador);
        p->contador++;
        pthread_mutex_unlock(&p->mutex_contador);

        pthread_mutex_lock(&p->mutex_cond);
        pthread_cond_signal(&p->cond);
        pthread_mutex_unlock(&p->mutex_cond);

        if (p->contador == n_colunas)
        {
            return NULL;
        }
    }
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