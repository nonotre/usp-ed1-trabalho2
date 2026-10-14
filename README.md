# Trabalho 2 · Estruturas de Dados I (ICMC-USP, 2026)

Lista com Cursor em duas implementações (sequencial em vetor dinâmico e duplamente encadeada) e comparação empírica de desempenho.
Enunciado em `enunciado/enunciado.pdf`.

| Parte | Responsável |
|---|---|
| `lista_encadeada.c`, `main.c` | Lorenzo Vittorio Rudnik Spelta · 17906700 |
| `lista_sequencial.c`, experimentos, relatório | João Pedro Oliveira · 17930847 |
| (a definir) | |
| (a definir) | |
| (a definir) | |

## Estrutura

```
src/ListaComCursor.h, Makefile     fornecidos pelo professor
src/main.c                         interpretador de comandos (o mesmo nos dois exercícios do Run.Codes)
src/lista_sequencial.c             vetor dinâmico, cursor = índice
src/lista_encadeada.c              duplamente encadeada, cursor = ponteiro
testes/                            10 casos N.in / N.out + rodar_testes.sh
auxiliar/benchmark.c               os 5 casos do enunciado, só pela interface
auxiliar/rodar_benchmark.sh        compila bench_seq e bench_enc, mede, gera resultados/
auxiliar/grafico.py                resultados/resumo.csv e graficos/*.png
entrega/gerar_zips.sh              zips do Run.Codes (2.1 sequencial, 2.2 encadeada)
relatorio.md                       relatório
```

## Reproduzir

```bash
./testes/rodar_testes.sh           # as duas implementações nos 10 casos, com e sem sanitizer
./auxiliar/rodar_benchmark.sh      # ~2 min; regrava resultados/ e graficos/
./entrega/gerar_zips.sh            # entrega/trabalho2.1_sequencial.zip e trabalho2.2_encadeada.zip
```

Os números do relatório vêm de uma única rodada do benchmark. Se rodar de novo, `resultados/` e `graficos/` mudam e as tabelas do relatório precisam ser atualizadas.
