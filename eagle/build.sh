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
cp ./build/src/eagle ./release/eagle/
cp -r ./conf  ./release/eagle/

exit 0