#include "log.h"
#include "charge_points.h"
#include "data_processor.h"
#include <thread>
#include <chrono>

std::vector<std::string> linesArray;
std::vector<std::pair<size_t, size_t>> chunk_boundaries;
size_t thread_count = 1;
size_t particle_limit = std::numeric_limits<size_t>::max();
std::vector<ParticleInfo> particles;
void parallel_processing(const std::vector<std::pair<size_t, size_t>>& chunk_boundaries) {
    std::vector<std::thread> threads;
    size_t chunk_size = chunk_boundaries.size();
    M_assert(thread_count == chunk_size,
        "Mismatch on thread number (%ld) and chunk size (%ld)!", thread_count, chunk_size);
    for (size_t i = 0; i < chunk_size; ++i) {
        threads.push_back(std::thread(compute_and_print_force, chunk_boundaries[i], static_cast<int>(i)));
    }

    /* Join all threads */
    for (auto& t : threads) {
        t.join();
    }
}

int main(int argc, char** argv) {
    M_assert(argc == 3 || argc == 4, "Usage: %s <mode> <thread_count> (optional)<particle_limit>", argv[0]);

    int mode = std::atoi(argv[1]);
    M_assert(mode <= 3 && mode >= 1, "mode has to be an integer between 1 to 3!");
    if (mode != 1) {
        thread_count = std::atoi(argv[2]);
        M_log("thread_count = %ld", thread_count);
    } else {
        M_log("thread_count set to 1 by default under mode 1.");
    }
    M_assert(thread_count > 0, "Thread count must be greater than 0!");

    if (argc == 4) {
        particle_limit = std::atoi(argv[3]);
    }

    // const std::string file_name = "data/particles-student-1.csv";
    const std::string file_name = "data/huge.csv";
    load_data(file_name);
    auto start = std::chrono::high_resolution_clock::now();
    switch (mode) {
        case 1:
            M_log("Running in mode 1");
            compute_and_print_force(chunk_boundaries[0], 1);
            break;

        case 2:
            M_log("Running in mode 2");
            parallel_processing(chunk_boundaries);
            break;

        case 3:
            M_log("Running in mode 3");
            break;
        default:
            M_log("Invalid mode specified: %d", mode);
            return 1;
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Mode " << mode << " with " << thread_count
        << " threads executed for " <<  duration.count() << " microseconds." <<std::endl;
    M_log("Mode 1 executed in %ld microseconds", duration.count());
    printParticles(particles);
    return 0;
}
