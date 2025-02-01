#!/bin/zsh

rm -rf build

mkdir build

cd build

cmake ..

make

./main

cd ../simplot

python -u plot-gap-mat.py

python -u plot_gap.py