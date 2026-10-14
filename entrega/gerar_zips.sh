#!/usr/bin/env bash
cd "$(dirname "$0")/.."

RAIZ="$(pwd)"
ENTREGA="$RAIZ/entrega"
SRC="$RAIZ/src"
FALHA_GERAL=0

gerar_zip() {
    local nome_zip="$1"
    local impl="$2"
    local dir
    dir=$(mktemp -d)
    cp "$SRC/Makefile" "$dir/"
    cp "$SRC/ListaComCursor.h" "$dir/"
    cp "$SRC/main.c" "$dir/"
    cp "$SRC/$impl.c" "$dir/"

    rm -f "$ENTREGA/$nome_zip"
    (cd "$dir" && zip -q "$ENTREGA/$nome_zip" Makefile ListaComCursor.h main.c "$impl.c")
    rm -rf "$dir"
}

verificar_zip() {
    local nome_zip="$1"
    local dir
    dir=$(mktemp -d)
    unzip -q "$ENTREGA/$nome_zip" -d "$dir"

    if ! (cd "$dir" && make -s) > "$dir/build.log" 2>&1; then
        echo "FALHOU: $nome_zip nao compila isoladamente"
        cat "$dir/build.log"
        FALHA_GERAL=1
        rm -rf "$dir"
        return
    fi

    local esperado="$RAIZ/testes/1.out"
    local saida
    saida=$("$dir/main" < "$RAIZ/testes/1.in")
    if diff -q <(printf '%s\n' "$saida") "$esperado" > /dev/null 2>&1; then
        echo "$nome_zip OK (compila e passa o caso 1)"
    else
        echo "FALHOU: $nome_zip nao bate no caso 1"
        diff <(printf '%s\n' "$saida") "$esperado"
        FALHA_GERAL=1
    fi

    rm -rf "$dir"
}

mkdir -p "$ENTREGA"
gerar_zip trabalho2.1_sequencial.zip lista_sequencial
gerar_zip trabalho2.2_encadeada.zip lista_encadeada

verificar_zip trabalho2.1_sequencial.zip
verificar_zip trabalho2.2_encadeada.zip

if [ "$FALHA_GERAL" -ne 0 ]; then
    echo "RESULTADO FINAL: HA FALHAS"
    exit 1
fi

echo "RESULTADO FINAL: TUDO OK"
exit 0
