#!/bin/bash
set -e

WORKDIR="/UMAS_GUI"

echo "Compiling UMAS_GUI..."
cd $WORKDIR
source /opt/ros/humble/setup.bash

# Очистка предыдущей сборки
rm -rf build
mkdir build
cd build

# CMake + Make
cmake ..
make -j$(nproc)

echo "UMAS_GUI compiled successfully."
