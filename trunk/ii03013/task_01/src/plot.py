import csv
import os
import sys
import matplotlib.pyplot as plt


# Проверяем, передан ли CSV-файл
if len(sys.argv) < 2:
    print("Ошибка: CSV-файл не указан.")
    sys.exit(1)

# Получаем путь к CSV
csv_file = sys.argv[1]

# Папка проекта
project_dir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

# Папка для графика
doc_dir = os.path.join(project_dir, "doc")
os.makedirs(doc_dir, exist_ok=True)

# Файл с графиком
output_file = os.path.join(doc_dir, "graph.png")


# Читаем CSV
x = []
models = {}

with open(csv_file, "r", encoding="utf-8") as file:
    reader = csv.DictReader(file)

    columns = reader.fieldnames

    # Первый столбец — X
    x_column = columns[0]

    # Остальные столбцы — модели
    for column in columns[1:]:
        models[column] = []

    for row in reader:
        x.append(float(row[x_column]))

        for column in models:
            models[column].append(float(row[column]))


# Строим график
plt.figure(figsize=(10, 6))

for name, values in models.items():
    plt.plot(x, values, marker="o", label=name)


plt.xlabel(x_column)
plt.ylabel("Значение")
plt.title("Результаты моделирования")
plt.grid(True)
plt.legend()

# Сохраняем график
plt.savefig(output_file, dpi=300, bbox_inches="tight")

print(f"График сохранён: {output_file}")

plt.show()