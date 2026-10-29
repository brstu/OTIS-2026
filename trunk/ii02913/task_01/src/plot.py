import os
import pandas as pd
import matplotlib.pyplot as plt

def display_single_chart(csv_path="simulation_data.csv"):
    if not os.path.exists(csv_path):
        print(f"Error: {csv_path} not found. Run the C++ program first.")
        return

    df = pd.read_csv(csv_path)
    if df.empty:
        print("CSV file is empty.")
        return

    model_title = df["ModelTitle"].iloc[0]
    signal_type = df["SignalType"].iloc[0]

    plt.figure(figsize=(9, 5))
    plt.plot(df["Step"], df["InputU"], label="Control Input u(τ)", linestyle="--", color="gray")
    plt.plot(df["Step"], df["OutputY"], label="Model Output y(τ)", marker="o", markersize=4, color="tab:blue")

    plt.title(f"{model_title} — {signal_type} Input (Variant 13)")
    plt.xlabel("Step (τ)")
    plt.ylabel("Value")
    plt.grid(True)
    plt.legend()
    plt.tight_layout()

    plt.savefig("plot.png", dpi=150)
    print("Chart saved to plot.png")
    plt.show()

if __name__ == "__main__":
    display_single_chart()