import os
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.ticker import ScalarFormatter

# 配置参数
iterations = 4
x_data = np.array([x*0.05 + 0.06 for x in range(iterations)])
PATH = "../../fig/gap/"
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

# 用于存储所有轮次的数据
all_rounds_data = [[] for _ in range(curve_num)]

mean_200000 = []

def draw_plt(all_rounds_data):
    # 转换为 NumPy 数组，计算均值和标准差
    all_rounds_data = np.array(
        [np.array(curve) for curve in all_rounds_data]
    )  # 转换为 NumPy 数组
    mean_data = np.mean(all_rounds_data, axis=1)  # 每条曲线的均值
    
    std_data = np.std(all_rounds_data, axis=1)  # 每条曲线的标准差

    # 定义颜色、形状和线条样式的映射
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

    # 设置全局字体大小
    plt.rcParams.update({"font.size": 20})

    # 遍历每张图的曲线组合
    for idx, curves in enumerate(curve_combinations):
        print(idx)
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
            # 绘制标准差阴影区域
            if std_data[curve_idx].any():
                plt.fill_between(
                    x_data,
                    mean_data[curve_idx] - std_data[curve_idx],
                    mean_data[curve_idx] + std_data[curve_idx],
                    alpha=0.1,
                    color=color_map[curve_idx],
                )


        # 设置科学计数法坐标轴
        ax = plt.gca()
        # ax.xaxis.set_major_formatter(ScalarFormatter(useMathText=True))
        ax.yaxis.set_major_formatter(ScalarFormatter(useMathText=True))
        # ax.ticklabel_format(axis="x", style="sci", scilimits=(0, 0))
        ax.ticklabel_format(axis="y", style="sci", scilimits=(0, 0))
        plt.yscale("log")

        # 设置坐标轴和图例
        plt.xticks(x_data, fontsize=18, rotation=45)
        plt.yticks(fontsize=18)
        plt.xlabel("Minimal reward gap " + r"$\Delta_2^{(\text{R})}$", fontsize=20, fontweight="bold")
        plt.ylabel(ylabel[idx], fontsize=20, fontweight="bold")
        # 让 curve name 显示为 2 * 2 而不是 4 * 1
        plt.legend(fontsize=18, ncol=2)
        plt.ylim(bottom=1)

        # 调整布局并保存图像
        plt.tight_layout()
        if not os.path.exists(PATH):
            os.makedirs(PATH)
        plt.savefig(os.path.join(PATH, "gap-exp.png"))
        plt.close()


# 读取数据文件
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

# 绘制图像
draw_plt(all_rounds_data)
