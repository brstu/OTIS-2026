# -*- coding: utf-8 -*-
import os
import csv
import matplotlib.pyplot as plt

steps = []
u_vals = []
y_vals = []
model_name = ""
signal_name = ""

# Read simulation results from build folder
with open("build/results.csv", mode="r") as f:
    reader = csv.DictReader(f)
    for row in reader:
        steps.append(int(row["step"]))
        u_vals.append(float(row["u"]))
        y_vals.append(float(row["y"]))
        model_name = row["model_name"]
        signal_name = row["signal"]

# Configure plot
plt.figure(figsize=(10, 5))
plt.plot(steps, y_vals, label="System output y(t) [" + model_name + "]", color="#1f77b4", linewidth=2, marker="o", markersize=4)
plt.step(steps, u_vals, label="Input signal u(t) [" + signal_name + "]", color="#ff7f0e", linestyle="--", where="post", alpha=0.8)

plt.title("Simulation: " + model_name + " (Signal: " + signal_name + ")", fontsize=13)
plt.xlabel("Step (k)", fontsize=11)
plt.ylabel("Value", fontsize=11)
plt.grid(True, linestyle=":", alpha=0.7)
plt.legend()
plt.tight_layout()

# Create doc directory if needed and save plot
if not os.path.exists("doc"):
    os.makedirs("doc")

output_img = os.path.join("doc", model_name.replace(" ", "_") + "_" + signal_name + ".png")
plt.savefig(output_img, dpi=200)
plt.close()

print("Plot saved successfully to: " + output_img)

# Automatically open the saved image in Windows viewer
os.startfile(os.path.abspath(output_img))