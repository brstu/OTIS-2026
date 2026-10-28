import pandas as pd
import matplotlib.pyplot as plt
import sys

def main():
    try:
        # Читаем CSV файл, который сгенерировала C++ программа
        df = pd.read_csv('result.csv')
    except FileNotFoundError:
        print("Error: result.csv not found. Run the C++ program first.")
        sys.exit(1)

    # Строим графики
    plt.figure(figsize=(10, 6))
    
    plt.plot(df['tau'], df['u_tau'], label='Input u(t)', linestyle='--')
    plt.plot(df['tau'], df['y_tau'], label='Output y(t)', marker='o')
    
    plt.title('Simulation Results')
    plt.xlabel('Time (tau)')
    plt.ylabel('Value')
    plt.grid(True)
    plt.legend()
    
    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    main()