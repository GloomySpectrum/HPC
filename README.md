# Multiplicación de Matrices Cuadradas (C)

Programa en C que multiplica dos matrices cuadradas de enteros positivos generados
aleatoriamente, usando reserva dinámica de memoria. Todos los parámetros se pasan
por línea de comandos (argv); el programa nunca pide datos por teclado.

## Requisitos
- gcc (o cualquier compilador C compatible con C11)
- make (opcional, para usar el Makefile)

## Compilar

```bash
make
```

o directamente:

```bash
gcc -Wall -Wextra -std=c11 -O2 -o matmul main.c
```

## Uso

```bash
./matmul N [MAX_VALOR] [SEMILLA]
```

- `N`: dimensión de las matrices cuadradas (entero positivo, obligatorio).
- `MAX_VALOR`: valor máximo (exclusivo) para cada elemento aleatorio. Por defecto 10.
- `SEMILLA`: semilla para el generador aleatorio. Por defecto se usa la hora actual
  (así los resultados cambian en cada ejecución).

El programa valida que `N * (MAX_VALOR-1)^2` no desborde un `int`, para evitar
overflow en la suma de productos de la multiplicación.

### Ejemplos

```bash
./matmul 4
./matmul 5 20
./matmul 3 15 42
```

## Limpiar binarios

```bash
make clean
```

## Subir este proyecto a GitHub

1. Crea un repositorio vacío en https://github.com/new (sin README, para no tener conflictos).
2. Desde esta carpeta, ejecuta:

```bash
git init
git add .
git commit -m "Multiplicacion de matrices cuadradas con memoria dinamica"
git branch -M main
git remote add origin https://github.com/<tu-usuario>/<tu-repo>.git
git push -u origin main
```

Reemplaza `<tu-usuario>/<tu-repo>` por la URL real de tu repositorio.
