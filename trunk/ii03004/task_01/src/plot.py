import csv
import glob
import os
from collections import defaultdict
import matplotlib.pyplot as plt

# Удаляем графики прошлого запуска, чтобы остались только выбранные сейчас
for old_file in glob.glob("model_*.png") + ["plot.png"]:
    if os.path.exists(old_file):
        os.remove(old_file)

# data[(модель, вход)] = (список tau, список u, список y)
data = defaultdict(lambda: ([], [], []))
with open("results.csv", encoding="utf-8") as f:
    for row in csv.DictReader(f):
        taus, us, ys = data[(row["model"], row["input"])]
        taus.append(int(row["tau"]))
        us.append(float(row["u"]))
        ys.append(float(row["y"]))

models = sorted({key[0] for key in data})
inputs = sorted({key[1] for key in data}, key=["step", "impulse", "harmonic"].index)
if not models:
    print("results.csv is empty, nothing to plot")
    raise SystemExit(0)

COLORS = {"step": "tab:blue", "impulse": "tab:orange", "harmonic": "tab:green"}


def draw(ax, model, inp):
    """Одно поле: выход y (сплошная линия) и вход u (пунктир)."""
    taus, us, ys = data[(model, inp)]
    color = COLORS.get(inp, "tab:gray")
    ax.plot(taus, ys, marker=".", color=color, label="y (output)")
    ax.plot(taus, us, linestyle="--", color="black", alpha=0.6, label="u (input)")
    ax.set_title(f"{model} | {inp}")
    ax.set_xlabel("tau")
    ax.set_ylabel("value")
    ax.grid(True)
    ax.legend()


# Отдельный файл для каждой модели: по полю на каждый вход
for model in models:
    fig, axes = plt.subplots(len(inputs), 1, figsize=(9, 3.2 * len(inputs)), squeeze=False)
    for ax, inp in zip(axes[:, 0], inputs):
        draw(ax, model, inp)
    fig.tight_layout()
    file_name = "model_" + model.split()[1].replace(".", "_") + ".png"
    fig.savefig(file_name, dpi=120)
    plt.close(fig)
    print("Plot saved to", file_name)

# Общее окно: строки = модели, столбцы = входы
fig, axes = plt.subplots(len(models), len(inputs),
                         figsize=(5 * len(inputs), 3.2 * len(models)), squeeze=False)
for row, model in enumerate(models):
    for col, inp in enumerate(inputs):
        draw(axes[row, col], model, inp)

fig.tight_layout()
fig.savefig("plot.png", dpi=120)
print("Plot saved to plot.png")
plt.show()
