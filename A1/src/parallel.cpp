#include "parallel.h"
#include "charge_points.h"
#include "log.h"

void parallel_processing() {
    std::vector<std::thread> threads;
    size_t chunk_size = g_chunk_boundaries.size();
    M_assert(g_thread_count == chunk_size,
        "Mismatch on thread number (%ld) and chunk size (%ld)!", g_thread_count, chunk_size);
    for (size_t i = 0; i < chunk_size; ++i) {
        threads.push_back(std::thread(compute_and_print_force, g_chunk_boundaries[i], static_cast<int>(i)));
    }

    /* Join all threads */
    for (auto& t : threads) {
        t.join();
    }
}
