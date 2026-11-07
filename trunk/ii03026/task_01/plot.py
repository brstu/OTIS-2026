import pandas as pd
import matplotlib.pyplot as plt

models = ['linear_1_1', 'nonlinear_2_3', 'diff_3_5']
responses = ['step', 'pulse', 'harmonic']

for model in models:
    plt.figure(figsize=(10, 5))
    
    for resp in responses:
        filename = f"{model}_{resp}.csv"
        try:
            # Читаем CSV файл
            df = pd.read_csv(filename)
            # Берём первый столбец для X (шаг/время) и второй для Y (значение)
            plt.plot(df.iloc[:, 0], df.iloc[:, 1], label=f"{resp} response", linewidth=2)
        except FileNotFoundError:
            print(f"Файл {filename} не найден.")
            
    plt.title(f"Графики откликов для модели: {model}")
    plt.xlabel("Шаг (Step)")
    plt.ylabel("Значение Y")
    plt.legend()
    plt.grid(True)
    
    # Сохраняем готовую картинку в папку проекта
    plt.savefig(f"{model}_plot.png", dpi=300)
    plt.show()