#!/usr/bin/env python3
"""
Скрипт для построения графиков по результатам симуляции.
Читает result.csv (генерируется C++ программой) и сохраняет PNG.

Использование:
    python plot_results.py <csv_file> <output_png> "<title>"

Пример:
    python plot_results.py result.csv doc/screenshots/06_plot_model_1_5.png "Model 1.5 - Step, a1=0.4, a2=0.3, k=2"
"""

import sys
import csv
import matplotlib
matplotlib.use('Agg')  # без графического окна — только сохранение в файл
import matplotlib.pyplot as plt


def read_csv(path):
    """Читает result.csv с колонками tau, u_tau, y_tau."""
    taus, us, ys = [], [], []
    with open(path, 'r', encoding='utf-8') as f:
        reader = csv.DictReader(f)
        for row in reader:
            taus.append(int(row['tau']))
            us.append(float(row['u_tau']))
            ys.append(float(row['y_tau']))
    return taus, us, ys


def main():
    if len(sys.argv) < 4:
        print("Usage: python plot_results.py <csv_file> <output_png> \"<title>\"")
        sys.exit(1)

    csv_file = sys.argv[1]
    out_png  = sys.argv[2]
    title    = sys.argv[3]

    taus, us, ys = read_csv(csv_file)

    # --- График ---
    fig, ax = plt.subplots(figsize=(10, 6))

    # Основной график y(tau)
    ax.plot(taus, ys, marker='o', linewidth=2, markersize=6,
            color='#2E86AB', label='y (output)')

    # Вход u(tau) как вспомогательный пунктир
    ax.plot(taus, us, linestyle='--', linewidth=1.5,
            color='#EE6C4D', alpha=0.7, label='u (input)')

    # Оформление
    ax.set_xlabel('Simulation step tau', fontsize=12)
    ax.set_ylabel('Value', fontsize=12)
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