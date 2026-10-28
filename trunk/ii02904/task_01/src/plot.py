"""Читает results.csv (его создаёт программа на C++) и показывает график."""

import csv

import matplotlib.pyplot as plt

FILE_NAME = "results.csv"
PICTURE_NAME = "results.png"


def main():
    """Читает файл с результатами и рисует график."""
    # В первой строке файла лежит заголовок графика, дальше таблица tau,u,y
    with open(FILE_NAME, encoding="utf-8") as file:
        title = file.readline().strip()
        rows = list(csv.DictReader(file))

    tau = [int(row["tau"]) for row in rows]
    u = [float(row["u"]) for row in rows]
    y = [float(row["y"]) for row in rows]

    plt.figure(figsize=(10, 6))
    plt.plot(tau, y, "o-", label="y (выход)")
    plt.plot(tau, u, "--", label="u (вход)")
    plt.title(title)
    plt.xlabel("Шаг tau")
    plt.grid(True)
    plt.legend()

    plt.savefig(PICTURE_NAME, dpi=150)  # заодно сохраняем картинку в файл
    plt.show()  # и сразу открываем окно с графиком


if __name__ == "__main__":
    main()
