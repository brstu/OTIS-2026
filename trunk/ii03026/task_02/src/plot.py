import os
import matplotlib.pyplot as plt
import pandas as pd
import glob

# Автоматически находим все CSV файлы, сгенерированные симуляцией
files = sorted(glob.glob("*_step.csv") + glob.glob("*_pulse.csv") + glob.glob("*_harmonic.csv"))

if not files:
    print("CSV файлы не найдены. Сначала запустите симуляцию.")
    exit(1)

# Группируем по префиксу (имя модели)
model_files = {}
for f in files:
    # Извлекаем префикс до _step/_pulse/_harmonic
    prefix = f.rsplit('_', 1)[0]
    if prefix not in model_files:
        model_files[prefix] = []
    model_files[prefix].append(f)

# Строим графики для каждой модели
for model_name, model_file_list in model_files.items():
    plt.figure(figsize=(10, 8))
    plt.suptitle(f"Модель: {model_name}", fontsize=14)

    for i, file in enumerate(sorted(model_file_list), 1):
        if os.path.exists(file):
            df = pd.read_csv(file)
            plt.subplot(3, 1, i)
            plt.plot(df["Step"], df["Y"], marker="o", markersize=2, label=file)
            plt.legend()
            plt.grid(True)
            plt.ylabel("Y")
        else:
            print(f"Файл {file} не найден.")

    plt.xlabel("Step")
    plt.tight_layout()
    plt.savefig(f"{model_name}_plots.png", dpi=150)
    plt.close()
    print(f"График сохранён: {model_name}_plots.png")

print("Все графики успешно созданы!")