import csv
import glob
import matplotlib.pyplot as plt

files = sorted(glob.glob("model*.csv"))
if not files:
    raise SystemExit("CSV files not found. Run task_01 first.")

for filename in files:
    t_values, y_values = [], []
    with open(filename, newline="", encoding="utf-8") as file:
        for row in csv.DictReader(file):
            t_values.append(int(row["t"]))
            y_values.append(float(row["y"]))
    plt.figure()
    plt.plot(t_values, y_values, marker=".", linewidth=1)
    plt.title(filename.replace(".csv", ""))
    plt.xlabel("t (step)")
    plt.ylabel("y(t)")
    plt.grid(True)
    plt.tight_layout()
    image_name = filename.replace(".csv", ".png")
    plt.savefig(image_name, dpi=150)
    print("Saved:", image_name)

plt.show()
