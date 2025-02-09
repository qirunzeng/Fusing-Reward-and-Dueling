import os
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.ticker import ScalarFormatter

iterations = 200000
x_data = np.arange(iterations + 1)
PATH = "../fig/regret/"
curve_name = [
    "ElimNoFusion",     # 0
    "ElimNoFusion",     # 1

    "Elim (Reward)",    # 2
    "Elim (Dueling)",   # 3
    
    "ElimNoFusion",     # 4
    "ElimFusion",       # 5
    
    "MEDNoFusion",      # 6
    "MEDNoFusion",      # 7
    
    "DMED (Reward)",    # 8
    "RMED (Dueling)",   # 9
    
    "MEDNoFusion",      # 10 
    "DecoFusion",       # 11 
]

curve_num = len(curve_name)

graph_names = [
    "Reward",
    "Dueling",
    "NO_FUSION",
    "NO_FUSION-reward", 
    "NO_FUSION-dueling",
]

curve_combinations = [
    [0, 6,  5, 11],
    [1, 7,  5, 11],
    [4, 10, 5, 11], 
    [2, 8,  5, 11], 
    [3, 9,  5, 11] 
]

ylabel = [
    "Reward-based regret " + r"$R_T^{(R)}$", 
    "Dueling-based regret " + r"$R_T^{(D)}$",
    "Aggregated regret " + r"$R_T$", 
    "Aggregated regret" + r"$R_T$",
    "Aggregated regret" + r"$R_T$"
]

all_rounds_data = [[] for _ in range(curve_num)]


def draw_plt(all_rounds_data):
    all_rounds_data = np.array(
        [np.array(curve) for curve in all_rounds_data]
    ) 
    mean_data = np.mean(all_rounds_data, axis=1) 
    
    std_data = np.std(all_rounds_data, axis=1)  

    color_map = [
        "green", "green", "green", "green", 
        
        "green", "red", 
        
        "purple", "purple","purple", "purple", 
        
        "purple", "blue"
    ]
    marker_map = [
        "o", "o", "o", "o", 
        
        "o", "^", 
        
        "s", "s", "s", "s", 
        
        "s", "d"
    ]
    linestyle_map = [
        "--", "--", "--", "--", 

        "--", "-.",

        ":", ":", ":", ":", 

        ":", "-"
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
                markevery=(iterations // 10),
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
        ax.xaxis.set_major_formatter(ScalarFormatter(useMathText=True))
        ax.yaxis.set_major_formatter(ScalarFormatter(useMathText=True))
        ax.ticklabel_format(axis="x", style="sci", scilimits=(0, 0))
        ax.ticklabel_format(axis="y", style="sci", scilimits=(0, 0))
        plt.yscale("log")

        plt.xticks(fontsize=18, rotation=45)
        plt.yticks(fontsize=18)
        plt.xlabel("Round " + r"$t$", fontsize=20, fontweight="bold")
        plt.ylabel(ylabel[idx], fontsize=20, fontweight="bold")
        plt.legend(fontsize=18, ncol=2)
        plt.ylim(bottom=10)

        plt.tight_layout()
        if not os.path.exists(PATH):
            os.makedirs(PATH)
        plt.savefig(os.path.join(PATH, f"{graph_names[idx]}.png"))
        plt.close()

with open("../out.nosync/raw.txt") as f:
    round_count = 0
    while True:
        line = f.readline()
        if not line:
            break
        elif line.startswith("#"):
            round_count += 1
            print(f"Processing round: {round_count}")

            data = np.array(
                [
                    list(map(float, f.readline().strip().split()[1:]))
                    for _ in range(iterations + 1)
                ]
            )

            for i in range(curve_num):
                all_rounds_data[i].append(data[:, i])

draw_plt(all_rounds_data)

