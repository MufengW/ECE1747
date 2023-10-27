#include "global.h"
#include "log.h"
#include "data_processor.h"
#include "particles.h"
#include "parallel.h"
#include "computation.h"
#include <cstdlib>
#include <functional>

GlobalConfig g_config;
GlobalData g_data;

void report_result();
void parse_arguments(int argc, char** argv);
void initialize_MPI(int argc, char** argv);
void finalize_MPI();

int main(int argc, char** argv) {
    /* Parse command-line arguments */
    parse_arguments(argc, argv);

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
    finalize_MPI();

    return 0;
}

/*
 * Parse command-line arguments and populate the global configuration.
 */
void parse_arguments(int argc, char** argv) {
    M_assert(argc == 3 || argc == 4, "Usage: %s <mode> <thread_count> (optional)<particle_limit>", argv[0]);
    g_config.mode = std::atoi(argv[1]);
    M_assert(g_config.mode <= 3 && g_config.mode >= 1, "mode has to be an integer between 1 to 3!");

    initialize_MPI(argc, argv);

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
void initialize_MPI(int argc, char** argv) {
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
void finalize_MPI() {
    MPI_Finalize();
}

/*
 * Report the execution time and other details of the program.
 */
void report_time() {
    if (g_config.mode == 3) {
        std::cout << "Process #" << g_config.world_rank << std::endl;
    }
    std::cout << "Mode " << g_config.mode << " with " << g_config.thread_count <<
        (g_config.thread_count > 1 ? " threads " : " thread ")
        << "executed in " <<  g_data.duration.count() << " microseconds for "
        << g_config.particle_limit << " particles." << std::endl;
    M_log("Mode%d executed in %ld microseconds", g_config.mode, g_data.duration.count());
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

/*
 * Execute a function in a synchronized manner across all MPI processes.
 * Ensures that the function is executed by one process at a time, in the order of their ranks.
 */
void synchronized_execution(std::function<void()> func_to_execute) {
    int dummy = 0;
    if (g_config.world_rank == 0) {
        /* Process 0 executes first */
        func_to_execute();
        /* Send a message to the next process to let it execute */
        MPI_Send(&dummy, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
    } else {
        /* Wait for a message from the previous process before executing */
        MPI_Recv(&dummy, 1, MPI_INT, g_config.world_rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        func_to_execute();
        /* Send a message to the next process to let it execute */
        if (g_config.world_rank < g_config.process_count - 1) {
            MPI_Send(&dummy, 1, MPI_INT, g_config.world_rank + 1, 0, MPI_COMM_WORLD);
        }
    }
}

/*
 * Report the results of the computation.
 * Uses different reporting mechanisms based on the mode.
 */
void report_result() {
    MPI_Barrier(MPI_COMM_WORLD);
    if (g_config.mode != 3) {
        report_time();
        printParticleInfo();
    } else {
        synchronized_execution(report_time);
        MPI_Barrier(MPI_COMM_WORLD);
        synchronized_execution(printParticleInfo);
    }
}
