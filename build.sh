#!/bin/bash
mkdir -p build
cd build

cmake_options="-G Ninja -DCMAKE_INSTALL_PREFIX=${HOME}/libs/jellyGraph"

cmake $cmake_options ..
ninja
ninja install
mv test ..
