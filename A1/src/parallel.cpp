#include <mutex>

#include "parallel.h"
#include "charge_points.h"
#include "log.h"

std::mutex queue_mutex;  /* Mutex for synchronizing access to the queue */

void parallel_threading() {
    std::vector<std::thread> threads;
    size_t chunk_size = g_data.chunk_boundaries.size();
    M_assert(g_config.thread_count == chunk_size,
        "Mismatch on thread number (%ld) and chunk size (%ld)!", g_config.thread_count, chunk_size);
    for (size_t i = 0; i < chunk_size; ++i) {
        threads.push_back(std::thread(compute_and_print_force, g_data.chunk_boundaries[i], static_cast<int>(i)));
    }

    /* Join all threads */
    for (auto& t : threads) {
        t.join();
    }
}


void worker_thread(int thread_id) {
    while (true) {
        std::vector<Particle> current_sub_chunk;

        /* Critical section: Dequeue a sub-chunk */
        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            if (g_data.particleQueue.empty()) {
                break;  /* No more work to do */
            }
            current_sub_chunk = g_data.particleQueue.front();
            g_data.particleQueue.pop();
        }

        /* Process the sub-chunk */
        compute_and_print_force2(current_sub_chunk, thread_id);
    }
}

void parallel_processing() {
    std::vector<std::thread> threads;

    /* Create worker threads */
    for (int i = 0; i < g_config.thread_count; ++i) {
        threads.push_back(std::thread(worker_thread, i));
    }

    /* Join all threads */
    for (auto& t : threads) {
        t.join();
    }
}
