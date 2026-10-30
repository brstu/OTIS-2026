import csv
from pathlib import Path
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

RESULTS_CSV_NAME = "results.csv"


class SimulationVisualizer:
    def __init__(self, data_file: Path) -> None:
        self.data_file = data_file
        self.steps = []
        self.inputs = []
        self.outputs = []
        self.model_title = "Dynamic Model"
        self.signal_title = "Input Signal"

    def read_dataset(self) -> bool:
        if not self.data_file.is_file():
            print(f"Error: dataset file '{self.data_file}' not found.")
            return False

        with open(self.data_file, mode="r", encoding="utf-8") as stream:
            table_reader = csv.DictReader(stream)
            for row in table_reader:
                self.steps.append(int(row["step"]))
                self.inputs.append(float(row["u"]))
                self.outputs.append(float(row["y"]))
                self.model_title = row.get("model_name", self.model_title)
                self.signal_title = row.get("signal", self.signal_title)
        return bool(self.steps)

    def render_and_save(self, destination: Path) -> None:
        fig, axes = plt.subplots(nrows=2, ncols=1, figsize=(10, 6), sharex=True)

        axes[0].step(self.steps, self.inputs, where="post", color="#1f77b4", linewidth=1.8, label="Control u(t)")
        axes[0].set_ylabel("Input u", fontsize=11)
        axes[0].set_title(f"{self.model_title} under {self.signal_title} excitation", fontsize=12, fontweight="bold")
        axes[0].grid(True, linestyle="--", alpha=0.6)
        axes[0].legend(loc="upper right")

        axes[1].plot(self.steps, self.outputs, color="#d62728", marker="o", markersize=3.5, linewidth=1.5, label="Response y(t)")
        axes[1].set_xlabel("Discrete step (k)", fontsize=11)
        axes[1].set_ylabel("State y", fontsize=11)
        axes[1].grid(True, linestyle="--", alpha=0.6)
        axes[1].legend(loc="upper right")

        plt.tight_layout()
        plt.savefig(destination, dpi=250)
        plt.close(fig)
        print(f"Plot successfully saved to: {destination.resolve()}")


def main() -> None:
    base_folder = Path(__file__).resolve().parent
    default_csv = Path(RESULTS_CSV_NAME)
    csv_path = default_csv if default_csv.exists() else base_folder / RESULTS_CSV_NAME
    doc_target = base_folder.parent / "doc" / "plot_task01.png"

    doc_target.parent.mkdir(parents=True, exist_ok=True)

    app = SimulationVisualizer(csv_path)
    if app.read_dataset():
        app.render_and_save(doc_target)


if __name__ == "__main__":
    main()