import os

try:
  import pandas as pd
  import matplotlib.pyplot as plt
except ImportError as e:
  print(
      f'[CRITICAL ERROR] Не установлена библиотека: {e}. Выполните в консоли:'
      ' pip install matplotlib pandas'
  )
  exit(1)

current_dir = os.path.dirname(os.path.abspath(__file__))  # src
task_01_dir = os.path.abspath(os.path.join(current_dir, '..'))  # task_01
doc_dir = os.path.join(task_01_dir, 'doc')
images_dir = os.path.join(doc_dir, 'images')

os.makedirs(images_dir, exist_ok=True)

models = [
    (
        'Model 1.6 (Динамическая система 3-го порядка)',
        'model_1_6_results.csv',
        'model_1_6.png',
    ),
    (
        'Model 2.10 (Нелинейное сглаживание: tanh)',
        'model_2_10_results.csv',
        'model_2_10.png',
    ),
    (
        'Model 3.4 (Квадратичный самораспад)',
        'model_3_4_results.csv',
        'model_3_4.png',
    ),
]

# Рисуем все 3 модели в одном общем окне для наглядного отображения
fig, axes = plt.subplots(3, 1, figsize=(12, 9))
fig.suptitle('Результаты моделирования ОТИС (Лабораторная №1)', fontsize=14)

success_count = 0

for i, (title, csv_filename, img_filename) in enumerate(models):
  csv_path = os.path.join(task_01_dir, csv_filename)

  if os.path.exists(csv_path):
    df = pd.read_csv(csv_path)

    # 1. Индивидуальный график для сохранения в doc/images/
    plt.figure(figsize=(10, 5))
    plt.plot(
        df['Step'],
        df['StepInput'],
        label='Стуенчатое (u = const)',
        linewidth=2,
    )
    plt.plot(
        df['Step'],
        df['ImpulseInput'],
        label='Импульсное (u_0 = 1)',
        linewidth=2,
    )
    plt.plot(
        df['Step'],
        df['HarmonicInput'],
        label='Гармоническое (sin)',
        linewidth=2,
    )
    plt.title(title)
    plt.xlabel('Шаги симуляции (τ)')
    plt.ylabel('Выход y(τ)')
    plt.legend()
    plt.grid(True)

    img_path = os.path.join(images_dir, img_filename)
    plt.tight_layout()
    plt.savefig(img_path, dpi=300)
    plt.close()

    # 2. Добавляем на общий холст для мгновенного показа
    ax = axes[i]
    ax.plot(
        df['Step'],
        df['StepInput'],
        label='Стуенчатое (u = const)',
        linewidth=2,
    )
    ax.plot(
        df['Step'], df['ImpulseInput'], label='Импульсное (u_0 = 1)', linewidth=2
    )
    ax.plot(
        df['Step'], df['HarmonicInput'], label='Гармоническое (sin)', linewidth=2
    )
    ax.set_title(title)
    ax.set_xlabel('Шаги (τ)')
    ax.set_ylabel('y(τ)')
    ax.legend()
    ax.grid(True)

    print(f'[INFO] График сохранен в doc/images: {img_path}')
    success_count += 1
  else:
    print(f'[WARNING] Файл не найден: {csv_path}')

plt.tight_layout()

if success_count > 0:
  print(
      '[INFO] Открываем окно с графиками... (закройте окно графиков, чтобы'
      ' завершить работу батника)'
  )
  plt.show()  # Мгновенно показывает графики на экране!
else:
  print('[ERROR] Не найдено ни одного .csv файла для отображения.')