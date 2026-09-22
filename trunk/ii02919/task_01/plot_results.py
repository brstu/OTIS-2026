#!/usr/bin/env python3
"""
Скрипт визуализации результатов симуляции.
Читает result.csv и сохраняет график в PNG.

Использование:
    python3 plot_results.py <csv_file> <output_png> "<title>"
"""

import sys
import csv
import matplotlib
import os
matplotlib.use('Agg')  # без графического окна
import matplotlib.pyplot as plt


def read_csv(path):
    # Защита от path traversal: разрешаем только относительные пути,
    # не выходящие за пределы текущей рабочей директории.
    if os.path.isabs(path) or '..' in path.split(os.sep):
        raise ValueError(f"Unsafe path detected: {path}")

    # Дополнительно: убеждаемся, что итоговый путь внутри CWD.
    safe_path = os.path.realpath(path)
    cwd = os.path.realpath(os.getcwd())
    if not safe_path.startswith(cwd + os.sep) and safe_path != cwd:
        raise ValueError(f"Path escapes working directory: {path}")

    taus, us, ys = [], [], []
    with open(safe_path, 'r', encoding='utf-8') as f:
        reader = csv.DictReader(f)
        for row in reader:
            taus.append(int(row['tau']))
            us.append(float(row['u_tau']))
            ys.append(float(row['y_tau']))
    return taus, us, ys 


def main():
    if len(sys.argv) < 4:
        print("Usage: python3 plot_results.py <csv> <png> \"<title>\"")
        sys.exit(1)

    csv_file = sys.argv[1]
    out_png  = sys.argv[2]
    title    = sys.argv[3]

    taus, us, ys = read_csv(csv_file)

    fig, ax = plt.subplots(figsize=(10, 6))
    ax.plot(taus, ys, marker='o', linewidth=2, markersize=6,
            color='#1f77b4', label='y (выход)')
    ax.plot(taus, us, linestyle='--', linewidth=1.5,
            color='#ff7f0e', alpha=0.7, label='u (вход)')

    ax.set_xlabel('Шаг симуляции τ', fontsize=12)
    ax.set_ylabel('Значение', fontsize=12)
    ax.set_title(title, fontsize=13, fontweight='bold')
    ax.grid(True, linestyle=':', alpha=0.6)
    ax.axhline(y=0, color='black', linewidth=0.8, alpha=0.4)
    ax.legend(loc='best', fontsize=11)
    ax.set_xlim(min(taus) - 0.5, max(taus) + 0.5)

    plt.tight_layout()
    plt.savefig(out_png, dpi=150, bbox_inches='tight')
    plt.close(fig)

    print(f"[OK] Saved plot: {out_png}")


if __name__ == "__main__":
    main()
