# Particle Simulation Project: Coulomb Force Calculation

## Overview

This project simulates the behavior of charged particles in a 2D space and calculates the Coulomb force between them. It supports three modes of operation:

1. Single-threaded execution
2. Multi-threaded execution
3. Distributed execution using MPI

## Dependencies

- C++11 or higher
- MPI (Message Passing Interface) for distributed execution

## Files

- `A1.cpp`: Main program file.
- `particles.cpp`: Handles the charge points in the simulation.
- `data_processor.cpp`: Processes the data for the simulation.
- `parallel.cpp`: Contains parallelization logic.

### Header Files

- `parallel.h`: Header for parallel computing functions.
- `log.h`: Logging utilities.
- `global.h`: Contains global configuration and data structures.
- `data_processor.h`: Header for data processing functions.
- `particles.h`: Header for charge points functions.

## Data Structures

### Particle

Represents a particle in the simulation.

```cpp
struct Particle {
    int id;
    int x;
    int y;
    /* Constructors */
};
```

### ParticleInfo

Extends `Particle` to include force information.

```cpp
struct ParticleInfo : Particle {
    double force;
    /* Constructors */
};
```

### GlobalConfig

Holds global configuration options.

```cpp
struct GlobalConfig {
    size_t thread_count;
    size_t particle_limit;
    int mode;
    int process_count;
    int world_rank;
    size_t start_pos;
    size_t end_pos;
};
```

### GlobalData

Holds global data used in the simulation.

```cpp
struct GlobalData {
    std::deque<Particle> particle_list;
    std::vector<std::pair<size_t, size_t>> chunk_boundaries;
    std::vector<ParticleInfo> particles;
    std::queue<std::vector<Particle>> particle_queue;
    std::chrono::microseconds duration;
}
```

## Usage

### Compilation

```bash
# release mode
cd A1
./build.sh -m release
```

```bash
# debug mode
cd A1
./build.sh -m debug
```

### Running the Program

```bash
# mode 1 or 2
cd A1; build/A1 <mode> <thread_count> [particle_limit]
# or
cd A1; mpiexec -np 1 build/A1 <mode> <thread_count> [particle_limit]
```
```bash
# mode 3
cd A1; mpiexec -np <$num_process> build/A1 3 <thread_count> [particle_limit]
```

- `mode`: The execution mode (1, 2, or 3)
- `thread_count`: The number of threads to use (only relevant for modes 2 and 3)
- `particle_limit`: (Optional) The maximum number of particles to simulate

## Running Test Cases

You can run the test cases using the `test.sh` script.

```bash
cd A1; ./test.sh [-t <thread_count>] [-l <max_lines>] [-d]
```

- `-t`: Number of threads (default is 16)
- `-l`: Maximum number of lines to read from the CSV file
- `-d`: Debug mode
- `-p`: Number of processes for MPI (default is 1)
