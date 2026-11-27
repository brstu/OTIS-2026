"""Графики по pid_result.csv: python plot.py -> graph.png."""

import csv

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt


def load_run(path="pid_result.csv"):
    data = {"t": [], "w": [], "u": [], "y": []}
    with open(path, encoding="utf-8") as stream:
        for row in csv.DictReader(stream, delimiter=";"):
            if not row.get("t"):
                continue
            for key in data:
                data[key].append(float(row[key]))
    return data


def main():
    run = load_run()

    figure, (top, bottom) = plt.subplots(2, 1, figsize=(9, 7), sharex=True)

    top.plot(run["t"], run["y"], color="tab:blue", marker="o", markersize=3,
             label="выход объекта y(t)")
    top.plot(run["t"], run["w"], color="tab:red", linestyle="--",
             label="задание w")
    top.set_ylabel("регулируемая величина")
    top.set_title("Переходный процесс в системе с ПИД-регулятором (вариант 23)")
    top.grid(alpha=0.4)
    top.legend()

    bottom.plot(run["t"], run["u"], color="tab:green", label="управление u(t)")
    bottom.set_xlabel("номер такта t")
    bottom.set_ylabel("управление")
    bottom.grid(alpha=0.4)
    bottom.legend()

    figure.tight_layout()
    figure.savefig("graph.png", dpi=150)
    print("готово: graph.png")


if __name__ == "__main__":
    main()