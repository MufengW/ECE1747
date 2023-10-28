#!/bin/bash

BUILD_TYPE=Release
USE_MPI=OFF

display_usage() {
    echo "Usage: $0 [-m <build_mode>] [-p]"
    echo "Options:"
    echo "  -m <build_mode>     Specify the build mode (release or debug, default: release)"
    echo "  -p                  Enable MPI support"
    echo "  -h                  Display this help message and exit"
}

while getopts ":m:ph" opt; do
  case $opt in
    m)
       case "$OPTARG" in
           release) BUILD_TYPE=Release ;;
           debug) BUILD_TYPE=Debug ;;
           *) echo "Invalid build mode: $OPTARG" >&2; display_usage; exit 1 ;;
       esac
    ;;
    p)
       USE_MPI=ON
    ;;
    h)
       display_usage
       exit 0
    ;;
    *)
       echo "Invalid option: -$OPTARG" >&2
       display_usage
       exit 1
    ;;
  esac
done

git clean -xdf

rm -rf build
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DUSE_MPI=$USE_MPI ..
make
cd -
