from pathlib import Path
import numpy as np
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

CURRENT_DIR = Path(__file__).parent.resolve()
OUT_DIR = CURRENT_DIR.parent / "doc"
OUT_DIR = OUT_DIR.resolve()

ALLOWED_INPUTS = {"step", "impulse", "harmonic"}
inputs = [name for name in ["step", "impulse", "harmonic"] if name in ALLOWED_INPUTS]

step_file_path = OUT_DIR / "results_step.csv"

with open(step_file_path, encoding="utf-8") as f:
    titles = f.readline().strip().split(",")[2:]
n_models = len(titles)

fig, axes = plt.subplots(3, n_models, figsize=(4.5 * n_models + 1, 9),
                         sharex=True, squeeze=False)

for row, name in enumerate(inputs):
    csv_path = (OUT_DIR / f"results_{name}.csv").resolve()
    
    if not csv_path.is_relative_to(OUT_DIR):
        raise PermissionError("Попытка несанкционированного обхода директории!")

    data = np.loadtxt(csv_path, delimiter=",", skiprows=1, ndmin=2)
    tau, u = data[:, 0], data[:, 1]
    
    for col in range(n_models):
        ax = axes[row][col]
        ax.step(tau, u, "--", color="gray", label="u", where="post")
        ax.plot(tau, data[:, 2 + col], "o-", label="y")
        ax.grid(True)
        if row == 0:
            ax.set_title(titles[col])
        if col == 0:
            ax.set_ylabel(f"вход: {name}")
        if row == 2:
            ax.set_xlabel("tau")

axes[0][0].legend()
plt.tight_layout()

plot_path = OUT_DIR / "plot.png"
plt.savefig(plot_path, dpi=110)
print("График сохранён в", plot_path)
