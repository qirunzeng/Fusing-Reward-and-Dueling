import os
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.ticker import ScalarFormatter

iterations = 4
x_data = np.array([x*0.05 + 0.06 for x in range(iterations)])
PATH = "../fig/gap/"
curve_name = [
    "ElimNoFusion",
    "ElimFusion",
    "MEDNoFusion",
    "DecoFusion"
]

curve_num = len(curve_name)

graph_names = [
    "gap-exp",
]
curve_combinations = [
    [0, 1, 2, 3]
]

ylabel = [
    "Aggregated regret " + r"$R_T$", 
]

all_rounds_data = [[] for _ in range(curve_num)]

mean_200000 = []

def draw_plt(all_rounds_data):
    all_rounds_data = np.array(
        [np.array(curve) for curve in all_rounds_data]
    ) 
    mean_data = np.mean(all_rounds_data, axis=1) 
    
    std_data = np.std(all_rounds_data, axis=1) 

    color_map = [
        "green",
        "red",
        "purple",
        "blue"
    ]
    marker_map = [
        "o",
        "^",
        "s",
        "d"
    ]
    linestyle_map = [
        "--",
        "-.",
        ":",
        "-"
    ]

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
            )
            if std_data[curve_idx].any():
                plt.fill_between(
                    x_data,
                    mean_data[curve_idx] - std_data[curve_idx],
                    mean_data[curve_idx] + std_data[curve_idx],
                    alpha=0.1,
                    color=color_map[curve_idx],
                )


        ax = plt.gca()
        ax.yaxis.set_major_formatter(ScalarFormatter(useMathText=True))
        ax.ticklabel_format(axis="y", style="sci", scilimits=(0, 0))
        plt.yscale("log")

        plt.xticks(x_data, fontsize=18, rotation=45)
        plt.yticks(fontsize=18)
        plt.xlabel("Minimal reward gap " + r"$\Delta_2^{(\text{R})}$", fontsize=20, fontweight="bold")
        plt.ylabel(ylabel[idx], fontsize=20, fontweight="bold")
        plt.legend(fontsize=18, ncol=2)
        plt.ylim(bottom=1)

        plt.tight_layout()
        if not os.path.exists(PATH):
            os.makedirs(PATH)
        plt.savefig(os.path.join(PATH, "gap-exp.png"))
        plt.close()


with open("../out.nosync/gap.txt") as f:
    while True:
        line = f.readline()
        if not line:
            break
        elif line.startswith("#"):
            data = np.array(
                [
                    list(map(float, f.readline().strip().split()[1:]))
                    for _ in range(iterations)
                ]
            )
            for i in range(curve_num):
                all_rounds_data[i].append(data[:, i])

draw_plt(all_rounds_data)
