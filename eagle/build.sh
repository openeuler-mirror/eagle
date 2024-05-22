#!/bin/bash

mkdir build
cd build
cmake ..
if [ $? -ne 0 ]; then
    exit 1
fi

make all
if [ $? -ne 0 ]; then
    exit 1
fi

cd ..
rm -rf release
mkdir release
mkdir ./release/eagle
mkdir ./release/eagle/lib
cp ./build/src/eagle ./release/eagle/
cp -r ./conf  ./release/eagle/

find ./build/src/ -name "*.so" -exec cp {} ./release/eagle/lib \;

exit 0