# Fusing Reward and Relative Feedback for Reinforcement Learning

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

### Data Generating

There are 4 `$if` in main.cpp:

1. `#if RUN_GAP`, controls whether to run the experiment to see regrets with different gaps between the expectation, with fixed $\alpha = 0.5$ and `T = 200000`
2. `#if RUN_GAP_MAT`, controls whether to run the experiment to see regrets with different gaps between the preference matrix, with fixed $\alpha = 0.5$ and `T = 200000`
3. `#if RUN_WITH_T`, controls whether to run the experiment to see the regrets with different `T`, with fixed $\alpha = 0.5$
4. `#if RUN_WITH_ALPHA`, controls whether to run the experiment to see the regrets with different $\alpha$, with fixed `T = 200000`

### Param

In `include/environment.h`

### Plot

```shell
cd simpleplot

python simpleplot.py 

python plot_alpha.py 

python plot_gap.py

python plot-gap-mat.py
```
