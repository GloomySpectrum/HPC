#ifndef MATRIX_UTILS_H
#define MATRIX_UTILS_H

#include <time.h>

/* Reserva dinamica de una matriz N x N de enteros */
int **reservar_matriz(int n);

/* Libera la memoria de una matriz N x N */
void liberar_matriz(int **m, int n);

/* Llena una matriz N x N con enteros positivos aleatorios en [0, max_valor) */
void llenar_aleatoria(int **m, int n, int max_valor);

/* Imprime una matriz N x N con un titulo */
void imprimir_matriz(const char *titulo, int **m, int n);

/* Devuelve la marca de tiempo actual (wall-clock, alta resolucion) */
struct timespec obtener_tiempo(void);

/* Calcula la diferencia en segundos entre dos marcas de obtener_tiempo() */
double diferencia_segundos(struct timespec inicio, struct timespec fin);

#endif /* MATRIX_UTILS_H */
