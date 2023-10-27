#!/bin/bash

# Function to collect data
collect_data() {
    local mode=$1
    local max_threads=$2
    # local data_size=$3
    local output_file=$3
    local mpi_processes=$4

    echo "Mode,Threads,ExecutionTime" > $output_file

    for i in $(seq 1 $max_threads)
    do
        if [ "$mode" -eq 3 ]; then
            # For Mode 3, use mpiexec
            output=$(mpiexec -np $mpi_processes build/A1 $mode $i 2>/dev/null)
        else
            # For other modes
            output=$(build/A1 $mode $i 2>/dev/null)
        fi

        relevant_line=$(echo "$output" | grep "Mode $mode")
        echo $relevant_line
        execution_time=$(echo $relevant_line | awk '{print $8}')
        echo "$mode,$i,$execution_time" >> $output_file
    done
}

collect_data 2 32 results_mode2.csv 0
# collect_data 3 10 30 results_mode3.csv 8
