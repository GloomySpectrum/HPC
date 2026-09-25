#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "matrix_utils.h"

int **reservar_matriz(int n){
    int **m = (int **)malloc(n * sizeof(int *));
    if(m == NULL){
        fprintf(stderr, "Error: no se pudo reservar memoria (filas).\n");
        exit(EXIT_FAILURE);
    }
    for(int i = 0; i < n; i++){
        m[i] = (int *) malloc(n * sizeof(int));
        if(m[i] == NULL) {
            fprintf(stderr, "Error: no se pudo reservar memoria (columnas).\n");
            exit(EXIT_FAILURE);
        }
    }
    return m;
}

void liberar_matriz(int **m, int n){
    for(int i = 0; i < n; i++){
        free(m[i]);
    }
    free(m);
}

void llenar_aleatoria(int **m, int n, int max_valor){
    for (int i = 0; i < n; i++){
        for (int j = 0; j < n; j++){
            m[i][j] = rand() % max_valor;
        }
    }
}

void imprimir_matriz(const char *titulo, int **m, int n){
    printf("%s (%dx%d):\n", titulo, n, n);
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            printf("%6d ", m[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

struct timespec obtener_tiempo(void) {
    struct timespec ts;
    /* CLOCK_MONOTONIC: no se ve afectado por cambios en la hora del sistema,
       ideal para medir tiempo transcurrido (wall-clock time) */
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts;
}

double diferencia_segundos(struct timespec inicio, struct timespec fin){
    double segundos = (double) (fin.tv_sec - inicio.tv_sec);
    double nanosegundos = (double) (fin.tv_nsec - inicio.tv_nsec);
    return segundos + nanosegundos / 1e9;
}
