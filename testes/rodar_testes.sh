#!/usr/bin/env bash
cd "$(dirname "$0")/.."

RAIZ="$(pwd)"
TESTES="$RAIZ/testes"
FALHA_GERAL=0

rodar_implementacao() {
    local impl="$1"
    local dir
    dir=$(mktemp -d)
    cp "$RAIZ/src/Makefile" "$dir/"
    cp "$RAIZ/src/ListaComCursor.h" "$dir/"
    cp "$RAIZ/src/main.c" "$dir/"
    cp "$RAIZ/src/$impl.c" "$dir/"

    echo "=== $impl (make do professor) ==="
    if ! (cd "$dir" && make -s) > "$dir/build.log" 2>&1; then
        echo "FALHOU: compilacao com o Makefile do professor"
        cat "$dir/build.log"
        FALHA_GERAL=1
        rm -rf "$dir"
        return
    fi

    local total=0
    local ok=0
    for entrada in "$TESTES"/*.in; do
        [ -e "$entrada" ] || continue
        local nome
        nome=$(basename "$entrada" .in)
        local esperado="$TESTES/$nome.out"
        total=$((total + 1))
        local saida
        saida=$("$dir/main" < "$entrada")
        if diff -q <(printf '%s\n' "$saida") "$esperado" > /dev/null 2>&1; then
            echo "$nome OK"
            ok=$((ok + 1))
        else
            echo "$nome FALHOU"
            diff <(printf '%s\n' "$saida") "$esperado"
            FALHA_GERAL=1
        fi
    done
    echo "$impl: $ok/$total casos OK"

    echo "=== $impl (sanitizer) ==="
    if ! gcc -std=c99 -Wall -Wextra -pedantic -g -fsanitize=address,undefined \
        "$dir/main.c" "$dir/$impl.c" -o "$dir/main_san" > "$dir/san_build.log" 2>&1; then
        echo "FALHOU: compilacao com sanitizer"
        cat "$dir/san_build.log"
        FALHA_GERAL=1
        rm -rf "$dir"
        return
    fi

    for entrada in "$TESTES"/*.in; do
        [ -e "$entrada" ] || continue
        local nome
        nome=$(basename "$entrada" .in)
        local log="$dir/$nome.san.log"
        if ! "$dir/main_san" < "$entrada" > /dev/null 2> "$log"; then
            echo "$nome FALHOU (sanitizer, codigo de saida != 0)"
            cat "$log"
            FALHA_GERAL=1
        elif [ -s "$log" ]; then
            echo "$nome FALHOU (sanitizer reportou algo)"
            cat "$log"
            FALHA_GERAL=1
        else
            echo "$nome sanitizer OK"
        fi
    done

    rm -rf "$dir"
}

rodar_implementacao lista_sequencial
rodar_implementacao lista_encadeada

if [ "$FALHA_GERAL" -ne 0 ]; then
    echo "RESULTADO FINAL: HA FALHAS"
    exit 1
fi

echo "RESULTADO FINAL: TUDO OK"
exit 0
