/*
 matmul_hilos.c - Multiplicacion de matrices cuadradas generadas
                   con medicion de wall-clock time.


 Estrategia de paralelizacion: particion por filas. Cada hilo calcula un
 bloque contiguo de filas de C, por lo que no hay condiciones de carrera
 (cada hilo escribe en una region distinta de la matriz resultado).

 Parametros por linea de comandos:
   N         -> dimension de las matrices cuadradas (obligatorio, entero > 0)
   NUM_HILOS -> cantidad de hilos a usar (obligatorio, entero > 0; ej: 2, 4, 8, 16)
   MAX_VALOR -> valor maximo (exclusivo) de cada elemento aleatorio (opcional, por defecto 10)
   SEMILLA   -> semilla para el generador aleatorio (opcional, por defecto: time(NULL))

 Nota: para N > 10 no se imprimen las matrices completas (solo el tiempo),
 ya que para N grande (como 400+) el volumen de salida es ilegible y el
 propio I/O de impresion contaminaria la medicion de tiempo.

 Ejemplo:
   ./matmul_hilos 400 4
   ./matmul_hilos 400 8 20
   ./matmul_hilos 400 16 20 42
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <pthread.h>
#include "matrix_utils.h"

#define UMBRAL_IMPRESION 10

/* Argumentos que recibe cada hilo: las tres matrices, su dimension,
   y el rango de filas [fila_inicio, fila_fin) que le toca calcular */
typedef struct {
    int **a;
    int **b;
    int **c;
    int n;
    int fila_inicio;
    int fila_fin;
} ArgsHilo;

/* Funcion que ejecuta cada hilo: calcula unicamente su bloque de filas de C */
void *multiplicar_hilo(void *arg){
    ArgsHilo *args = (ArgsHilo *) arg;

    for(int i = args->fila_inicio; i < args->fila_fin; i++){
        for(int j = 0; j < args->n; j++){
            long suma = 0; /* long para mayor margen antes de pasar a int */
            for(int k = 0; k < args->n; k++){
                suma += (long)args->a[i][k] * (long)args->b[k][j];
            }
            args->c[i][j] = (int)suma;
        }
    }

    return NULL;
}

/* Multiplica A (n x n) por B (n x n) repartiendo las filas de C entre
   num_hilos hilos. Si num_hilos > n, los hilos sobrantes reciben un
   rango vacio (fila_inicio == fila_fin) y no hacen trabajo. */
void multiplicar_paralelo(int **a, int **b, int **c, int n, int num_hilos){
    pthread_t *hilos = (pthread_t *)malloc(num_hilos * sizeof(pthread_t));
    ArgsHilo *args = (ArgsHilo *)malloc(num_hilos * sizeof(ArgsHilo));
    if(hilos == NULL || args == NULL){
        fprintf(stderr, "Error: no se pudo reservar memoria para los hilos.\n");
        exit(EXIT_FAILURE);
    }

    /* Reparto de filas: las primeras (n % num_hilos) hilos reciben una
       fila extra, para que el reparto quede lo mas parejo posible */
    int filas_por_hilo = n / num_hilos;
    int filas_extra = n % num_hilos;
    int fila_actual = 0;

    for(int t = 0; t < num_hilos; t++){
        int filas_de_este_hilo = filas_por_hilo + (t < filas_extra ? 1 : 0);

        args[t].a = a;
        args[t].b = b;
        args[t].c = c;
        args[t].n = n;
        args[t].fila_inicio = fila_actual;
        args[t].fila_fin = fila_actual + filas_de_este_hilo;

        fila_actual += filas_de_este_hilo;

        int rc = pthread_create(&hilos[t], NULL, multiplicar_hilo, &args[t]);
        if(rc != 0){
            fprintf(stderr, "Error: no se pudo crear el hilo %d (codigo %d).\n", t, rc);
            exit(EXIT_FAILURE);
        }
    }

    for(int t = 0; t < num_hilos; t++){
        pthread_join(hilos[t], NULL);
    }

    free(hilos);
    free(args);
}

int main(int argc, char *argv[]){
    if(argc < 3 || argc > 5){
        fprintf(stderr, "Uso: %s N NUM_HILOS [MAX_VALOR] [SEMILLA]\n", argv[0]);
        fprintf(stderr, "  N         : dimension de las matrices cuadradas (entero > 0)\n");
        fprintf(stderr, "  NUM_HILOS : cantidad de hilos a usar (entero > 0, ej: 2, 4, 8, 16)\n");
        fprintf(stderr, "  MAX_VALOR : valor maximo exclusivo por elemento (opcional, defecto 10)\n");
        fprintf(stderr, "  SEMILLA   : semilla aleatoria (opcional, defecto: tiempo actual)\n");
        return EXIT_FAILURE;
    }

    int n = atoi(argv[1]);
    if(n <= 0){
        fprintf(stderr, "Error: N debe ser un entero positivo.\n");
        return EXIT_FAILURE;
    }

    int num_hilos = atoi(argv[2]);
    if(num_hilos <= 0){
        fprintf(stderr, "Error: NUM_HILOS debe ser un entero positivo.\n");
        return EXIT_FAILURE;
    }
    if(num_hilos > n){
        fprintf(stderr,
            "Advertencia: NUM_HILOS (%d) es mayor que N (%d); algunos hilos no tendran filas que calcular.\n",
            num_hilos, n);
    }

    int max_valor = 10; /* valor por defecto para evitar desborde */
    if(argc >= 4){
        max_valor = atoi(argv[3]);
        if(max_valor <= 0){
            fprintf(stderr, "Error: MAX_VALOR debe ser un entero positivo.\n");
            return EXIT_FAILURE;
        }
    }

    /* Verificacion de que N * max_valor^2 no desborde un long (suma interna)
       ni el resultado final no desborde un int */
    double max_suma_posible = (double)n * (double)(max_valor - 1) * (double)(max_valor - 1);
    if(max_suma_posible > (double)INT_MAX){
        fprintf(stderr,
            "Error: con N=%d y MAX_VALOR=%d el resultado podria desbordar un int "
            "(maximo teorico por celda: %.0f). Reduce N o MAX_VALOR.\n",
            n, max_valor, max_suma_posible);
        return EXIT_FAILURE;
    }

    unsigned int semilla = (unsigned int)time(NULL);
    if(argc == 5){
        semilla = (unsigned int) strtoul(argv[4], NULL, 10);
    }
    srand(semilla);

    printf("[HILOS] Dimension (N): %d | Hilos: %d | Valor maximo por celda: %d | Semilla: %u\n\n",
           n, num_hilos, max_valor, semilla);

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
    multiplicar_paralelo(A, B, C, n, num_hilos);
    struct timespec t_fin = obtener_tiempo();

    if(n <= UMBRAL_IMPRESION){
        imprimir_matriz("Matriz C = A x B", C, n);
    }

    double segundos = diferencia_segundos(t_inicio, t_fin);
    printf("Tiempo de ejecucion (clock time) de la multiplicacion con %d hilo(s): %.9f segundos\n",
           num_hilos, segundos);

    liberar_matriz(A, n);
    liberar_matriz(B, n);
    liberar_matriz(C, n);

    return EXIT_SUCCESS;
}
