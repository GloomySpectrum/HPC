CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2

matmul: main.c
	$(CC) $(CFLAGS) -o matmul main.c

clean:
	rm -f matmul

.PHONY: clean
