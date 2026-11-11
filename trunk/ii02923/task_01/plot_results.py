#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Построение графиков переходных процессов для ЛР №1 (вариант 11).

Скрипт сам запускает собранную программу otis_lab1_ii02911.exe для каждой
комбинации "модель × входное воздействие", читает текстовые таблицы
(табуляция, без CSV) и сохраняет графики в doc/screenshots.

Запуск:  python plot_results.py
"""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt  # noqa: E402

# Чтобы русские сообщения нормально печатались в консоли Windows.
if hasattr(sys.stdout, "reconfigure"):
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")

ROOT = Path(__file__).resolve().parent
EXE_NAME = "otis_lab1_ii02911.exe" if sys.platform.startswith("win") else "otis_lab1_ii02911"
OUT_DIR = ROOT / "doc" / "screenshots"
DATA_DIR = ROOT / "build" / "plot_data"

MODELS = [
    {
        "id": "1.1",
        "title": "Model 1.1 — линейная инерционная система",
        "params": {"a": 0.8, "b": 1.0},
        "steps": 40,
    },
    {
        "id": "2.5",
        "title": "Model 2.5 — нелинейное «сухое трение»",
        "params": {"a": 0.85, "b": 1.2},
        "steps": 40,
    },
    {
        "id": "3.9",
        "title": "Model 3.9 — dy/dt = b·tanh(u), метод Эйлера",
        "params": {"b": 1.0},
        "steps": 80,
    },
]

SIGNALS = [
    ("step", "Ступенчатое воздействие"),
    ("impulse", "Импульсное воздействие"),
    ("harmonic", "Гармоническое воздействие"),
]

plt.rcParams.update(
    {
        "font.family": "DejaVu Sans",
        "axes.unicode_minus": False,
        "figure.dpi": 110,
    }
)


def find_executable() -> Path:
    candidates = [
        ROOT / "build" / "bin" / EXE_NAME,
        ROOT / "build" / "bin" / "Release" / EXE_NAME,
        ROOT / "build" / "bin" / "Debug" / EXE_NAME,
        ROOT / "build" / "Release" / EXE_NAME,
        ROOT / "build" / EXE_NAME,
    ]
    for candidate in candidates:
        if candidate.is_file():
            return candidate
    raise FileNotFoundError(
        "Не найден исполняемый файл. Сначала запустите build.bat "
        "(или: cmake -S . -B build && cmake --build build --config Release)."
    )


def run_model(exe: Path, model: dict, signal: str) -> list[tuple[int, float, float]]:
    DATA_DIR.mkdir(parents=True, exist_ok=True)
    out_file = DATA_DIR / f"model_{model['id'].replace('.', '_')}_{signal}.txt"

    cmd = [
        str(exe),
        "--model",
        model["id"],
        "--signal",
        signal,
        "--steps",
        str(model["steps"]),
        "--dt",
        "0.1",
        "--out",
        str(out_file),
    ]
    for key, value in model["params"].items():
        cmd.extend([f"--{key}", str(value)])

    result = subprocess.run(cmd, capture_output=True, text=True, encoding="utf-8")
    if result.returncode != 0:
        raise RuntimeError(
            f"Программа завершилась с ошибкой для модели {model['id']} / {signal}:\n"
            f"{result.stdout}\n{result.stderr}"
        )

    rows: list[tuple[int, float, float]] = []
    for line in out_file.read_text(encoding="utf-8").splitlines()[1:]:
        tau, u, y = line.split("\t")
        rows.append((int(tau), float(u), float(y)))
    return rows


def plot_model(exe: Path, model: dict) -> Path:
    fig, axes = plt.subplots(1, 3, figsize=(14.5, 4.2))

    for ax, (signal_id, signal_title) in zip(axes, SIGNALS):
        data = run_model(exe, model, signal_id)
        tau = [row[0] for row in data]
        u = [row[1] for row in data]
        y = [row[2] for row in data]

        ax.plot(tau, y, color="#1f4e9c", linewidth=2.2, label="y(t)")
        ax.set_title(signal_title, fontsize=11)
        ax.set_xlabel("τ, номер шага")
        ax.set_ylabel("y(τ)", color="#1f4e9c")
        ax.tick_params(axis="y", labelcolor="#1f4e9c")
        ax.grid(True, alpha=0.3, linestyle=":")

        ax2 = ax.twinx()
        ax2.plot(
            tau, u, color="#d9822b", linewidth=1.3, linestyle="--", alpha=0.85, label="u(t)"
        )
        ax2.set_ylabel("u(τ)", color="#d9822b")
        ax2.tick_params(axis="y", labelcolor="#d9822b")
        ax2.set_ylim(-1.35, 1.35)

        params = ", ".join(f"{k} = {v}" for k, v in model["params"].items())
        ax.text(
            0.02,
            0.97,
            f"{model['id']}: {params}",
            transform=ax.transAxes,
            va="top",
            fontsize=8.5,
            bbox={"boxstyle": "round,pad=0.25", "facecolor": "white", "alpha": 0.75, "edgecolor": "#cccccc"},
        )
        # Пояснение к линиям — в подписи осей (y — синяя, u — оранжевая).

    fig.suptitle(model["title"], fontsize=13, fontweight="bold")
    fig.tight_layout(rect=(0, 0, 1, 0.94))

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    out_path = OUT_DIR / f"plot_model_{model['id'].replace('.', '_')}.png"
    fig.savefig(out_path, bbox_inches="tight")
    plt.close(fig)
    return out_path


def main() -> int:
    exe = find_executable()
    print(f"Исполняемый файл: {exe}")
    for model in MODELS:
        path = plot_model(exe, model)
        print(f"  сохранён график: {path}")
    print("Готово.")
    return 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except Exception as exc:  # noqa: BLE001
        print(f"Ошибка: {exc}", file=sys.stderr)
        sys.exit(1)
