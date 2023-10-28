import matplotlib.pyplot as plt
import os
import glob

# Initialize variables
data_sizes = [100, 500, 1000, 5000, 10000, 50000, 100000, 500000, 1000000]  # Modify this list based on the data sizes you've tested
result_folder = '/homes/w/wangmufe/ECE1747/A1/results_mode_3'
# Initialize an empty dictionary to hold the times for each data size
execution_times = {}

# Loop through each data size and read the corresponding time file
for max_lines in data_sizes:
    time_file = os.path.join(result_folder, f'time_{max_lines}.txt')
    with open(time_file, 'r') as f:
        times = [int(line.strip()) for line in f.readlines()]
    execution_times[max_lines] = times

# Plot the graph
plt.figure(figsize=(10, 6))
for process in range(len(times)):
    plt.plot(data_sizes, [execution_times[size][process] for size in data_sizes], label=f'Process {process}')

plt.xlabel('Data Size')
plt.ylabel('Execution Time (microseconds)')
plt.title('Mode 3: Execution Time vs Data Size')
plt.legend()
plt.grid(True)
plt.show()
