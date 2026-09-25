#!/bin/bash
#
# benchmark.sh - Ejecuta la version secuencial y la version con hilos para
#                varios tamanos de N y varios numeros de hilos, repitiendo
#                cada configuracion varias veces, y guarda los resultados
#                (wall clock time) en un archivo CSV.
#
# Uso:
#   ./benchmark.sh [SALIDA_CSV]
#
# Edita las variables N_VALORES, HILOS_VALORES y REPETICIONES segun lo que
# necesites para tu informe.

set -e

# ------------------------- CONFIGURACION -------------------------
N_VALORES=(500 1000 2000)
HILOS_VALORES=(2 4 8 12)
REPETICIONES=3
MAX_VALOR=20
SEMILLA=42
SALIDA_CSV="${1:-resultados_benchmark.csv}"
# -------------------------------------------------------------------

# Extrae el numero de segundos de la linea "Tiempo de ejecucion ... : X segundos"
extraer_segundos() {
    echo "$1" | grep -oP 'multiplicacion[^:]*:\s*\K[0-9.]+'
}

echo "implementacion,N,hilos,corrida,segundos" > "$SALIDA_CSV"

echo "=== Corriendo version SECUENCIAL ==="
for n in "${N_VALORES[@]}"; do
    for r in $(seq 1 "$REPETICIONES"); do
        echo "  N=$n, corrida $r/$REPETICIONES ..."
        salida=$(./matmul "$n" "$MAX_VALOR" "$SEMILLA")
        segundos=$(extraer_segundos "$salida")
        echo "secuencial,$n,1,$r,$segundos" >> "$SALIDA_CSV"
    done
done

echo "=== Corriendo version con HILOS ==="
for n in "${N_VALORES[@]}"; do
    for h in "${HILOS_VALORES[@]}"; do
        for r in $(seq 1 "$REPETICIONES"); do
            echo "  N=$n, hilos=$h, corrida $r/$REPETICIONES ..."
            salida=$(./matmul_hilos "$n" "$h" "$MAX_VALOR" "$SEMILLA")
            segundos=$(extraer_segundos "$salida")
            echo "hilos,$n,$h,$r,$segundos" >> "$SALIDA_CSV"
        done
    done
done

echo ""
echo "Listo. Resultados guardados en: $SALIDA_CSV"
