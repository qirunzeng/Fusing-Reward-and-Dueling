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

There are multiple `#if` in `main.cpp`, which will generate different data.

You should make directory `out.nosync` before implementing the code.

### Param

In `include/environment.h`

### Plot

```shell
cd simpleplot

python -u simpleplot.py
python -u plot_gap.py
python -u plot-gap-mat.py
python -u plot_alpha.py 
```

And then the graph will be in `../fig/`, which is not in the code directory.
