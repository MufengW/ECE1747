#include "global.h"
#include "log.h"
#include "data_processor.h"
#include "particles.h"
#include "parallel.h"
#include "computation.h"
#include <cstdlib>

GlobalConfig g_config;
GlobalData g_data;

void parseArguments(int argc, char** argv);
void initializeMPI(int argc, char** argv);
void finalizeMPI();

int main(int argc, char** argv) {
    /* Parse command-line arguments */
    parseArguments(argc, argv);

    /* Load data from file */
    const std::string file_name = "data/huge.csv";
    loadData(file_name);

    /* Record the start time */
    auto start = std::chrono::high_resolution_clock::now();

    /* Execute the appropriate mode */
    switch (g_config.mode) {
        case 1: {
            std::vector<Particle> particles(g_data.particle_list.begin(), g_data.particle_list.end());
            batchComputeAndStoreParticleForces(particles);
            break;
        }
        case 2: parallel_threading(); break;
        case 3: parallel_processing(); break;
        default: M_log("Invalid mode specified: %d", g_config.mode); return 1;
    }

    /* Record the end time */
    auto end = std::chrono::high_resolution_clock::now();
    g_data.duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    /* Report the results */
    report_result();

    /* Finalize MPI environment */
    finalizeMPI();

    return 0;
}

/*
 * Parse command-line arguments and populate the global configuration.
 */
void parseArguments(int argc, char** argv) {
    M_assert(argc == 3 || argc == 4, "Usage: %s <mode> <thread_count> (optional)<particle_limit>", argv[0]);
    g_config.mode = std::atoi(argv[1]);
    M_assert(g_config.mode <= 3 && g_config.mode >= 1, "mode has to be an integer between 1 to 3!");

    initializeMPI(argc, argv);

    if (g_config.mode == 2) {
        g_config.thread_count = std::atoi(argv[2]);
        M_log("thread_count = %ld", g_config.thread_count);
    } else if (g_config.mode == 3) {
        size_t total_thread_count = std::atoi(argv[2]);
        /* Evenly distribute these threads to each process. */
        M_assert(static_cast<int>(total_thread_count) >= g_config.process_count,
            "Number of threads must not be less than number of MPI process!");
        g_config.thread_count = total_thread_count / g_config.process_count;
        int remaining_threads = total_thread_count % g_config.process_count;
        if (g_config.world_rank < remaining_threads) {
            g_config.thread_count += 1;
        }
    } else {
        M_log("thread_count set to 1 by default under mode 1.");
    }
    M_assert(g_config.thread_count > 0, "Thread count must be greater than 0!");

    if (argc == 4) {
        g_config.particle_limit = std::atoi(argv[3]);
    }
}

/*
 * Initialize the MPI environment.
 */
void initializeMPI(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &g_config.process_count);
    if (g_config.process_count > 1 && g_config.mode != 3) {
        MPI_Finalize();
        M_assert(g_config.mode == 3 , "Only mode 3 supports MPI!");
    }
    if (g_config.mode == 3) {
        /* Get the MPI rank of the current process */
        MPI_Comm_rank(MPI_COMM_WORLD, &g_config.world_rank);
    }
}

/*
 * Finalize the MPI environment.
 */
void finalizeMPI() {
    MPI_Finalize();
}
