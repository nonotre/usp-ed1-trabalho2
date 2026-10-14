# Trabalho 2 · Estruturas de Dados I (ICMC-USP, 2026)

Lista com Cursor em vetor dinâmico e em lista duplamente encadeada, com comparação de desempenho. Enunciado em `enunciado/enunciado.pdf`, relatório em `relatorio.pdf`.

| Parte | Responsável |
|---|---|
| `lista_encadeada.c`, `main.c` | Lorenzo Vittorio Rudnik Spelta · 17906700 |
| `lista_sequencial.c`, experimentos, relatório | João Pedro Oliveira · 17930847 |
| (a definir) | João Victor Pinheiro Lacerda · 15512277 |
| (a definir) | Pietro Augusto Venancio da Costa · 17839104 |
| (a definir) | Matheus de Souza Santana · (nº USP pendente) |

## Pastas

| Pasta | O que tem |
|---|---|
| `src/` | código entregue: `main.c`, as duas implementações, `ListaComCursor.h` e `Makefile` do professor |
| `testes/` | 10 casos `.in`/`.out` e `rodar_testes.sh` |
| `auxiliar/` | `benchmark.c`, `rodar_benchmark.sh`, `grafico.py` |
| `resultados/`, `graficos/` | dados e gráficos usados no relatório |
| `entrega/` | `gerar_zips.sh`, que monta os zips do Run.Codes |

## Comandos

```bash
./testes/rodar_testes.sh        # testa as duas implementações (com sanitizer)
./auxiliar/rodar_benchmark.sh   # mede tudo (~2 min) e regrava resultados/ e graficos/
./entrega/gerar_zips.sh         # entrega/trabalho2.1_sequencial.zip e trabalho2.2_encadeada.zip
python3 gerar_pdf.py            # relatorio.md → relatorio.pdf com capa e sumário (precisa de markdown e chromium)
```

Rodar o benchmark de novo muda os números; aí as tabelas do relatório precisam ser atualizadas.
