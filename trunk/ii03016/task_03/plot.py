import csv
import os
from datetime import datetime
import matplotlib.pyplot as plt

os.makedirs("doc/images", exist_ok=True)
timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
output_name = f"doc/images/plot_{timestamp}.png"

steps, setpoints, u_vals, y_vals = [], [], [], []

with open('simulation_results.csv', 'r', encoding='utf-8') as f:
    reader = csv.DictReader(f)
    for row in reader:
        steps.append(int(row['Step']))
        setpoints.append(float(row['Setpoint']))
        u_vals.append(float(row['U']))
        y_vals.append(float(row['Y']))

fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 7), sharex=True)

ax1.plot(steps, setpoints, 'g--', linewidth=2, label='Уставка w(k)')
ax1.plot(steps, y_vals, 'b-', linewidth=2, label='Выход объекта y(k)')
ax1.set_ylabel('Температура / Выход (Y)')
ax1.set_title('Переходный процесс САУ с ПИД-регулятором')
ax1.legend()
ax1.grid(True)

ax2.plot(steps, u_vals, 'r-', linewidth=1.5, label='Управление u(k)')
ax2.set_xlabel('Шаг симуляции (k)')
ax2.set_ylabel('Управление (U)')
ax2.legend()
ax2.grid(True)

plt.tight_layout()
plt.savefig(output_name, dpi=150)
print(f"\n[OK] График автоматически сохранён в: {output_name}")
