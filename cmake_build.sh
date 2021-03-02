#! /bin/bash

if [ $# -eq 1 ]; then
if [ $1 == "clean" ]; then
    rm -rf build
    rm -rf output
    exit 0
fi
fi

if [ ! -d "build" ]; then
    mkdir -p build
fi

cd build
cmake ..
make
