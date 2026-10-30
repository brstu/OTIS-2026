import csv
import sys

import matplotlib.pyplot as plt

# Файл с результатами, который создаёт программа на C++
filename = "result.csv"

steps = []
inputs = []
outputs = []

try:
    with open(filename, mode="r", encoding="utf-8") as file:
        for row in csv.DictReader(file):
            steps.append(int(row["Step"]))
            inputs.append(float(row["Input"]))
            outputs.append(float(row["Output"]))
except FileNotFoundError:
    print(f"[Ошибка] Файл {filename} не найден! Сначала запустите программу на C++.")
    sys.exit(1)

plt.figure(figsize=(10, 6))
plt.plot(steps, inputs, label="u(τ) - вход", color="orange", linestyle="--", linewidth=1.5)
plt.plot(steps, outputs, label="y(τ) - выход", color="blue", linewidth=2)
plt.title("Результаты моделирования (вариант 15)")
plt.xlabel("Шаг τ")
plt.ylabel("Значение")
plt.axhline(0, color="black", linewidth=0.8)
plt.grid(True, linestyle=":", alpha=0.7)
plt.legend()

plt.savefig("simulation_plot.png", dpi=150)
print("График сохранён: simulation_plot.png")
plt.show()
