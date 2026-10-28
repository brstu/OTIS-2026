import sys
import os
import numpy as np
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

out_dir = sys.argv[1] if len(sys.argv) > 1 else "."

inputs = ["step", "impulse", "harmonic"]

with open(os.path.join(out_dir, "results_step.csv"), encoding="utf-8") as f:
    titles = f.readline().strip().split(",")[2:]
n_models = len(titles)

fig, axes = plt.subplots(3, n_models, figsize=(4.5 * n_models + 1, 9),
                         sharex=True, squeeze=False)
for row, name in enumerate(inputs):
    data = np.loadtxt(os.path.join(out_dir, f"results_{name}.csv"),
                      delimiter=",", skiprows=1, ndmin=2)
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
plt.savefig(os.path.join(out_dir, "plot.png"), dpi=110)
print("График сохранён в", os.path.join(out_dir, "plot.png"))
