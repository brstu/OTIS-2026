# -*- coding: utf-8 -*-
import os
import sys
import csv
import matplotlib.pyplot as plt

# 1. Определяем папку, где лежит сам скрипт, и путь к файлу с расчетами
BASE_DIR = os.path.dirname(os.path.abspath(__file__))
csv_path = os.path.join(BASE_DIR, "result.csv")

if not os.path.exists(csv_path):
    print("[ERROR] Файл result.csv не найден!")
    sys.exit(1)

# 2. Читаем данные из таблицы result.csv в три списка
taus = []
us = []
ys = []

with open(csv_path, mode="r", encoding="utf-8") as f:
    reader = csv.DictReader(f)
    for row in reader:
        taus.append(int(row["tau"]))
        us.append(float(row["u_tau"]))
        ys.append(float(row["y_tau"]))

# 3. Рисуем график: красным пунктиром — вход u, синей линией — выход y
plt.figure(figsize=(9, 5))
plt.plot(taus, us, "r--o", label="Входной сигнал u(tau)", linewidth=1.5, markersize=4)
plt.plot(taus, ys, "b-s", label="Выход системы y(tau)", linewidth=2.0, markersize=4)

plt.title("Переходный процесс (ЛР №1, Вариант 7)", fontsize=12)
plt.xlabel("Шаг (tau)", fontsize=10)
plt.ylabel("Значение", fontsize=10)
plt.grid(True, linestyle=":", alpha=0.6)
plt.legend()
plt.tight_layout()

# 4. Сохраняем картинку и сразу открываем её на экране
save_path = os.path.join(BASE_DIR, "doc", "latest_plot.png")
plt.savefig(save_path, dpi=300)
plt.close()

print(f"График сохранен: {save_path}")
if hasattr(os, "startfile"):
    os.startfile(save_path)