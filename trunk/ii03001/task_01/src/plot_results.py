import csv
import os
import sys

import matplotlib.pyplot as plt

TITLES = {
    "model11.csv": "Model 1.1: first-order inertial",
    "model23.csv": "Model 2.3: dead zone",
    "model35.csv": "Model 3.5: harmonic driving force",
}

SERIES = {
    "y_step": ("step", "-"),
    "y_impulse": ("impulse", "--"),
    "y_harmonic": ("harmonic", ":"),
}


def plot_file(filename):
    columns = {"t": [], "y_step": [], "y_impulse": [], "y_harmonic": []}
    try:
        with open(filename, newline="", encoding="utf-8") as file:
            for row in csv.DictReader(file):
                for key in columns:
                    columns[key].append(float(row[key]))
    except FileNotFoundError:
        raise SystemExit(f"{filename} not found. Run task_01 first.")

    plt.figure()
    if columns["y_step"] == columns["y_impulse"] == columns["y_harmonic"]:
        # The model does not depend on the input: one curve is enough.
        plt.plot(columns["t"], columns["y_step"], marker=".", linewidth=1,
                 label="y(t) (same for all inputs)")
    else:
        for key, (label, style) in SERIES.items():
            plt.plot(columns["t"], columns[key], linestyle=style, marker=".",
                     linewidth=1, label=label)

    plt.title(TITLES.get(filename, filename))
    plt.xlabel("t (step)")
    plt.ylabel("y(t)")
    plt.grid(True)
    plt.legend()
    plt.tight_layout()
    image_name = filename.replace(".csv", ".png")
    plt.savefig(image_name, dpi=150)
    print("Saved:", image_name)


# With arguments: plot only the given files (the model chosen in the menu).
# Without arguments: plot every model file that exists.
if len(sys.argv) > 1:
    files = sys.argv[1:]
else:
    files = [name for name in TITLES if os.path.exists(name)]

if not files:
    raise SystemExit("CSV files not found. Run task_01 first.")

for name in files:
    plot_file(name)

plt.show()
