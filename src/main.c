#include <stdio.h>
#include "ListaComCursor.h"

int main(void)
{
    ListaCursor *l = criar_lista();

    if (l == NULL)
        return 1;

    int opcao;
    int valor;
    int indice;

    while (scanf("%d", &opcao) == 1) {
        switch (opcao) {
            case 0:
                destruir_lista(l);
                return 0;

            case 1:
                if (scanf("%d", &valor) != 1) {
                    destruir_lista(l);
                    return 1;
                }
                inserir_antes(l, valor);
                break;

            case 2:
                if (scanf("%d", &valor) != 1) {
                    destruir_lista(l);
                    return 1;
                }
                inserir_depois(l, valor);
                break;

            case 3:
                remover_atual(l);
                break;

            case 4:
                if (obter_atual(l, &valor) == 1)
                    printf("%d\n", valor);
                else
                    printf("ERRO\n");
                break;

            case 5:
                mover_inicio(l);
                break;

            case 6:
                mover_fim(l);
                break;

            case 7:
                printf("%d\n", avancar_cursor(l));
                break;

            case 8:
                printf("%d\n", retroceder_cursor(l));
                break;

            case 9:
                if (scanf("%d", &indice) != 1) {
                    destruir_lista(l);
                    return 1;
                }
                printf("%d\n", mover_para_indice(l, indice));
                break;

            case 10:
                printf("%d\n", tamanho_lista(l));
                break;

            default:
                break;
        }
    }

    destruir_lista(l);
    return 0;
}
