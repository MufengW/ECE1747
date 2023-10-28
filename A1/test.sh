#!/bin/bash

# Initialize variables
result_folder="results_mode_3"
num_process=8
thread_count=64

# Create result folder if it doesn't exist
mkdir -p $result_folder

# Data sizes to test. You can modify this array as needed.
data_sizes=(100 500 1000 5000 10000 50000 100000 500000 1000000)

# Loop through each data size
echo "Data collection for Mode 3 started."
for max_lines in "${data_sizes[@]}"; do
  # Run the program and redirect output to a file
  mpiexec -np "$num_process" build/A1 3 "$thread_count" "$max_lines" > $result_folder/${max_lines}_3.out 2> /dev/null

  # Extract and save the time for each process
  grep 'Mode 3 with' $result_folder/${max_lines}_3.out | awk '{print $8}' >> $result_folder/time_${max_lines}.txt

done
echo "Data collection for Mode 3 completed."
