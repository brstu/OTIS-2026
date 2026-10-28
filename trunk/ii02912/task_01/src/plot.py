"""Построение графика: python plot.py файл.csv  (окно графика + сохранение .png)"""
import sys, csv, os
import matplotlib.pyplot as plt

path = sys.argv[1]
t, u, y = [], [], []
with open(path) as fh:
    for r in csv.DictReader(fh):
        t.append(int(r["tau"])); u.append(float(r["u"])); y.append(float(r["y"]))

title = os.path.splitext(os.path.basename(path))[0]
fig, ax = plt.subplots(figsize=(10, 5.5))
ax.step(t, u, where="post", color="gray", alpha=.7, label="u(τ)")
ax.plot(t, y, "o-", ms=4, label="y(τ)")
ax.set_title(title)
ax.set_xlabel("τ")
ax.grid(True)
ax.legend()
fig.tight_layout()
png = os.path.splitext(path)[0] + ".png"
fig.savefig(png, dpi=120)
print("График сохранён:", png)
plt.show()
