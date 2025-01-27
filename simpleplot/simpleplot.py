import os
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.ticker import ScalarFormatter

# 配置参数
iterations = 200000
x_data = np.arange(iterations + 1)
PATH = "../../fig/regret/"
curve_name = [
    "ElimNoFusion",
    "ElimNoFusion",
    "ElimNoFusion",
    "ElimFusion",
    "MEDNoFusion",
    "MEDNoFusion",
    "MEDNoFusion",
    "DecoFusion",
]
curve_num = len(curve_name)
graph_names = ["Reward", "Dueling", "NO_FUSION"]
curve_combinations = [[0, 4, 3, 7], [1, 5, 3, 7], [2, 6, 3, 7]]

# 用于存储所有轮次的数据
all_rounds_data = [[] for _ in range(curve_num)]


def draw_plt(all_rounds_data):
    # 转换为 NumPy 数组，计算均值和标准差
    all_rounds_data = np.array(
        [np.array(curve) for curve in all_rounds_data]
    )  # 转换为 NumPy 数组
    mean_data = np.mean(all_rounds_data, axis=1)  # 每条曲线的均值
    std_data = np.std(all_rounds_data, axis=1)  # 每条曲线的标准差

    # 定义颜色、形状和线条样式的映射
    color_map = ["green", "green", "green", "red", "purple", "purple", "purple", "blue"]
    marker_map = ["o", "o", "o", "^", "s", "s", "s", "d"]
    linestyle_map = ["--", "--", "--", "-.", ":", ":", ":", "-"]

    # 设置全局字体大小
    plt.rcParams.update({"font.size": 20})

    # 遍历每张图的曲线组合
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
        ax.xaxis.set_major_formatter(ScalarFormatter(useMathText=True))
        ax.yaxis.set_major_formatter(ScalarFormatter(useMathText=True))
        ax.ticklabel_format(axis="x", style="sci", scilimits=(0, 0))
        ax.ticklabel_format(axis="y", style="sci", scilimits=(0, 0))

        # 设置坐标轴和图例
        plt.xticks(fontsize=18, rotation=45)
        plt.yticks(fontsize=18)
        plt.xlabel("Rounds " + r"$t$", fontsize=20, fontweight="bold")
        plt.ylabel("Aggregated regret" + r"$R_T$", fontsize=20, fontweight="bold")
        ## Please label the y-axis with the corresponding names
        # plt.ylabel(
        #     "Reward-based regret " + r"$R_T^{(\text{R})}$",
        #     fontsize=20,
        #     fontweight="bold",
        # )
        # plt.ylabel(
        #     "Dueling-based regret " + r"$R_T^{(\text{D})}$",
        #     fontsize=20,
        #     fontweight="bold",
        # )
        plt.legend(fontsize=18)
        plt.ylim(bottom=0, top=2500)

        # 调整布局并保存图像
        plt.tight_layout()
        if not os.path.exists(PATH):
            os.makedirs(PATH)
        plt.savefig(os.path.join(PATH, f"{graph_names[idx]}.png"))
        plt.close()


# 读取数据文件
with open("../out.nosync/raw.txt") as f:
    round_count = 0
    while True:
        line = f.readline()
        if not line:
            break
        elif line.startswith("#"):
            round_count += 1
            print(f"Processing round: {round_count}")

            # 读取并解析当前轮次数据
            data = np.array(
                [
                    list(map(float, f.readline().strip().split()[1:]))
                    for _ in range(iterations + 1)
                ]
            )

            # 使用转置直接存储每条曲线数据
            for i in range(curve_num):
                all_rounds_data[i].append(data[:, i])

# 绘制图像
draw_plt(all_rounds_data)
