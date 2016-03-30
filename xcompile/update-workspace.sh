#!/bin/sh
cd ../
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ../
make -j3

