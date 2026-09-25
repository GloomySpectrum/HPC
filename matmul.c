/*
 matmul.c - Multiplicacion de matrices cuadradas generadas aleatoriamente,
            version SECUENCIAL, con medicion de wall-clock time.


 Parametros por linea de comandos:
   N         -> dimension de las matrices cuadradas (obligatorio, entero > 0)
   MAX_VALOR -> valor maximo (exclusivo) de cada elemento aleatorio (opcional, por defecto 10)
   SEMILLA   -> semilla para el generador aleatorio (opcional, por defecto: time(NULL))

 Nota: para N > 10 no se imprimen las matrices completas (solo el tiempo),
 ya que para N grande (como 400+) el volumen de salida es ilegible y el
 propio I/O de impresion contaminaria la medicion de tiempo.

 Ejemplo:
   ./matmul 400
   ./matmul 400 20
   ./matmul 400 20 42
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include "matrix_utils.h"

#define UMBRAL_IMPRESION 10

/* Multiplica A (n x n) por B (n x n) y deja el resultado en C (n x n) */
void multiplicar(int **a, int **b, int **c, int n){
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            long suma = 0; /* long para mayor margen antes de pasar a int */
            for(int k = 0; k < n; k++){
                suma += (long)a[i][k] * (long)b[k][j];
            }
            c[i][j] = (int)suma;
        }
    }
}

int main(int argc, char *argv[]){
    if(argc < 2 || argc > 4){
        fprintf(stderr, "Uso: %s N [MAX_VALOR] [SEMILLA]\n", argv[0]);
        fprintf(stderr, "  N         : dimension de las matrices cuadradas (entero > 0)\n");
        fprintf(stderr, "  MAX_VALOR : valor maximo exclusivo por elemento (opcional, defecto 10)\n");
        fprintf(stderr, "  SEMILLA   : semilla aleatoria (opcional, defecto: tiempo actual)\n");
        return EXIT_FAILURE;
    }

    int n = atoi(argv[1]);
    if(n <= 0){
        fprintf(stderr, "Error: N debe ser un entero positivo.\n");
        return EXIT_FAILURE;
    }

    int max_valor = 10; /* valor por defecto para evitar desborde */
    if(argc >= 3){
        max_valor = atoi(argv[2]);
        if(max_valor <= 0){
            fprintf(stderr, "Error: MAX_VALOR debe ser un entero positivo.\n");
            return EXIT_FAILURE;
        }
    }

    /* Verificacion de que N * max_valor^2 no desborde un long (suma interna)
       ni el resultado final no desborde un int */
    double max_suma_posible = (double)n * (double)(max_valor - 1) * (double)(max_valor - 1);
    if(max_suma_posible > (double) INT_MAX){
        fprintf(stderr,
            "Error: con N=%d y MAX_VALOR=%d el resultado podria desbordar un int "
            "(maximo teorico por celda: %.0f). Reduce N o MAX_VALOR.\n",
            n, max_valor, max_suma_posible);
        return EXIT_FAILURE;
    }

    unsigned int semilla = (unsigned int)time(NULL);
    if (argc == 4) {
        semilla = (unsigned int)strtoul(argv[3], NULL, 10);
    }
    srand(semilla);

    printf("[SECUENCIAL] Dimension (N): %d | Valor maximo por celda: %d | Semilla: %u\n\n",
           n, max_valor, semilla);

    int **A = reservar_matriz(n);
    int **B = reservar_matriz(n);
    int **C = reservar_matriz(n);

    llenar_aleatoria(A, n, max_valor);
    llenar_aleatoria(B, n, max_valor);

    if(n <= UMBRAL_IMPRESION){
        imprimir_matriz("Matriz A", A, n);
        imprimir_matriz("Matriz B", B, n);
    }

    /* Medicion de wall-clock time solo alrededor de la multiplicacion */
    struct timespec t_inicio = obtener_tiempo();
    multiplicar(A, B, C, n);
    struct timespec t_fin = obtener_tiempo();

    if(n <= UMBRAL_IMPRESION){
        imprimir_matriz("Matriz C = A x B", C, n);
    }

    double segundos = diferencia_segundos(t_inicio, t_fin);
    printf("Tiempo de ejecucion (clock time) de la multiplicacion: %.9f segundos\n", segundos);

    liberar_matriz(A, n);
    liberar_matriz(B, n);
    liberar_matriz(C, n);

    return EXIT_SUCCESS;
}
