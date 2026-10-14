#!/bin/bash
set -e

cd "$(dirname "$0")/.."

mkdir -p resultados graficos

gcc -std=c99 -O2 -Wall -Wextra -Isrc -o bench_seq auxiliar/benchmark.c src/lista_sequencial.c
gcc -std=c99 -O2 -Wall -Wextra -Isrc -o bench_enc auxiliar/benchmark.c src/lista_encadeada.c

PERFIL_ANTERIOR=""
if command -v powerprofilesctl >/dev/null 2>&1; then
    if PERFIL_ANTERIOR=$(powerprofilesctl get 2>/dev/null); then
        if powerprofilesctl set performance 2>/dev/null; then
            echo "Perfil de energia: performance (antes: $PERFIL_ANTERIOR)"
        else
            echo "Nao foi possivel mudar o perfil de energia (sem permissao?)"
            PERFIL_ANTERIOR=""
        fi
    else
        PERFIL_ANTERIOR=""
    fi
fi
restaurar_perfil() {
    if [ -n "$PERFIL_ANTERIOR" ]; then
        powerprofilesctl set "$PERFIL_ANTERIOR" 2>/dev/null
        echo "Perfil de energia restaurado: $PERFIL_ANTERIOR"
    fi
}
trap restaurar_perfil EXIT

echo "implementacao,caso,n,execucao,R,tempo_ns" > resultados/tempos.csv

if command -v taskset >/dev/null 2>&1; then
    echo "Rodando bench_seq (fixado na CPU 1)..."
    taskset -c 1 ./bench_seq sequencial resultados/tempos.csv
    echo "Rodando bench_enc (fixado na CPU 1)..."
    taskset -c 1 ./bench_enc encadeada resultados/tempos.csv
else
    echo "Rodando bench_seq..."
    ./bench_seq sequencial resultados/tempos.csv
    echo "Rodando bench_enc..."
    ./bench_enc encadeada resultados/tempos.csv
fi

PYTHON=python3
if ! python3 -c "import matplotlib" 2>/dev/null; then
    if [ ! -x .venv/bin/python3 ]; then
        python3 -m venv .venv
        .venv/bin/pip install --quiet matplotlib
    fi
    PYTHON=.venv/bin/python3
fi
"$PYTHON" auxiliar/grafico.py

echo "Concluido."
echo "  - resultados/tempos.csv"
echo "  - resultados/resumo.csv"
echo "  - graficos/"
