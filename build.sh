#!/bin/bash

mkdir build
cd build
cmake ..
make all


cd ..
rm -rf release
mkdir release
mkdir ./release/eagle
cp ./build/src/eagle ./release/eagle/
cp -r ./conf  ./release/eagle/

#make clean
#cd ..
#rm -rf build