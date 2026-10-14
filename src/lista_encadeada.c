#include <stdlib.h>
#include "ListaComCursor.h"

typedef struct no {
    int valor;
    struct no *anterior;
    struct no *proximo;
} No;

struct lista_cursor {
    No *inicio;
    No *fim;
    No *cursor;
    int qtd;
};

static No *criar_no(int valor)
{
    No *novo = malloc(sizeof(No));

    if (novo == NULL)
        return NULL;   // falha de alocacao

    novo->valor = valor;
    novo->anterior = NULL;
    novo->proximo = NULL;

    return novo;
}

ListaCursor *criar_lista(void)
{
    ListaCursor *l = malloc(sizeof(ListaCursor));

    if (l == NULL)
        return NULL;   // falha de alocacao

    l->inicio = NULL;
    l->fim = NULL;
    l->cursor = NULL;
    l->qtd = 0;

    return l;
}

void destruir_lista(ListaCursor *l)
{
    if (l == NULL)
        return;   // lista nula: nada a liberar

    No *atual = l->inicio;

    while (atual != NULL) {
        No *proximo = atual->proximo;
        free(atual);
        atual = proximo;
    }

    free(l);
}

void mover_inicio(ListaCursor *l)
{
    if (l != NULL && l->inicio != NULL)
        l->cursor = l->inicio;
}

void mover_fim(ListaCursor *l)
{
    if (l != NULL && l->fim != NULL)
        l->cursor = l->fim;
}

int avancar_cursor(ListaCursor *l)
{
    if (l == NULL || l->cursor == NULL || l->cursor->proximo == NULL)
        return 0;   // lista vazia ou cursor ja no fim

    l->cursor = l->cursor->proximo;
    return 1;
}

int retroceder_cursor(ListaCursor *l)
{
    if (l == NULL || l->cursor == NULL || l->cursor->anterior == NULL)
        return 0;   // lista vazia ou cursor ja no inicio

    l->cursor = l->cursor->anterior;
    return 1;
}

int mover_para_indice(ListaCursor *l, int indice)
{
    if (l == NULL || indice < 0 || indice >= l->qtd)
        return 0;   // indice invalido

    No *atual;

    if (indice <= l->qtd - 1 - indice) {
        atual = l->inicio;

        for (int i = 0; i < indice; i++)
            atual = atual->proximo;
    } else {
        atual = l->fim;

        for (int i = l->qtd - 1; i > indice; i--)
            atual = atual->anterior;
    }

    l->cursor = atual;
    return 1;
}

int obter_atual(ListaCursor *l, int *valor_saida)
{
    if (l == NULL || l->cursor == NULL || valor_saida == NULL)
        return 0;   // lista vazia

    *valor_saida = l->cursor->valor;
    return 1;
}

int inserir_antes(ListaCursor *l, int valor)
{
    if (l == NULL)
        return 0;   // ponteiro nulo

    No *novo = criar_no(valor);

    if (novo == NULL)
        return 0;   // falha de alocacao: lista intacta

    if (l->cursor == NULL) {
        l->inicio = novo;
        l->fim = novo;
        l->cursor = novo;
    } else {
        novo->anterior = l->cursor->anterior;
        novo->proximo = l->cursor;

        if (l->cursor->anterior != NULL)
            l->cursor->anterior->proximo = novo;
        else
            l->inicio = novo;

        l->cursor->anterior = novo;
    }

    l->qtd++;
    return 1;
}

int inserir_depois(ListaCursor *l, int valor)
{
    if (l == NULL)
        return 0;   // ponteiro nulo

    No *novo = criar_no(valor);

    if (novo == NULL)
        return 0;   // falha de alocacao: lista intacta

    if (l->cursor == NULL) {
        l->inicio = novo;
        l->fim = novo;
        l->cursor = novo;
    } else {
        novo->anterior = l->cursor;
        novo->proximo = l->cursor->proximo;

        if (l->cursor->proximo != NULL)
            l->cursor->proximo->anterior = novo;
        else
            l->fim = novo;

        l->cursor->proximo = novo;
    }

    l->qtd++;
    return 1;
}

int remover_atual(ListaCursor *l)
{
    if (l == NULL || l->cursor == NULL)
        return 0;   // lista vazia

    No *removido = l->cursor;

    if (removido->anterior != NULL)
        removido->anterior->proximo = removido->proximo;
    else
        l->inicio = removido->proximo;

    if (removido->proximo != NULL)
        removido->proximo->anterior = removido->anterior;
    else
        l->fim = removido->anterior;

    if (removido->proximo != NULL)
        l->cursor = removido->proximo;
    else
        l->cursor = removido->anterior;

    free(removido);
    l->qtd--;

    return 1;
}

int tamanho_lista(ListaCursor *l)
{
    if (l == NULL)
        return 0;   // lista nula

    return l->qtd;
}
