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
- `charge_points.cpp`: Handles the charge points in the simulation.
- `data_processor.cpp`: Processes the data for the simulation.
- `parallel.cpp`: Contains parallelization logic.

### Header Files

- `parallel.h`: Header for parallel computing functions.
- `log.h`: Logging utilities.
- `global.h`: Contains global configuration and data structures.
- `data_processor.h`: Header for data processing functions.
- `charge_points.h`: Header for charge points functions.

## Data Structures

### Particle

Represents a particle in the simulation.

```cpp
struct Particle {
    int id;
    int x;
    int y;
    // Constructors
};
```

### ParticleInfo

Extends `Particle` to include force information.

```cpp
struct ParticleInfo : Particle {
    double force;
    // Constructors
};
```

### GlobalConfig

Holds global configuration options.

```cpp
struct GlobalConfig {
    size_t thread_count;
    size_t particle_limit;
    int mode;
    // Other fields
};
```

### GlobalData

Holds global data used in the simulation.

```cpp
struct GlobalData {
    std::deque<Particle> particleVector;
    std::vector<ParticleInfo> particles;
    // Other fields
};
```

## Usage

### Compilation

```bash
$ g++ main.cpp -o A1 -lmpi
```

### Running the Program

```bash
$ ./A1 <mode> <thread_count> [particle_limit]
```

- `mode`: The execution mode (1, 2, or 3)
- `thread_count`: The number of threads to use (only relevant for modes 2 and 3)
- `particle_limit`: (Optional) The maximum number of particles to simulate

## Running Test Cases

You can run the test cases using the `test.sh` script.

```bash
$ ./test.sh [-t <thread_count>] [-l <max_lines>] [-d]
```

- `-t`: Number of threads (default is 16)
- `-l`: Maximum number of lines to read from the CSV file
- `-d`: Debug mode
- `-p`: Number of processes for MPI (default is 1)

## Contributing

Feel free to submit pull requests or open issues to improve the project.
