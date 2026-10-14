#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "ListaComCursor.h"

#define NUM_TAMANHOS 5
#define NUM_EXECUCOES_PADRAO 10
#define NUM_OPS 100
#define NUM_CASOS 5

static const long TAMANHOS_PADRAO[NUM_TAMANHOS] = {1000, 50000, 100000, 500000, 1000000};

static const long R_TABELA[NUM_CASOS][NUM_TAMANHOS] = {
    {8000, 320, 160, 32, 16},
    {2000, 80, 40, 8, 4},
    {8000, 320, 160, 32, 16},
    {8000, 320, 160, 32, 16},
    {20000, 800, 400, 80, 40}
};

static double agora_ns(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec * 1e9 + (double)ts.tv_nsec;
}

static unsigned seed_para(long n, int execucao, int sal)
{
    return (unsigned)((unsigned long)n * 1000003u + (unsigned)execucao * 97u + (unsigned)sal * 7919u + 12345u);
}

static int *gerar_inteiros(long qtd, unsigned seed)
{
    int *v = malloc(sizeof(int) * (size_t)qtd);

    if (v == NULL)
        return NULL;   // falha de alocacao

    srand(seed);
    for (long i = 0; i < qtd; i++)
        v[i] = rand();

    return v;
}

static int *gerar_indices(long qtd, long n, unsigned seed)
{
    int *v = malloc(sizeof(int) * (size_t)qtd);

    if (v == NULL)
        return NULL;   // falha de alocacao

    srand(seed);
    for (long i = 0; i < qtd; i++)
        v[i] = (int)(rand() % n);

    return v;
}

static ListaCursor *construir_lista(const int *valores, long n, int *ok)
{
    ListaCursor *l = criar_lista();

    *ok = 1;
    if (l == NULL) {
        *ok = 0;
        return NULL;   // falha de alocacao
    }
    if (n == 0)
        return l;

    if (!inserir_depois(l, valores[0])) {
        *ok = 0;
        return l;
    }
    for (long i = 1; i < n; i++) {
        if (!inserir_depois(l, valores[i])) {
            *ok = 0;
            break;
        }
        avancar_cursor(l);
    }

    return l;
}

static void bloco_caso1(ListaCursor *l, const int *valores_insercao, int *falhou)
{
    for (int i = 0; i < NUM_OPS; i++) {
        mover_inicio(l);
        if (!inserir_antes(l, valores_insercao[i]))
            *falhou = 1;
    }
}

static void bloco_caso2(ListaCursor *l, const int *indices, int *falhou)
{
    int valor;

    for (int i = 0; i < NUM_OPS; i++) {
        if (!mover_para_indice(l, indices[i]))
            *falhou = 1;
        if (!obter_atual(l, &valor))
            *falhou = 1;
    }
}

static void bloco_caso3(ListaCursor *l, int *falhou)
{
    for (int i = 0; i < NUM_OPS; i++) {
        if (!remover_atual(l))
            *falhou = 1;
    }
}

static void bloco_caso4(ListaCursor *l, int *falhou)
{
    for (int i = 0; i < NUM_OPS; i++) {
        if (!remover_atual(l))
            *falhou = 1;
    }
}

static void bloco_caso5(ListaCursor *l, int *falhou)
{
    volatile long acumulador = 0;
    int valor;
    int ok = obter_atual(l, &valor);

    while (ok) {
        acumulador += valor;
        if (!avancar_cursor(l))
            break;
        ok = obter_atual(l, &valor);
    }
    if (!ok)
        *falhou = 1;
    (void)acumulador;
}

static double executar_caso(int caso, ListaCursor *l, const int *valores, long n,
                             const int *dados_extra, long R, int *falhou)
{
    double soma = 0.0;
    long meio = n / 2;

    for (long rep = 0; rep < R; rep++) {
        if (caso == 3 || caso == 5)
            mover_inicio(l);
        else if (caso == 4)
            mover_para_indice(l, (int)meio);

        double t0 = agora_ns();

        if (caso == 1)
            bloco_caso1(l, dados_extra, falhou);
        else if (caso == 2)
            bloco_caso2(l, dados_extra, falhou);
        else if (caso == 3)
            bloco_caso3(l, falhou);
        else if (caso == 4)
            bloco_caso4(l, falhou);
        else
            bloco_caso5(l, falhou);

        double t1 = agora_ns();

        if (*falhou)
            return 0.0;

        soma += (t1 - t0);

        if (caso == 1) {
            for (int i = 0; i < NUM_OPS; i++) {
                mover_inicio(l);
                if (!remover_atual(l))
                    *falhou = 1;
            }
        } else if (caso == 3) {
            for (int i = 0; i < NUM_OPS; i++) {
                if (!inserir_antes(l, valores[i]))
                    *falhou = 1;
            }
        } else if (caso == 4) {
            for (int i = 0; i < NUM_OPS; i++) {
                if (!inserir_antes(l, valores[meio + i]))
                    *falhou = 1;
            }
        }

        if (*falhou)
            return 0.0;
    }

    return soma / (double)R;
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "uso: %s <nome_implementacao> [caminho_csv] [execucoes]\n", argv[0]);
        return 1;   // argumentos insuficientes
    }

    const char *nome_impl = argv[1];
    const char *caminho_csv = (argc >= 3) ? argv[2] : "resultados/tempos.csv";
    int num_execucoes = (argc >= 4) ? atoi(argv[3]) : NUM_EXECUCOES_PADRAO;

    if (num_execucoes <= 0)
        num_execucoes = NUM_EXECUCOES_PADRAO;

    FILE *csv = fopen(caminho_csv, "a");

    if (csv == NULL) {
        fprintf(stderr, "erro: nao foi possivel abrir %s\n", caminho_csv);
        return 1;   // falha ao abrir arquivo de saida
    }

    double inicio_total = agora_ns();

    for (int caso = 1; caso <= NUM_CASOS; caso++) {
        for (int idx_n = 0; idx_n < NUM_TAMANHOS; idx_n++) {
            long n = TAMANHOS_PADRAO[idx_n];
            long R = R_TABELA[caso - 1][idx_n];

            for (int execucao = 1; execucao <= num_execucoes; execucao++) {
                int *valores = gerar_inteiros(n, seed_para(n, execucao, 0));
                int *dados_extra = NULL;

                if (valores == NULL) {
                    fprintf(stderr, "%s caso=%d n=%ld execucao=%d: falha ao gerar valores iniciais\n",
                            nome_impl, caso, n, execucao);
                    continue;
                }

                if (caso == 1) {
                    dados_extra = gerar_inteiros(NUM_OPS, seed_para(n, execucao, 1));
                } else if (caso == 2) {
                    dados_extra = gerar_indices(NUM_OPS, n, seed_para(n, execucao, 2));
                }

                if ((caso == 1 || caso == 2) && dados_extra == NULL) {
                    fprintf(stderr, "%s caso=%d n=%ld execucao=%d: falha ao gerar dados auxiliares\n",
                            nome_impl, caso, n, execucao);
                    free(valores);
                    continue;
                }

                int falhou = 0;
                double tempo_ns = 0.0;
                int ok_construcao;
                ListaCursor *l = construir_lista(valores, n, &ok_construcao);

                if (!ok_construcao)
                    falhou = 1;
                else
                    tempo_ns = executar_caso(caso, l, valores, n, dados_extra, R, &falhou);
                if (l != NULL)
                    destruir_lista(l);

                if (falhou) {
                    fprintf(stderr, "%s caso=%d n=%ld execucao=%d: falha durante a medicao, descartada\n",
                            nome_impl, caso, n, execucao);
                } else {
                    fprintf(csv, "%s,%d,%ld,%d,%ld,%.3f\n", nome_impl, caso, n, execucao, R, tempo_ns);
                    fflush(csv);
                    fprintf(stderr, "%s caso=%d n=%ld execucao=%d/%d R=%ld tempo_ns=%.1f\n",
                            nome_impl, caso, n, execucao, num_execucoes, R, tempo_ns);
                }

                free(valores);
                free(dados_extra);
            }
        }
    }

    double fim_total = agora_ns();

    fprintf(stderr, "%s: tempo total %.2f s\n", nome_impl, (fim_total - inicio_total) / 1e9);

    fclose(csv);

    return 0;
}
