import matplotlib.pyplot as plt
import numpy as np
import os

arm = 0
rounds = 0
iterations = 0
seed = 0
delta = 0.0
alpha = 0.0

PATH = '../../fig/alpha/'

curve_name = ["reward regret", "duel regret", "alg2 regret"]

curve_num = 3

# 用于存储所有轮次的数据
all_rounds_data = [[] for _ in range(curve_num)]
x_mapped = None



graph_names = [
    'alg2'
]

curve_combinations = [
    [0, 1, 2]
]

def draw_plt(all_rounds_data, x_mapped, x_data):
    # 计算均值和标准差
    mean_data = []  # 每条曲线的均值
    std_data = []   # 每条曲线的标准差

    for arr in all_rounds_data:
        curve_data = np.array(arr)
        mean_data.append(np.mean(curve_data, axis=0))
        std_data.append(np.std(curve_data, axis=0))

    print(all_rounds_data)
    print(std_data)

    # 定义每条曲线的标记形状
    markers = ['^', 's', 'D']  # 三角形、正方形、菱形

    # 遍历每张图的曲线组合
    for idx, curves in enumerate(curve_combinations):
        # 绘制图像
        plt.figure(figsize=(8.0, 6.0), dpi=200)

        for i, curve_idx in enumerate(curves):
            # 绘制均值曲线，并指定不同的标记形状
            plt.plot(
                x_mapped, 
                mean_data[curve_idx], 
                label=curve_name[curve_idx], 
                marker=markers[i % len(markers)],  # 使用不同的标记形状
                markersize=8,                      # 标记大小
                markerfacecolor='none',            # 空心填充
                markeredgecolor=plt.cm.tab10(i),   # 边框颜色
                linestyle='-',                     # 线条样式
                color=plt.cm.tab10(i)              # 线条颜色
            )
            # 绘制标准差阴影区域（仅当 std > 0 时绘制）
            if np.any(std_data[curve_idx] > 0):  # 检查标准差是否大于 0
                plt.fill_between(
                    x_mapped, 
                    mean_data[curve_idx] - std_data[curve_idx], 
                    mean_data[curve_idx] + std_data[curve_idx], 
                    alpha=0.2,  # 阴影透明度
                    color=plt.cm.tab10(i)  # 阴影颜色
                )

        plt.legend()
        plt.xticks(x_mapped[::5], x_data[::5], rotation=45, fontsize=10)  # 旋转刻度标签，避免重叠
        plt.xlabel('alpha')
        plt.ylabel('regret')
        plt.title(f'rounds: {rounds}, arms: {arm}, iterations: {iterations}, seed: {seed}, delta: {delta}')
        
        # 调整布局，确保刻度标签不被裁剪
        plt.tight_layout()
        
        # 保存图像
        if not os.path.exists(PATH + str(seed)):
            os.makedirs(PATH + str(seed))
        curr_path = PATH + str(seed) + '/'
            
        plt.savefig(curr_path + graph_names[idx] + f'_{rounds}_{arm}_{iterations}_{seed}_{delta}_{alpha}.png')
        plt.close()  # 关闭图像，释放内存
    
flag = False
    
x_data = []
with open('../out/alpha.txt') as f:
    while True:
        data = []
        line = f.readline()
        if not line:
            break
        
        if line.startswith('>'):
            
            if flag:
                draw_plt(all_rounds_data, x_mapped, x_data)
                all_rounds_data = [[] for _ in range(curve_num)]
                x_mapped = None
            
            flag = True
            line = f.readline()
            if line[-1] == '\n':
                line = line[:-1]
            words = line.strip().split()
            for i in range(len(words)):
                if words[i] == 'rounds':
                    rounds = int(words[i-1])
                elif words[i] == 'arms':
                    arm = int(words[i-1])
                elif words[i] == 'iterations':
                    iterations = int(words[i-1])
                    

            line = f.readline()
            if line[-1] == '\n':
                line = line[:-1]
            words = line.strip().split()
            for i in range(len(words)):
                if words[i] == 'seed:':
                    seed = int(words[i+1])
            line = f.readline()
            if line[-1] == '\n':
                line = line[:-1]
            words = line.strip().split()
            for i in range(len(words)):
                if words[i] == 'delta:':
                    delta = float(words[i+1])
            line = f.readline()
            if line[-1] == '\n':
                line = line[:-1]
            words = line.strip().split()
            for i in range(len(words)):
                if words[i] == 'alpha:':
                    alpha = float(words[i+1])
        
        elif line.startswith('alpha'):
            line = f.readline()
            x_data = []
            while line[0].isdigit():
                if line[-1] == '\n':
                    line = line[:-1]
                words = line.strip().split()
                x_data.append(float(words[0]))
                data.append([float(x) for x in words[1:]])
                line = f.readline()
                if not line:
                    break
        
            # 将 x_data 投影到 [0, iterations] 区间
            x_mapped = [x / max(x_data) * iterations for x in x_data]

            # 将当前轮次的数据存储到 all_rounds_data 中
            for i in range(curve_num):
                current_curve_data = []
                for j in range(len(data)):
                    current_curve_data.append(data[j][i])
                all_rounds_data[i].append(current_curve_data)
draw_plt(all_rounds_data, x_mapped, x_data)