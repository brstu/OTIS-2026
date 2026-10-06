import sys
import csv
import matplotlib.pyplot as plt


def plot_graph(filename):
    t = []
    u = []
    y = []

    with open(filename, "r", encoding="utf-8") as file:
        reader = csv.reader(file, delimiter=";")
        next(reader)

        for row in reader:
            if not row:
                continue

            t.append(float(row[0]))
            u.append(float(row[1]))
            y.append(float(row[2]))

    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(8, 7))

    # y(t)
    ax1.plot(t, y, "b-", label="y(t) — выход")
    ax1.set_ylabel("y")
    ax1.set_title("Выход модели")
    ax1.grid(True)
    ax1.legend()

    # u(t)
    ax2.plot(t, u, "r-", label="u(t) — вход")
    ax2.set_xlabel("t")
    ax2.set_ylabel("u")
    ax2.set_title("Вход модели")
    ax2.grid(True)
    ax2.legend()

    plt.tight_layout()
    plt.savefig("graph.png", dpi=150)
    plt.show()


def main():
    filename = sys.argv[1]
    plot_graph(filename)


if __name__ == "__main__":
    main()