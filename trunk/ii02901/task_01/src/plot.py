import pandas as pd
import matplotlib.pyplot as plt

file_path = '../build/results.csv'
df = pd.read_csv(file_path, sep=';')

inputs = df['Input'].unique()

fig, axes = plt.subplots(nrows=len(inputs), ncols=1, figsize=(10, 12))

if len(inputs) == 1:
    axes = [axes]

for ax, inp in zip(axes, inputs):
    subset = df[df['Input'] == inp]
    
    ax.plot(subset['tau'], subset['y1(tau)'], marker='o', linestyle='-', label='y1(tau)', color='blue')
    ax.plot(subset['tau'], subset['y2(tau)'], marker='s', linestyle='-', label='y2(tau)', color='green')
    
    ax2 = ax.twinx()
    ax2.plot(subset['tau'], subset['y3(tau)'], marker='^', linestyle='-', label='y3(tau)', color='red')
    
    ax.set_title(f'Реакции системы при воздействии: {inp}', fontsize=14, pad=10)
    ax.set_xlabel('tau (время)', fontsize=12)
    ax.set_ylabel('y1(tau), y2(tau)', fontsize=12, color='black')
    ax2.set_ylabel('y3(tau)', fontsize=12, color='red')
    
    ax.grid(True, linestyle='--', alpha=0.6)
    
    lines_1, labels_1 = ax.get_legend_handles_labels()
    lines_2, labels_2 = ax.get_legend_handles_labels()
    ax.legend(lines_1 + lines_2, labels_1 + labels_2, loc='upper left')

plt.tight_layout()
plt.show()