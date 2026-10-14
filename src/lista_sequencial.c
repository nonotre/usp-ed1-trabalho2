#include <stdlib.h>
#include "ListaComCursor.h"

#define CAPACIDADE_INICIAL 16

struct lista_cursor {
    int *v;
    int qtd;
    int cap;
    int cursor;
};

ListaCursor *criar_lista(void)
{
    ListaCursor *l = malloc(sizeof(ListaCursor));

    if (l == NULL)
        return NULL;   // falha de alocacao

    l->v = malloc(sizeof(int) * CAPACIDADE_INICIAL);

    if (l->v == NULL) {
        free(l);
        return NULL;   // falha de alocacao do vetor
    }

    l->qtd = 0;
    l->cap = CAPACIDADE_INICIAL;
    l->cursor = -1;

    return l;
}

void destruir_lista(ListaCursor *l)
{
    if (l == NULL)
        return;   // lista nula: nada a liberar

    free(l->v);
    free(l);
}

void mover_inicio(ListaCursor *l)
{
    if (l != NULL && l->qtd > 0)
        l->cursor = 0;
}

void mover_fim(ListaCursor *l)
{
    if (l != NULL && l->qtd > 0)
        l->cursor = l->qtd - 1;
}

int avancar_cursor(ListaCursor *l)
{
    if (l == NULL || l->cursor == -1 || l->cursor >= l->qtd - 1)
        return 0;   // lista vazia ou cursor ja no fim

    l->cursor++;
    return 1;
}

int retroceder_cursor(ListaCursor *l)
{
    if (l == NULL || l->cursor == -1 || l->cursor == 0)
        return 0;   // lista vazia ou cursor ja no inicio

    l->cursor--;
    return 1;
}

int mover_para_indice(ListaCursor *l, int indice)
{
    if (l == NULL || indice < 0 || indice >= l->qtd)
        return 0;   // indice invalido

    l->cursor = indice;
    return 1;
}

int obter_atual(ListaCursor *l, int *valor_saida)
{
    if (l == NULL || l->cursor == -1 || valor_saida == NULL)
        return 0;   // lista vazia

    *valor_saida = l->v[l->cursor];
    return 1;
}

static int garantir_capacidade(ListaCursor *l)
{
    if (l->qtd < l->cap)
        return 1;

    int nova_cap = l->cap * 2;
    int *novo_v = realloc(l->v, sizeof(int) * nova_cap);

    if (novo_v == NULL)
        return 0;   // realloc falhou: vetor original intacto

    l->v = novo_v;
    l->cap = nova_cap;
    return 1;
}

int inserir_antes(ListaCursor *l, int valor)
{
    if (l == NULL)
        return 0;   // ponteiro nulo

    if (garantir_capacidade(l) == 0)
        return 0;   // falha de alocacao: lista intacta

    if (l->cursor == -1) {
        l->v[0] = valor;
        l->qtd++;
        l->cursor = 0;
        return 1;
    }

    for (int i = l->qtd; i > l->cursor; i--)
        l->v[i] = l->v[i - 1];

    l->v[l->cursor] = valor;
    l->qtd++;
    l->cursor++;

    return 1;
}

int inserir_depois(ListaCursor *l, int valor)
{
    if (l == NULL)
        return 0;   // ponteiro nulo

    if (garantir_capacidade(l) == 0)
        return 0;   // falha de alocacao: lista intacta

    if (l->cursor == -1) {
        l->v[0] = valor;
        l->qtd++;
        l->cursor = 0;
        return 1;
    }

    for (int i = l->qtd; i > l->cursor + 1; i--)
        l->v[i] = l->v[i - 1];

    l->v[l->cursor + 1] = valor;
    l->qtd++;

    return 1;
}

int remover_atual(ListaCursor *l)
{
    if (l == NULL || l->cursor == -1)
        return 0;   // lista vazia

    for (int i = l->cursor; i < l->qtd - 1; i++)
        l->v[i] = l->v[i + 1];

    l->qtd--;

    if (l->cursor == l->qtd)
        l->cursor--;

    if (l->qtd == 0)
        l->cursor = -1;   // lista ficou vazia

    return 1;
}

int tamanho_lista(ListaCursor *l)
{
    if (l == NULL)
        return 0;   // lista nula

    return l->qtd;
}
