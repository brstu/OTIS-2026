import pandas as pd
import matplotlib.pyplot as plt
import os

def plot_results():
    csv_file = "result_task03.csv"
    
    # Проверяем, есть ли файл с результатами
    if not os.path.exists(csv_file):
        print(f"[ERROR] File {csv_file} not found. Run the simulation first.")
        return

    # Читаем данные из CSV
    try:
        df = pd.read_csv(csv_file)
    except Exception as e:
        print(f"[ERROR] Failed to read CSV: {e}")
        return

    # Создаем фигуру с двумя графиками (сверху и снизу)
    # Используем _ для fig, так как переменная fig дальше не используется
    _, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 8))

    # --- График 1: Переходный процесс (Цель и Выход) ---
    ax1.plot(df['k'], df['w'], 'r--', label='Target (w)')
    ax1.plot(df['k'], df['y'], 'b-', label='Output (y)')
    ax1.set_title('Transition Process (PID Control)')
    ax1.set_xlabel('Step k')
    ax1.set_ylabel('Value')
    ax1.grid(True, linestyle=':', alpha=0.7)
    ax1.legend()

    # --- График 2: Управляющее воздействие ---
    ax2.plot(df['k'], df['u'], 'g-', label='Control (u)')
    ax2.set_title('Control Signal')
    ax2.set_xlabel('Step k')
    ax2.set_ylabel('Control Value')
    ax2.grid(True, linestyle=':', alpha=0.7)
    ax2.legend()

    # Красивое расположение элементов
    plt.tight_layout()

    # Сохраняем картинку
    output_file = 'plot_task03.png'
    plt.savefig(output_file, dpi=150)
    print(f"[OK] Plot saved as {output_file}")
    
    # Пытаемся автоматически открыть картинку (только для Windows)
    try:
        os.startfile(output_file)
    except AttributeError:
        pass # Для Linux/Mac

if __name__ == "__main__":
    plot_results()