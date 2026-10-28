"""Графики по результатам симуляции.

Сетка: строки = выбранные воздействия, колонки = выбранные модели.
Что именно выбрано, берётся из plot_inputs.txt (пишет main.cpp),
а если его нет — по фактически существующим results_*.csv.

Запуск:  python plot.py [каталог_с_csv]
"""
from __future__ import annotations

import sys
from pathlib import Path

import numpy as np
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

CURRENT_DIR = Path(__file__).parent.resolve()
OUT_DIR = (Path(sys.argv[1]).resolve() if len(sys.argv) > 1
           else (CURRENT_DIR.parent / "doc").resolve())

ALLOWED_INPUTS = ("step", "impulse", "harmonic")
RU_TITLES = {"step": "ступенька", "impulse": "импульс", "harmonic": "синусоида"}


def csv_path(name: str) -> Path:
    """Путь к results_<name>.csv с защитой от выхода за пределы OUT_DIR."""
    path = (OUT_DIR / f"results_{name}.csv").resolve()
    if path.parent != OUT_DIR:
        raise PermissionError("Попытка несанкционированного обхода директории!")
    return path


def selected_inputs() -> list[str]:
    """Какие воздействия строим (в порядке step -> impulse -> harmonic)."""
    manifest = OUT_DIR / "plot_inputs.txt"
    names: list[str] = []
    if manifest.is_file():
        raw = manifest.read_text(encoding="utf-8").splitlines()
        names = [ln.strip() for ln in raw if ln.strip() in ALLOWED_INPUTS]
    if not names:  # fallback: автопоиск
        names = [n for n in ALLOWED_INPUTS if (OUT_DIR / f"results_{n}.csv").is_file()]
    # единый порядок строк + только существующие файлы
    ordered = [n for n in ALLOWED_INPUTS if n in names]
    return [n for n in ordered if csv_path(n).is_file()]


def load(name: str):
    path = csv_path(name)
    with open(path, encoding="utf-8") as f:
        header = f.readline().strip().split(",")
    titles = header[2:]
    data = np.loadtxt(path, delimiter=",", skiprows=1, ndmin=2)
    # защита от inf/nan, если нелинейная модель разошлась
    data = np.where(np.isfinite(data), data, np.nan)
    return titles, data


def main() -> int:
    OUT_DIR.mkdir(parents=True, exist_ok=True)
    names = selected_inputs()
    if not names:
        print(f"В каталоге {OUT_DIR} нет results_*.csv — сначала запустите программу.")
        return 1

    loaded = [(n, *load(n)) for n in names]           # (name, titles, data)
    n_models = max(len(t) for _, t, _ in loaded)
    n_rows = len(loaded)

    print(f"Каталог: {OUT_DIR}")
    print(f"Воздействия ({n_rows}): {', '.join(names)}")
    print(f"Моделей в колонках: {n_models}")

    fig, axes = plt.subplots(
        n_rows, n_models,
        figsize=(4.5 * n_models + 1, 3.0 * n_rows + 0.6),
        sharex=True, squeeze=False,
    )

    for r, (name, titles, data) in enumerate(loaded):
        tau, u = data[:, 0], data[:, 1]
        for c in range(n_models):
            ax = axes[r][c]
            if c >= len(titles) or data.shape[1] <= 2 + c:
                ax.axis("off")          # в этой строке моделей меньше
                continue
            ax.step(tau, u, "--", where="post", color="gray",
                    alpha=0.85, lw=1.0, label="u")
            ax.plot(tau, data[:, 2 + c], "o-", color="tab:blue",
                    ms=3.5, lw=1.3, label="y")
            ax.grid(True, alpha=0.4)
            if r == 0:
                ax.set_title(titles[c])
            if c == 0:
                ax.set_ylabel(f"{name}\n({RU_TITLES.get(name, name)})", fontsize=9)
            if r == n_rows - 1:
                ax.set_xlabel("tau")

    axes[0][0].legend(loc="best", fontsize=8)
    fig.suptitle(f"Переходные процессы: {', '.join(names)}", y=0.995, fontsize=11)
    fig.tight_layout(rect=(0, 0, 1, 0.975))

    plot_path = OUT_DIR / "plot.png"
    fig.savefig(plot_path, dpi=110)
    plt.close(fig)
    print("График сохранён в", plot_path)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())