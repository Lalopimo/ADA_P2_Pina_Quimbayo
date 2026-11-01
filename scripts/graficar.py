#!/usr/bin/env python3
"""Gráficas y exponentes ajustados a partir de los CSV de medicion.cpp.

Uso:
    python3 scripts/graficar.py docs/macos_corrida1 docs/macos_corrida2 --prefijo docs/macos

Cada argumento es una carpeta con archivos resultados_*_u<UMBRAL>.csv. Si se dan varias
carpetas (varias corridas), para cada (umbral, n) se usa la MEDIANA entre corridas.
Genera <prefijo>_tiempo.png, <prefijo>_razon.png y <prefijo>_tabla.tex (tabla LaTeX de tiempos),
e imprime los exponentes ajustados y los puntos de cruce.
Requiere matplotlib y numpy (pip install matplotlib).
"""
import argparse
import csv
import glob
import os
import statistics
from collections import defaultdict

import matplotlib
matplotlib.use("Agg")                      # sin ventana: solo guarda archivos
import matplotlib.pyplot as plt
import numpy as np


def leer(carpetas):
    """datos[umbral][n] = ([tiempos clasica], [tiempos karatsuba]) sobre las corridas."""
    datos = defaultdict(lambda: defaultdict(lambda: ([], [])))
    for carpeta in carpetas:
        archivos = sorted(glob.glob(os.path.join(carpeta, "resultados_*.csv")))
        if not archivos:
            raise SystemExit(f"No hay resultados_*.csv en {carpeta}")
        for ruta in archivos:
            with open(ruta, newline="") as f:
                for fila in csv.DictReader(f):
                    u, n = int(fila["umbral"]), int(fila["n"])
                    datos[u][n][0].append(float(fila["clasica_us_mediana"]))
                    datos[u][n][1].append(float(fila["karatsuba_us_mediana"]))
    return datos


def serie(datos, u):
    ns = sorted(datos[u])
    c = [statistics.median(datos[u][n][0]) for n in ns]
    k = [statistics.median(datos[u][n][1]) for n in ns]
    return np.array(ns, float), np.array(c), np.array(k)


def exponente(ns, t, n_min):
    """Pendiente de log2(t) contra log2(n) para n >= n_min (t ~ n^p)."""
    m = ns >= n_min
    p, _ = np.polyfit(np.log2(ns[m]), np.log2(t[m]), 1)
    return p


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("carpetas", nargs="+")
    ap.add_argument("--prefijo", default="docs/grafica")
    ap.add_argument("--n_min_ajuste", type=int, default=128)
    a = ap.parse_args()

    datos = leer(a.carpetas)
    umbrales = sorted(datos)
    colores = plt.cm.viridis(np.linspace(0.05, 0.9, len(umbrales)))

    # ---- Gráfica 1: tiempo contra tamaño (log-log)
    fig, ax = plt.subplots(figsize=(8, 5.5))
    ns, c, _ = serie(datos, umbrales[0])
    ax.plot(ns, c, "o-", color="black", lw=2, label="Clásica")
    for u, col in zip(umbrales, colores):
        ns, _, k = serie(datos, u)
        etiqueta = "Karatsuba puro (UMBRAL=1)" if u == 1 else f"Karatsuba híbrido (UMBRAL={u})"
        ax.plot(ns, k, "s--", color=col, label=etiqueta)
    # rectas de referencia ancladas en n = 64
    ns, c, _ = serie(datos, umbrales[0])
    i = int(np.where(ns == 64)[0][0]) if 64 in ns else 0
    xs = np.array([ns[i], ns[-1]])
    ax.plot(xs, c[i] * (xs / ns[i]) ** 2, ":", color="gray", lw=1)
    ax.text(xs[-1], c[i] * (xs[-1] / ns[i]) ** 2, " $n^2$", color="gray", va="bottom")
    if 1 in datos:
        ns1, _, k1 = serie(datos, 1)
        j = int(np.where(ns1 == 64)[0][0]) if 64 in ns1 else 0
        xs = np.array([ns1[j], ns1[-1]])
        ax.plot(xs, k1[j] * (xs / ns1[j]) ** np.log2(3), ":", color="gray", lw=1)
        ax.text(xs[-1], k1[j] * (xs[-1] / ns1[j]) ** np.log2(3), r" $n^{\log_2 3}$", color="gray", va="bottom")
    ax.set_xscale("log", base=2); ax.set_yscale("log")
    ax.set_xlabel("Tamaño n (dígitos)"); ax.set_ylabel("Tiempo por multiplicación (µs)")
    ax.set_title("Tiempo de ejecución: clásica vs Karatsuba")
    ax.grid(True, which="both", alpha=0.3); ax.legend(fontsize=8)
    fig.tight_layout(); fig.savefig(a.prefijo + "_tiempo.png", dpi=200); plt.close(fig)

    # ---- Gráfica 2: razón clásica/Karatsuba (>1 => Karatsuba gana)
    fig, ax = plt.subplots(figsize=(8, 5.5))
    for u, col in zip(umbrales, colores):
        ns, c, k = serie(datos, u)
        etiqueta = "UMBRAL=1 (puro)" if u == 1 else f"UMBRAL={u}"
        ax.plot(ns, c / k, "o-", color=col, label=etiqueta)
    ax.axhline(1.0, color="red", lw=1.2)
    ax.text(ns[0], 1.05, "Karatsuba más rápido ↑", color="red", fontsize=8)
    ax.set_xscale("log", base=2); ax.set_yscale("log")
    ax.set_xlabel("Tamaño n (dígitos)"); ax.set_ylabel("Tiempo clásica / tiempo Karatsuba")
    ax.set_title("Punto de cruce: la curva cruza la línea roja (razón = 1)")
    ax.grid(True, which="both", alpha=0.3); ax.legend(fontsize=8)
    fig.tight_layout(); fig.savefig(a.prefijo + "_razon.png", dpi=200); plt.close(fig)

    # ---- Exponentes ajustados
    print(f"\nExponente p ajustado (T ~ n^p), usando n >= {a.n_min_ajuste}   [log2(3) = {np.log2(3):.3f}]")
    print(f"{'UMBRAL':>7} {'clasica p':>10} {'karatsuba p':>12} {'x por duplicar (K)':>19}")
    for u in umbrales:
        ns, c, k = serie(datos, u)
        pk = exponente(ns, k, a.n_min_ajuste)
        print(f"{u:>7} {exponente(ns, c, a.n_min_ajuste):>10.3f} {pk:>12.3f} {2**pk:>19.2f}")
    # ---- Punto de cruce (solo cuenta n > UMBRAL: para n <= UMBRAL Karatsuba ejecuta el mismo caso base)
    print("\nPunto de cruce: menor n > UMBRAL con razón > 1 desde ahí hasta el final")
    print(f"{'UMBRAL':>7} {'cruce n':>9} {'razón en n':>11} {'razón en n_max':>15}  nota")
    for u in umbrales:
        ns, c, k = serie(datos, u)
        r = c / k
        idx = None
        for i in range(len(ns) - 1, -1, -1):
            if ns[i] <= u or r[i] <= 1.0:
                break
            idx = i
        if idx is None:
            print(f"{u:>7} {'--':>9} {'--':>11} {r[-1]:>15.3f}  no aparece en el rango medido")
        else:
            nota = "empate (<5%): no es un cruce firme" if r[idx] < 1.05 else ""
            print(f"{u:>7} {int(ns[idx]):>9} {r[idx]:>11.3f} {r[-1]:>15.3f}  {nota}")

    # ---- Tabla LaTeX con las medianas entre corridas (para \input en el informe)
    def fmt(v):
        if v < 1:
            return f"{v:.3g}"
        if v < 10:
            return f"{v:.2f}"
        if v < 100:
            return f"{v:.1f}"
        return f"{v:,.0f}".replace(",", r"\,")
    ns, c, _ = serie(datos, umbrales[0])
    ks = {u: serie(datos, u)[2] for u in umbrales}
    lineas = [r"\begin{tabular}{" + "r" * (2 + len(umbrales)) + "}", r"\toprule",
              "$n$ & Clásica & " + " & ".join(f"K$_{{{u}}}$" for u in umbrales) + r" \\", r"\midrule"]
    for i, n in enumerate(ns):
        fila = [str(int(n)), fmt(c[i])] + [fmt(ks[u][i]) for u in umbrales]
        lineas.append(" & ".join(fila) + r" \\")
    lineas += [r"\bottomrule", r"\end{tabular}"]
    with open(a.prefijo + "_tabla.tex", "w") as f:
        f.write("\n".join(lineas) + "\n")

    print(f"\nGuardado: {a.prefijo}_tiempo.png, {a.prefijo}_razon.png y {a.prefijo}_tabla.tex")


if __name__ == "__main__":
    main()