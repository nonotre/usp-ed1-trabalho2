#ifndef LISTA_COM_CURSOR_H
#define LISTA_COM_CURSOR_H

typedef struct lista_cursor ListaCursor;

// Criacao e Destruicao
ListaCursor* criar_lista(void);
void destruir_lista(ListaCursor* l);

// Navegacao do Cursor
void mover_inicio(ListaCursor* l);
void mover_fim(ListaCursor* l);

// Retorna 1 se avancou; 0 se esta no fim ou a lista esta vazia.
// Em caso de retorno 0, o cursor permanece inalterado.
int avancar_cursor(ListaCursor* l);

// Retorna 1 se retrocedeu; 0 se esta no inicio ou a lista esta vazia.
// Em caso de retorno 0, o cursor permanece inalterado.
int retroceder_cursor(ListaCursor* l);

// Indices comecam em 0. Retorna 1 se o indice for valido;
// caso contrario, retorna 0 e preserva o cursor.
int mover_para_indice(ListaCursor* l, int indice);

// Consulta
// Retorna 1 se obteve o valor; 0 se a lista esta vazia.
// Em caso de retorno 0, *valor_saida permanece inalterado.
int obter_atual(ListaCursor* l, int* valor_saida);

// Modificacao
int inserir_antes(ListaCursor* l, int valor);  // Insere antes do cursor
int inserir_depois(ListaCursor* l, int valor); // Insere depois do cursor
int remover_atual(ListaCursor* l);            // Remove o elemento atual

// Informacao
int tamanho_lista(ListaCursor* l);

#endif
