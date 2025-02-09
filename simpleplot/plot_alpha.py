import matplotlib.pyplot as plt
import numpy as np
import os
from matplotlib.ticker import ScalarFormatter

PATH = "../fig/alpha/"

rounds = 50
curve_name = [
    "Reward-based regret " + r"$R_T^{(\text{R})}$",
    "Dueling-based regret " + r"$R_T^{(\text{D})}$",
    "Aggregated regret " + r"$R_T$",
]
curve_num = len(curve_name)
graph_names = ["DecoFusion"]
curve_combinations = [[0, 1, 2]]

all_rounds_data = [[] for _ in range(curve_num)]
x_data = np.linspace(0, 1, 51)


def draw_plt(all_rounds_data):
    all_rounds_data = np.array(
        [np.array(curve) for curve in all_rounds_data]
    )
    mean_data = np.mean(all_rounds_data, axis=1)
    std_data = np.std(all_rounds_data, axis=1)  

    color_map = ["green", "red", "blue"]
    marker_map = ["o", "^", "s"]
    linestyle_map = ["--", "-.", "-"]

    plt.rcParams.update({"font.size": 20})

    for idx, curves in enumerate(curve_combinations):
        plt.figure(figsize=(8.0, 6.0), dpi=300)

        for curve_idx in curves:
            plt.plot(
                x_data,
                mean_data[curve_idx],
                label=curve_name[curve_idx],
                linewidth=2,
                marker=marker_map[curve_idx],
                markersize=8,
                markerfacecolor="none",
                markeredgecolor=color_map[curve_idx],
                linestyle=linestyle_map[curve_idx],
                color=color_map[curve_idx],
                markevery=5,
            )

            if np.any(std_data[curve_idx] > 0):
                plt.fill_between(
                    x_data,
                    mean_data[curve_idx] - std_data[curve_idx],
                    mean_data[curve_idx] + std_data[curve_idx],
                    alpha=0.1,
                    color=color_map[curve_idx],
                )

        plt.xticks(
            np.linspace(0, 1, 6), fontsize=18
        ) 

        plt.ylim(0, 500)

        ax = plt.gca()
        ax.yaxis.set_major_formatter(ScalarFormatter(useMathText=True))
        ax.yaxis.set_major_formatter(ScalarFormatter(useMathText=True))
        ax.ticklabel_format(axis="y", style="sci", scilimits=(0, 0))

        plt.xlabel("Parameter " + r"$\alpha$", fontsize=20, fontweight="bold")
        plt.ylabel("Regret", fontsize=20, fontweight="bold")
        plt.legend(fontsize=18)

        plt.tight_layout()
        if not os.path.exists(PATH):
            os.makedirs(PATH)
        plt.savefig(os.path.join(PATH, f"{graph_names[idx]}.png"))
        plt.close()


with open("../out.nosync/alpha.txt") as f:
    round_count = 0
    while True:
        line = f.readline()
        if not line:
            break
        elif line.startswith("alpha"):
            round_count += 1
            print(f"Processing round: {round_count}")

            data = np.array(
                [
                    list(map(float, f.readline().strip().split()[1:]))
                    for _ in range(rounds + 1)
                ]
            )

            for i in range(curve_num):
                all_rounds_data[i].append(data[:, i])

draw_plt(all_rounds_data)
