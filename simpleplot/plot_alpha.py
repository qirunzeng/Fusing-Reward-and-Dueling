import matplotlib.pyplot as plt
import numpy as np
import os
from matplotlib.ticker import ScalarFormatter

PATH = "../../fig/alpha/"

rounds = 50
curve_name = [
    "Reward-based regret " + r"$R_T^{(\text{R})}$",
    "Dueling-based regret " + r"$R_T^{(\text{D})}$",
    "Aggregated regret " + r"$R_T$",
]
curve_num = len(curve_name)
graph_names = ["DecoFusion"]
curve_combinations = [[0, 1, 2]]

# 用于存储所有轮次的数据
all_rounds_data = [[] for _ in range(curve_num)]
x_data = np.linspace(0, 1, 51)  # 生成 51 个点，alpha 从 0 到 1


def draw_plt(all_rounds_data):
    # 转换为 NumPy 数组，计算均值和标准差
    all_rounds_data = np.array(
        [np.array(curve) for curve in all_rounds_data]
    )  # 转换为 NumPy 数组
    mean_data = np.mean(all_rounds_data, axis=1)  # 每条曲线的均值
    std_data = np.std(all_rounds_data, axis=1)  # 每条曲线的标准差

    # 定义颜色、形状和线条样式的映射
    color_map = ["green", "red", "blue"]
    marker_map = ["o", "^", "s"]
    linestyle_map = ["--", "-.", "-"]

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
                markevery=5,
            )
            # 绘制标准差阴影区域
            if np.any(std_data[curve_idx] > 0):  # 确保 std > 0
                plt.fill_between(
                    x_data,
                    mean_data[curve_idx] - std_data[curve_idx],
                    mean_data[curve_idx] + std_data[curve_idx],
                    alpha=0.1,
                    color=color_map[curve_idx],
                )

        # 设置 x 轴的刻度，只显示 0, 0.2, 0.4, 0.6, 0.8, 1.0
        plt.xticks(
            np.linspace(0, 1, 6), fontsize=18
        )  # 生成 6 个点刻度，0, 0.2, ..., 1.0
        # 设置 y 轴的范围
        plt.ylim(0, 2000)  # 这里设置 y 轴的范围为 0 到 1，可以根据需要调整

        # 设置 y 轴科学计数法
        ax = plt.gca()
        ax.yaxis.set_major_formatter(ScalarFormatter(useMathText=True))
        ax.yaxis.set_major_formatter(ScalarFormatter(useMathText=True))
        ax.ticklabel_format(axis="y", style="sci", scilimits=(0, 0))

        # 设置坐标轴和图例
        plt.xlabel("Parameter " + r"$\alpha$", fontsize=20, fontweight="bold")
        plt.ylabel("Regret", fontsize=20, fontweight="bold")
        plt.legend(fontsize=18)

        # 调整布局并保存图像
        plt.tight_layout()
        if not os.path.exists(PATH):
            os.makedirs(PATH)
        plt.savefig(os.path.join(PATH, f"{graph_names[idx]}.png"))
        plt.close()


# 数据读取并处理
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

# 绘制最终图像
draw_plt(all_rounds_data)
