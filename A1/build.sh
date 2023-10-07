#!/bin/bash
# Default build type is Release
BUILD_TYPE=Release

# Check if an argument is provided
if [ "$#" -eq 1 ]; then
    BUILD_TYPE=$1
fi

git clean -xdf

rm -rf build
mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE ..
make
cd -