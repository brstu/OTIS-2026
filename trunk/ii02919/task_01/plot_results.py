#!/usr/bin/env python3
"""
Скрипт визуализации результатов симуляции.

Читает файл result.csv из текущей рабочей директории
и сохраняет график в указанный PNG-файл.

Использование:
    python3 plot_results.py <output_png> "<title>"
"""

import sys
import csv
from pathlib import Path

import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt


INPUT_FILE = "result.csv"


def read_csv():
    """Читает INPUT_FILE из текущей директории."""
    csv_path = Path.cwd() / INPUT_FILE

    if not csv_path.is_file():
        print(f"[ERROR] File not found: {csv_path}")
        sys.exit(1)

    taus = []
    us = []
    ys = []
    with csv_path.open('r', encoding='utf-8') as f:
        reader = csv.DictReader(f)
        for row in reader:
            taus.append(int(row['tau']))
            us.append(float(row['u_tau']))
            ys.append(float(row['y_tau']))
    return taus, us, ys


def main():
    if len(sys.argv) < 3:
        print("Usage: python3 plot_results.py <output_png> \"<title>\"")
        sys.exit(1)

    out_png = sys.argv[1]
    title = sys.argv[2]

    cwd = Path.cwd().resolve()
    out_path = (cwd / out_png).resolve()

    if cwd not in out_path.parents and out_path != cwd:
        print(f"[ERROR] Output path escapes working directory: {out_png}")
        sys.exit(1)

    taus, us, ys = read_csv()

    fig, ax = plt.subplots(figsize=(10, 6))
    ax.plot(taus, ys, marker='o', linewidth=2, markersize=6,
            color='#1f77b4', label='y (выход)')
    ax.plot(taus, us, linestyle='--', linewidth=1.5,
            color='#ff7f0e', alpha=0.7, label='u (вход)')

    ax.set_xlabel('Шаг симуляции tau', fontsize=12)
    ax.set_ylabel('Значение', fontsize=12)
    ax.set_title(title, fontsize=13, fontweight='bold')
    ax.grid(True, linestyle=':', alpha=0.6)
    ax.axhline(y=0, color='black', linewidth=0.8, alpha=0.4)
    ax.legend(loc='best', fontsize=11)
    ax.set_xlim(min(taus) - 0.5, max(taus) + 0.5)

    plt.tight_layout()
    plt.savefig(out_path, dpi=150, bbox_inches='tight')
    plt.close(fig)

    print(f"[OK] Saved plot: {out_path}")


if __name__ == "__main__":
    main()