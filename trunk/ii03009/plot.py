import pandas as pd
import matplotlib.pyplot as plt
import os
import sys

if len(sys.argv) > 1:
    prefix = sys.argv[1]
else:
    print("Ошибка: C++ программа не передала название модели!")
    sys.exit(1)

if prefix == 'out_linear':
    title_base = 'Линейная модель 1.9'
elif prefix == 'out_nonlinear':
    title_base = 'Нелинейная модель 2.9'
elif prefix == 'out_diffeq':
    title_base = 'Диф. уравнение 3.9'
else:
    title_base = 'Неизвестная модель'

fig, axes = plt.subplots(3, 1, figsize=(10, 10))
fig.suptitle(f'Переходные процессы: {title_base}', fontsize=16)

signals = [
    ('step', 'Ступенчатый'),
    ('impulse', 'Импульсный'),
    ('sin', 'Гармонический (синус)')
]

for i, (suffix, sig_name) in enumerate(signals):
    filename = f'{prefix}_{suffix}.csv'
    if os.path.exists(filename):
        df = pd.read_csv(filename)
        axes[i].plot(df['step'], df['y'], label='y(t)', color='blue', linewidth=2)
        axes[i].plot(df['step'], df['u'], label='u(t)', color='orange', linestyle='--')
        
        axes[i].set_title(sig_name)
        axes[i].set_xlabel('Шаг (t)')
        axes[i].set_ylabel('Значение')
        axes[i].legend()
        axes[i].grid(True)

plt.tight_layout()

save_path = f'../doc/Screenshots/{prefix}_plots.png'
plt.savefig(save_path)

print(f"График успешно сохранен по пути: {save_path}")
plt.show()