# Fusing Reward and Relative Feedback for Reinforcement Learning

[TOC]

### Execute

```shell
rm -rf build
mkdir build
cd build
cmake ..
make
```

or:

```shell
sh start.sh
```

### 生成数据

`main.cpp` 中有两个 `#if`，可根据注释修改 `#if 0`, `#if 1`。

- 第一个 `if` 表示是否生成 regret 根据 time slot 变化的数据
- 第二个 `if` 表示是否生成 regret 根据 $\alpha$ 变化的数据

### Param

位于 `include/environment.h` 中，可直接修改

### Plot

```shell
cd simpleplot

python simpleplot.py # 画 3 幅 regret 根据 time slot 变化的图

python plot_alpha.py # 画 1 幅根据 alpha 变化的图
```