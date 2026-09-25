CC = gcc
CFLAGS = -Wall -Wextra -O2

all: matmul matmul_hilos

matmul: matmul.c matrix_utils.c matrix_utils.h
	$(CC) $(CFLAGS) -o matmul matmul.c matrix_utils.c

matmul_hilos: matmul_hilos.c matrix_utils.c matrix_utils.h
	$(CC) $(CFLAGS) -pthread -o matmul_hilos matmul_hilos.c matrix_utils.c -pthread

clean:
	rm -f matmul matmul_hilos
