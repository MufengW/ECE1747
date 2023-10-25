#include "global.h"
#include "log.h"
#include "data_processor.h"
#include "charge_points.h"
#include "parallel.h"
#include <functional>

GlobalConfig g_config;
GlobalData g_data;

void report_result();

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
    MPI_Comm_size(MPI_COMM_WORLD, &g_config.process_count);
    if (g_config.process_count > 1 && mode != 3) {
        MPI_Finalize();
        M_assert(mode == 3 , "Only mode 3 supports MPI!");
    }
    const std::string file_name = "data/huge.csv";
    load_data(file_name);

    auto start = std::chrono::high_resolution_clock::now();
    switch (g_config.mode) {
        case 1: {
            M_log("Running in mode 1");
            compute_and_print_force(g_data.chunk_boundaries[0], 1);
            break;
        }

        case 2: {
            M_log("Running in mode 2");
            parallel_threading();
            break;
        }

        case 3: {
            M_log("Running in mode 3");
            parallel_processing();
            break;
        }
        default: {
            M_log("Invalid mode specified: %d", mode);
            return 1;
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    g_data.duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    report_result();
    /* Finalize the MPI environment */
    MPI_Finalize();
    return 0;
}

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

void report_result() {
    MPI_Barrier(MPI_COMM_WORLD);
    if (g_config.mode != 3) {
        report_time();
        printParticles();
    } else {
        synchronized_execution(report_time);
        MPI_Barrier(MPI_COMM_WORLD);
        synchronized_execution(printParticles);
    }
}
