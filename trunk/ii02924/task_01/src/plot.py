import matplotlib.pyplot as plt
import csv
import sys

steps = []
u_vals = []
y_vals = []

try:
    with open('results.csv', 'r') as file:
        reader = csv.reader(file)
        next(reader) # Пропускаем заголовок (step,u,y)
        for row in reader:
            if len(row) == 3:
                steps.append(float(row[0]))
                u_vals.append(float(row[1]))
                y_vals.append(float(row[2]))
except FileNotFoundError:
    print("Ошибка: файл results.csv не найден.")
    sys.exit(1)

plt.figure(figsize=(10, 6))
plt.plot(steps, u_vals, label='Входной сигнал u(t)', linestyle='--', marker='o', color='blue')
plt.plot(steps, y_vals, label='Выходной сигнал y(t)', linewidth=2, marker='s', color='red')

plt.title('Результаты симуляции модели')
plt.xlabel('Шаг симуляции (t)')
plt.ylabel('Значение')
plt.legend()
plt.grid(True)

# Эта команда автоматически открывает окно с графиком на экране
plt.show()