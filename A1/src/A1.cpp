#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"

#include <mpi.h>

#pragma GCC diagnostic pop

#include "global.h"
#include "log.h"
#include "data_processor.h"
#include "charge_points.h"
#include "parallel.h"
#include <chrono>

GlobalConfig g_config;
GlobalData g_data;
int main(int argc, char** argv) {
    M_assert(argc == 3 || argc == 4, "Usage: %s <mode> <thread_count> (optional)<particle_limit>", argv[0]);

    int mode = std::atoi(argv[1]);
    g_config.mode = mode;
    M_assert(mode <= 3 && mode >= 1, "mode has to be an integer between 1 to 3!");
    if (mode != 1) {
        g_config.thread_count = std::atoi(argv[2]);
        M_log("thread_count = %ld", g_config.thread_count);
    } else {
        M_log("thread_count set to 1 by default under mode 1.");
    }
    M_assert(g_config.thread_count > 0, "Thread count must be greater than 0!");

    if (argc == 4) {
        g_config.particle_limit = std::atoi(argv[3]);
    }

    MPI_Init(&argc, &argv);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    if (world_size > 1 && mode != 3) {
        MPI_Finalize();
        M_assert(mode == 3 , "Only mode 3 supports MPI!");
    }

    g_config.process_count = world_size;
    // unsigned int n = std::thread::hardware_concurrency();
    // std::cout << "Number of cores: " << n << std::endl;
    const std::string file_name = "data/huge.csv";
    load_data(file_name);
    auto start = std::chrono::high_resolution_clock::now();
    switch (mode) {
        case 1:
            M_log("Running in mode 1");
            compute_and_print_force(g_data.chunk_boundaries[0], 1);
            break;

        case 2:
            M_log("Running in mode 2");
            parallel_threading();
            break;

        case 3:
            /* Get the rank of the process */
            int world_rank;
            MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

            /* Print off a hello world message */
            std::cout << "Hello world from rank " << world_rank << " out of " << world_size << " processes.\n";

            M_log("Running in mode 3");
            break;
        default:
            M_log("Invalid mode specified: %d", mode);
            return 1;
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Mode " << mode << " with " << g_config.thread_count <<
        (g_config.thread_count > 1 ? " threads " : " thread ")
        << "executed in " <<  duration.count() << " microseconds for "
        << g_config.particle_limit << " particles." << std::endl;
    M_log("Mode%d executed in %ld microseconds", mode, duration.count());
    printParticles();
    /* Finalize the MPI environment */
    MPI_Finalize();
    return 0;
}
