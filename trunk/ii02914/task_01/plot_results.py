#!/usr/bin/env python3
import sys
import csv
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt

def read_csv(path):
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
        print('Usage: python plot_results.py <csv_file> <output_png> "<title>"')
        sys.exit(1)

    csv_file = sys.argv[1]
    out_png  = sys.argv[2]
    title    = sys.argv[3]

    taus, us, ys = read_csv(csv_file)

    fig, ax = plt.subplots(figsize=(8, 4.5))

    ax.plot(taus, ys, marker='o', linewidth=2, markersize=5,
            color='#1f77b4', label='y (выход)')
    ax.plot(taus, us, linestyle='--', linewidth=1.5,
            color='#ff7f0e', alpha=0.7, label='u (вход)')

    ax.set_xlabel('Шаг симуляции τ', fontsize=11)
    ax.set_ylabel('Значение', fontsize=11)
    ax.set_title(title, fontsize=12, fontweight='bold')
    ax.grid(True, linestyle=':', alpha=0.6)
    ax.axhline(y=0, color='black', linewidth=0.8, alpha=0.4)
    ax.legend(loc='best', fontsize=10)
    ax.set_xlim(min(taus) - 0.5, max(taus) + 0.5)

    plt.tight_layout()
    plt.savefig(out_png, dpi=130, bbox_inches='tight')
    plt.close(fig)

    print(f"[OK] Saved plot: {out_png}")

if __name__ == "__main__":
    main()