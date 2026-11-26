import csv
import matplotlib.pyplot as plt

TIME_LABEL = "Время"
time = []
setpoint = []
y = []
u = []

with open("results.csv", "r", encoding="utf-8") as file:

    reader = csv.DictReader(file, delimiter=";")

    for row in reader:
        time.append(float(row[TIME_LABEL]))
        setpoint.append(float(row["Заданная температура"]))
        y.append(float(row["Выход"]))
        u.append(float(row["Управляющий сигнал"]))


plt.figure()

plt.plot(time, setpoint, label="Заданная температура")
plt.plot(time, y, label="y(t)")

plt.xlabel(TIME_LABEL)
plt.ylabel("Температура")
plt.title("Переходный процесс системы управления")

plt.grid(True)
plt.legend()
plt.tight_layout()

plt.savefig("result.png", dpi=150)
plt.show()


plt.figure()

plt.plot(time, u, label="Управляющий сигнал")

plt.xlabel(TIME_LABEL)
plt.ylabel("Управление")
plt.title("Работа ПИД-регулятора")

plt.grid(True)
plt.legend()
plt.tight_layout()

plt.savefig("control.png", dpi=150)
plt.show()
