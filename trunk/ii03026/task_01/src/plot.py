import os
import matplotlib.pyplot as plt
import pandas as pd

# Имена файлов, которые генерирует C++ программа
files = [
    "linear_1_1_step.csv",
    "linear_1_1_pulse.csv",
    "linear_1_1_harmonic.csv",
]

plt.figure(figsize=(10, 8))

for i, file in enumerate(files, 1):
  plt.subplot(3, 1, i)
  if os.path.exists(file):
    df = pd.read_csv(file)
    plt.plot(df["Step"], df["Y"], marker="o", label=file)
    plt.legend()
    plt.grid(True)
  else:
    print(f"Файл {file} не найден.")

plt.tight_layout()
plt.show()