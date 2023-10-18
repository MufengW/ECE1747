#!/bin/bash

# Default values
thread_count=16
max_lines=""
debug=0

display_usage() {
    echo "Usage: $0 [-t <thread_count>] [-l <max_lines>] [-d]"
    echo "Options:"
    echo "  -t <thread_count>   Specify the number of threads (default: 16)"
    echo "  -l <max_lines>      Specify the maximum number of lines to read from csv"
    echo "  -d                  Use debug mode for testing"
    echo "  -h                  Display this help message and exit"
}

while getopts ":t:l:dh" opt; do
  case $opt in
    t) thread_count="$OPTARG" ;;
    l) max_lines="$OPTARG" ;;
    d) debug=1 ;;
    h) display_usage; exit 0 ;;
    *) echo "Invalid option: -$OPTARG" >&2; display_usage; exit 1 ;;
  esac
done

# Compile
if (( debug == 1 )); then
    echo "Compiling in debug mode..."
    bash build.sh -m debug
else
    echo "Compiling in release mode..."
    bash build.sh -m release
fi

if [[ -z "$max_lines" ]]; then
    build/A1 1 1 > 1.out
    build/A1 2 "$thread_count" > 2.out
else
    build/A1 1 1 "$max_lines" > 1.out
    build/A1 2 "$thread_count" "$max_lines" > 2.out
fi

# Print the first line of 1.out and 2.out
head -n 1 1.out
head -n 1 2.out

tail -n +3 1.out > result1.out
tail -n +3 2.out > result2.out

if diff -q result1.out result2.out >/dev/null; then
    echo "Test passed!"
    exit 0
else
    echo "Test failed!"
    exit 1
fi
