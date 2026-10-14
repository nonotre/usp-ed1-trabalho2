import csv
import os
import statistics

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

RAIZ = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
CAMINHO_TEMPOS = os.path.join(RAIZ, "resultados", "tempos.csv")
CAMINHO_RESUMO = os.path.join(RAIZ, "resultados", "resumo.csv")
PASTA_GRAFICOS = os.path.join(RAIZ, "graficos")

COR = {"sequencial": "#1b6ca8", "encadeada": "#d9730d"}
ORDEM_IMPL = ["sequencial", "encadeada"]

CASOS = {
    1: ("caso1_insercao_inicio", "Caso 1: Inserção no início (mover_inicio + inserir_antes)"),
    2: ("caso2_acesso_indice", "Caso 2: Acesso por índice (mover_para_indice + obter_atual)"),
    3: ("caso3_remocao_inicio", "Caso 3: Remoção no início (remover_atual)"),
    4: ("caso4_remocao_meio", "Caso 4: Remoção no meio (remover_atual)"),
    5: ("caso5_percurso", "Caso 5: Percurso completo (avancar_cursor + obter_atual)"),
}


def ler_tempos(caminho):
    dados = {}
    with open(caminho, newline="") as f:
        for linha in csv.DictReader(f):
            impl = linha["implementacao"]
            caso = int(linha["caso"])
            n = int(linha["n"])
            R = int(linha["R"])
            tempo_ns = float(linha["tempo_ns"])
            chave = (impl, caso, n)
            dados.setdefault(chave, {"R": R, "tempos": []})
            dados[chave]["tempos"].append(tempo_ns)
    return dados


def resumir(dados):
    resumo = {}
    for (impl, caso, n), info in dados.items():
        tempos = info["tempos"]
        media = statistics.mean(tempos)
        desvio = statistics.stdev(tempos) if len(tempos) >= 2 else 0.0
        resumo[(impl, caso, n)] = {
            "R": info["R"],
            "media_ns": media,
            "desvio_ns": desvio,
            "min_ns": min(tempos),
            "max_ns": max(tempos),
        }
    return resumo


def escrever_resumo(resumo, caminho):
    with open(caminho, "w", newline="") as f:
        escritor = csv.writer(f)
        escritor.writerow(["implementacao", "caso", "n", "R", "media_ns", "desvio_ns", "min_ns", "max_ns"])
        for (impl, caso, n) in sorted(resumo.keys(), key=lambda k: (k[1], k[2], k[0])):
            r = resumo[(impl, caso, n)]
            escritor.writerow([impl, caso, n, r["R"], f"{r['media_ns']:.3f}", f"{r['desvio_ns']:.3f}",
                                f"{r['min_ns']:.3f}", f"{r['max_ns']:.3f}"])


def escolher_unidade(valor_max_ns):
    if valor_max_ns < 1e6:
        return 1e3, "µs"
    return 1e6, "ms"


def serie_do_caso(resumo, caso):
    series = {}
    for impl in ORDEM_IMPL:
        ns_vals = sorted({n for (i, c, n) in resumo if i == impl and c == caso})
        if not ns_vals:
            continue
        medias = [resumo[(impl, caso, n)]["media_ns"] for n in ns_vals]
        desvios = [resumo[(impl, caso, n)]["desvio_ns"] for n in ns_vals]
        series[impl] = (ns_vals, medias, desvios)
    return series


def desenhar_eixo(ax, series, titulo):
    maior = 0.0
    for impl, (ns_vals, medias, desvios) in series.items():
        for m, d in zip(medias, desvios):
            maior = max(maior, m + d)
    fator, unidade = escolher_unidade(maior if maior > 0 else 1.0)

    for impl in ORDEM_IMPL:
        if impl not in series:
            continue
        ns_vals, medias, desvios = series[impl]
        y = [m / fator for m in medias]
        yerr = [d / fator for d in desvios]
        ax.errorbar(ns_vals, y, yerr=yerr, label=impl, color=COR[impl],
                     marker="o", markersize=6, linewidth=2, capsize=3)

    ax.set_xscale("log")
    ax.set_yscale("log")
    ax.set_xlabel("Tamanho da lista (n)")
    ax.set_ylabel(f"Tempo médio do bloco ({unidade})")
    ax.set_title(titulo)
    ax.grid(True, which="both", linestyle="-", linewidth=0.5, color="#dddddd", alpha=0.7)
    ax.legend()


def gerar_graficos(resumo):
    os.makedirs(PASTA_GRAFICOS, exist_ok=True)

    for caso, (nome_arquivo, titulo) in CASOS.items():
        series = serie_do_caso(resumo, caso)
        if not series:
            continue
        fig, ax = plt.subplots(figsize=(7, 5))
        desenhar_eixo(ax, series, titulo)
        fig.tight_layout()
        fig.savefig(os.path.join(PASTA_GRAFICOS, f"{nome_arquivo}.png"), dpi=150)
        plt.close(fig)

    fig, axs = plt.subplots(2, 3, figsize=(18, 10))
    eixos = axs.flatten()
    for idx, (caso, (_, titulo)) in enumerate(CASOS.items()):
        series = serie_do_caso(resumo, caso)
        if not series:
            continue
        desenhar_eixo(eixos[idx], series, titulo)
    for sobra in range(len(CASOS), len(eixos)):
        eixos[sobra].axis("off")
    fig.tight_layout()
    fig.savefig(os.path.join(PASTA_GRAFICOS, "todos_os_casos.png"), dpi=150)
    plt.close(fig)


def main():
    dados = ler_tempos(CAMINHO_TEMPOS)
    resumo = resumir(dados)
    escrever_resumo(resumo, CAMINHO_RESUMO)
    gerar_graficos(resumo)
    print(f"resumo escrito em {CAMINHO_RESUMO}")
    print(f"graficos escritos em {PASTA_GRAFICOS}")


if __name__ == "__main__":
    main()
