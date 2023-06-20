#!/bin/bash

if [ ! -f "/lib64/libgtest.so" ]; then
    sudo yum install gtest-devel
fi

mkdir -p out
cd out
cmake ../../ && make
#cmake -DDVFS_DEBUG=ON ../../ && make
cd -
