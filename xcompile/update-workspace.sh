#!/bin/sh
cd ../
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ../
make -j3
#mogrify -path ./n -format png -fill "#70707070" -opaque "#FF0000" *.png

