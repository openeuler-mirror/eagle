#!/bin/bash

mkdir -p out
cd out
#cmake -DDVFS_DEBUG=ON ../ && make
cmake ../ && make
cd -
