#include <mutex>

#include "parallel.h"
#include "particles.h"
#include "computation.h"
#include "log.h"

std::mutex queue_mutex;  /* Mutex for synchronizing access to the queue */

void parallel_threading() {
    std::vector<std::thread> threads;
    size_t chunk_size = g_data.chunk_boundary_map.size();
    M_assert(g_config.thread_count == chunk_size,
        "Mismatch on thread number (%ld) and chunk size (%ld)!", g_config.thread_count, chunk_size);
    for (size_t i = 0; i < chunk_size; ++i) {
        std::pair<size_t, size_t> boundary = g_data.chunk_boundary_map[i];
        size_t start = boundary.first;
        size_t end = boundary.second;
        std::vector<Particle> sub_chunk(g_data.particle_list.begin() + start, g_data.particle_list.begin() + end + 2);
        threads.push_back(std::thread(batchComputeAndStoreParticleForces, sub_chunk));
    }

    /* Join all threads */
    for (auto& t : threads) {
        t.join();
    }
}

#ifdef USE_MPI
void worker_thread() {
    while (true) {
        std::vector<Particle> current_sub_chunk;

        /* Critical section: Dequeue a sub-chunk */
        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            if (g_data.particle_queue.empty()) {
                break;  /* No more work to do */
            }
            current_sub_chunk = g_data.particle_queue.front();
            g_data.particle_queue.pop();
        }

        /* Process the sub-chunk */
        batchComputeAndStoreParticleForces(current_sub_chunk);
    }
}

void parallel_processing() {
    std::vector<std::thread> threads;

    /* Create worker threads */
    for (size_t i = 0; i < g_config.thread_count; ++i) {
        threads.push_back(std::thread(worker_thread));
    }

    /* Join all threads */
    for (auto& t : threads) {
        t.join();
    }
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
#endif
