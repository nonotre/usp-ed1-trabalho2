import os
import re
import subprocess
import sys
import tempfile

import markdown

RAIZ = os.path.dirname(os.path.abspath(__file__))
MD = os.path.join(RAIZ, "relatorio.md")
PDF = os.path.join(RAIZ, "relatorio.pdf")

CSS = """
@page { size: A4; margin: 2.2cm 2cm; @bottom-center { content: counter(page); font-family: 'DejaVu Serif', Georgia, serif; font-size: 9pt; color: #444; } }
@page :first { @bottom-center { content: none; } }
body { font-family: 'DejaVu Serif', Georgia, serif; font-size: 10.5pt; line-height: 1.45; color: #111; }
h2 { font-size: 14pt; margin-top: 18pt; border-bottom: 1px solid #999; padding-bottom: 2pt; }
h3 { font-size: 12pt; margin-top: 12pt; }
h3, h4 { break-after: avoid; }
h4 { font-size: 10.5pt; margin: 12pt 0 4pt; }
table { border-collapse: collapse; margin: 6pt 0 10pt; font-size: 9.5pt; page-break-inside: avoid; }
th, td { border: 1px solid #888; padding: 3pt 6pt; vertical-align: top; }
th { background: #eee; }
code { font-family: 'DejaVu Sans Mono', monospace; font-size: 9pt; }
img { width: 100%; page-break-inside: avoid; }
.capa { height: 25cm; display: flex; flex-direction: column; justify-content: space-between; text-align: center; break-after: page; }
.capa .inst { font-size: 12pt; line-height: 1.6; }
.capa .titulo { font-size: 22pt; font-weight: bold; line-height: 1.3; }
.capa .sub { font-size: 13pt; margin-top: 10pt; }
.capa .nomes { font-size: 11.5pt; line-height: 1.8; }
.capa .local { font-size: 11pt; }
.sumario { break-after: page; }
.sumario h2 { border: 0; text-align: center; }
.sumario ol { list-style: none; padding: 0; margin: 0; }
.sumario li { display: flex; align-items: baseline; margin: 5pt 0; }
.sumario li.n3 { padding-left: 18pt; font-size: 10pt; }
.sumario li.n2 { font-weight: bold; margin-top: 10pt; }
.sumario .pontos { flex: 1; border-bottom: 1px dotted #777; margin: 0 4pt; }
"""


def ler_relatorio():
    texto = open(MD, encoding="utf-8").read()
    cabecalho, corpo = texto.split("\n---\n", 1)
    titulo = re.search(r"^# (.+)$", cabecalho, re.M).group(1)
    nomes = re.findall(r"^- (.+)$", cabecalho, re.M)
    return titulo, nomes, corpo


def secoes(corpo):
    return [(len(m.group(1)), m.group(2).strip()) for m in re.finditer(r"^(##|###) (.+)$", corpo, re.M)]


def html_capa(titulo, nomes):
    trabalho, tema = titulo.split(": ", 1)
    linhas = "<br>".join(nomes)
    return f"""<div class="capa">
<div class="inst">Universidade de São Paulo<br>Instituto de Ciências Matemáticas e de Computação<br>SCC0502 Algoritmos e Estruturas de Dados I</div>
<div><div class="titulo">{tema}</div><div class="sub">{trabalho}</div></div>
<div class="nomes">{linhas}</div>
<div class="local">São Carlos<br>Outubro de 2026</div>
</div>"""


def html_sumario(itens, paginas):
    linhas = []
    for (nivel, texto), pagina in zip(itens, paginas):
        linhas.append(f'<li class="n{nivel}"><span>{texto}</span><span class="pontos"></span><span>{pagina}</span></li>')
    return '<div class="sumario"><h2>Sumário</h2><ol>' + "".join(linhas) + "</ol></div>"


def gerar(html, destino):
    with tempfile.NamedTemporaryFile("w", suffix=".html", dir=RAIZ, delete=False, encoding="utf-8") as f:
        f.write(html)
        caminho = f.name
    try:
        subprocess.run(["chromium", "--headless", "--disable-gpu", "--no-pdf-header-footer",
                        f"--print-to-pdf={destino}", f"file://{caminho}"],
                       check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    finally:
        os.remove(caminho)


def localizar(itens, destino):
    paginas = []
    total = int(re.search(r"Pages:\s+(\d+)", subprocess.run(["pdfinfo", destino], capture_output=True, text=True).stdout).group(1))
    textos = [subprocess.run(["pdftotext", "-f", str(p), "-l", str(p), "-layout", destino, "-"],
                             capture_output=True, text=True).stdout for p in range(1, total + 1)]
    for _, texto in itens:
        alvo = re.sub(r"`", "", texto)
        achou = next((p for p in range(3, total + 1) if any(l.strip() == alvo for l in textos[p - 1].splitlines())), None)
        if achou is None:
            sys.exit(f"secao nao encontrada no PDF: {alvo}")
        paginas.append(achou)
    return paginas


def montar(capa, sumario, corpo_html):
    return f"<!DOCTYPE html><html lang='pt-BR'><head><meta charset='utf-8'><style>{CSS}</style></head><body>{capa}{sumario}{corpo_html}</body></html>"


titulo, nomes, corpo = ler_relatorio()
itens = secoes(corpo)
corpo_html = markdown.markdown(corpo, extensions=["tables", "fenced_code"])
capa = html_capa(titulo, nomes)

gerar(montar(capa, html_sumario(itens, ["00"] * len(itens)), corpo_html), PDF)
paginas = localizar(itens, PDF)
gerar(montar(capa, html_sumario(itens, paginas), corpo_html), PDF)
if localizar(itens, PDF) != paginas:
    sys.exit("paginas do sumario mudaram entre as passagens")
print(f"{PDF}: secoes nas paginas {paginas}")
