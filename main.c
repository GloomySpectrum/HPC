/*
 * main.c - Multiplicacion de matrices cuadradas
 *
 * Caracteristicas:
 *   - Matrices cuadradas de tamano N x N
 *   - Enteros positivos generados aleatoriamente
 *   - Limite de valores para evitar desborde (overflow) en la suma de productos
 *   - Reserva dinamica de memoria (malloc / free)
 *   - TODO se recibe por linea de comandos (argv), sin pedir datos por teclado
 *
 * Uso:
 *   ./matmul N [MAX_VALOR] [SEMILLA]
 *
 *   N         -> dimension de las matrices cuadradas (obligatorio, entero > 0)
 *   MAX_VALOR -> valor maximo (exclusivo) de cada elemento aleatorio (opcional, por defecto 10)
 *   SEMILLA   -> semilla para el generador aleatorio (opcional, por defecto: time(NULL))
 *
 * Ejemplo:
 *   ./matmul 4
 *   ./matmul 5 20
 *   ./matmul 5 20 42
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

/* Reserva dinamica de una matriz N x N de enteros */
int **reservar_matriz(int n) {
    int **m = (int **) malloc(n * sizeof(int *));
    if (m == NULL) {
        fprintf(stderr, "Error: no se pudo reservar memoria (filas).\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < n; i++) {
        m[i] = (int *) malloc(n * sizeof(int));
        if (m[i] == NULL) {
            fprintf(stderr, "Error: no se pudo reservar memoria (columnas).\n");
            exit(EXIT_FAILURE);
        }
    }
    return m;
}

/* Libera la memoria de una matriz N x N */
void liberar_matriz(int **m, int n) {
    for (int i = 0; i < n; i++) {
        free(m[i]);
    }
    free(m);
}

/* Llena una matriz N x N con enteros positivos aleatorios en [0, max_valor) */
void llenar_aleatoria(int **m, int n, int max_valor) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            m[i][j] = rand() % max_valor;
        }
    }
}

/* Multiplica A (n x n) por B (n x n) y deja el resultado en C (n x n) */
void multiplicar(int **a, int **b, int **c, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            long suma = 0; /* long para mayor margen antes de pasar a int */
            for (int k = 0; k < n; k++) {
                suma += (long) a[i][k] * (long) b[k][j];
            }
            c[i][j] = (int) suma;
        }
    }
}

/* Imprime una matriz N x N con un titulo */
void imprimir_matriz(const char *titulo, int **m, int n) {
    printf("%s (%dx%d):\n", titulo, n, n);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%6d ", m[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2 || argc > 4) {
        fprintf(stderr, "Uso: %s N [MAX_VALOR] [SEMILLA]\n", argv[0]);
        fprintf(stderr, "  N         : dimension de las matrices cuadradas (entero > 0)\n");
        fprintf(stderr, "  MAX_VALOR : valor maximo exclusivo por elemento (opcional, defecto 10)\n");
        fprintf(stderr, "  SEMILLA   : semilla aleatoria (opcional, defecto: tiempo actual)\n");
        return EXIT_FAILURE;
    }

    int n = atoi(argv[1]);
    if (n <= 0) {
        fprintf(stderr, "Error: N debe ser un entero positivo.\n");
        return EXIT_FAILURE;
    }

    int max_valor = 10; /* valor por defecto para evitar desborde */
    if (argc >= 3) {
        max_valor = atoi(argv[2]);
        if (max_valor <= 0) {
            fprintf(stderr, "Error: MAX_VALOR debe ser un entero positivo.\n");
            return EXIT_FAILURE;
        }
    }

    /* Verificacion de que N * max_valor^2 no desborde un long (suma interna)
       ni el resultado final no desborde un int */
    double max_suma_posible = (double) n * (double) (max_valor - 1) * (double) (max_valor - 1);
    if (max_suma_posible > (double) INT_MAX) {
        fprintf(stderr,
            "Error: con N=%d y MAX_VALOR=%d el resultado podria desbordar un int "
            "(maximo teorico por celda: %.0f). Reduce N o MAX_VALOR.\n",
            n, max_valor, max_suma_posible);
        return EXIT_FAILURE;
    }

    unsigned int semilla = (unsigned int) time(NULL);
    if (argc == 4) {
        semilla = (unsigned int) strtoul(argv[3], NULL, 10);
    }
    srand(semilla);

    printf("Dimension (N): %d | Valor maximo por celda: %d | Semilla: %u\n\n",
           n, max_valor, semilla);

    int **A = reservar_matriz(n);
    int **B = reservar_matriz(n);
    int **C = reservar_matriz(n);

    llenar_aleatoria(A, n, max_valor);
    llenar_aleatoria(B, n, max_valor);

    imprimir_matriz("Matriz A", A, n);
    imprimir_matriz("Matriz B", B, n);

    multiplicar(A, B, C, n);

    imprimir_matriz("Matriz C = A x B", C, n);

    liberar_matriz(A, n);
    liberar_matriz(B, n);
    liberar_matriz(C, n);

    return EXIT_SUCCESS;
}
