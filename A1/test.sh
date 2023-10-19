#!/bin/bash

# Default values
thread_count=16
num_process=1
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

while getopts ":t:l:p:dh" opt; do
  case $opt in
    t) thread_count="$OPTARG" ;;
    l) max_lines="$OPTARG" ;;
    p) num_process="$OPTARG" ;;
    d) debug=1 ;;
    h) display_usage; exit 0 ;;
    *) echo "Invalid option: -$OPTARG" >&2; display_usage; exit 1 ;;
  esac
done

# Get the number of CPU cores using lscpu and awk
num_cores=$(lscpu | awk -F: '/^Core\(s\) per socket:/ {print $2}' | tr -d ' ')

# Check if num_process is greater than num_cores
if [ $num_process -gt $num_cores ]; then
  echo "Error: Number of processes ($num_process) is greater than the number of available CPU cores ($num_cores)."
  exit 1
fi

# Compile
if (( debug == 1 )); then
    echo "Compiling in debug mode..."
    bash build.sh -m debug
else
    echo "Compiling in release mode..."
    bash build.sh -m release
fi

if [[ -z "$max_lines" ]]; then
    build/A1 1 1 > 1.out 2> /dev/null
    build/A1 2 "$thread_count" > 2.out 2> /dev/null
    mpiexec -np "$num_process" build/A1 3 "$thread_count" > 3.out 2> /dev/null
else
    build/A1 1 1 "$max_lines" > 1.out 2> /dev/null
    build/A1 2 "$thread_count" "$max_lines" > 2.out 2> /dev/null
    mpiexec -np "$num_process" build/A1 3 "$thread_count" "$max_lines" > 3.out 2> /dev/null
fi

# Print the first line of 1.out and 2.out
head -n 1 1.out
head -n 1 2.out
head -n 1 3.out

tail -n +3 1.out > result1.out
tail -n +3 2.out > result2.out
tail -n +3 3.out > result3.out

# Testing mode 2
if diff -q result1.out result2.out >/dev/null; then
    echo "Mode2 test passed!"
else
    echo "Mode2 test failed!"
fi

# Testing mode 3
if diff -q result1.out result3.out >/dev/null; then
    echo "Mode3 test passed!"
else
    echo "Mode3 test failed!"
fi
