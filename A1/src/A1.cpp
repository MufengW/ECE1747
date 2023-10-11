#include "log.h"
#include "charge_points.h"
#include "data_processor.h"
#include <thread>

std::vector<std::string> linesArray;
std::vector<std::pair<size_t, size_t>> chunk_boundaries;
size_t thread_count = 1;

void parallel_processing(const std::vector<std::pair<size_t, size_t>>& chunk_boundaries) {
    std::vector<std::thread> threads;
    size_t chunk_size = chunk_boundaries.size();
    M_assert(thread_count == chunk_size,
        "Mismatch on thread number (%ld) and chunk size (%ld)!", thread_count, chunk_size);
    for (size_t i = 0; i < chunk_size; ++i) {
        threads.push_back(std::thread(compute_and_print_force, chunk_boundaries[i]));
    }

    /* Join all threads */
    for (auto& t : threads) {
        t.join();
    }
}

int main(int argc, char** argv) {
    M_assert(argc == 3, "Usage: %s <mode> <thread_count>", argv[0]);

    int mode = std::atoi(argv[1]);
    M_assert(mode <= 3 && mode >= 1, "mode has to be an integer between 1 to 3!");
    if (mode != 1) {
        thread_count = std::atoi(argv[2]);
        M_log("thread_count = %ld", thread_count);
    } else {
        M_log("thread_count set to 1 by default under mode 1.");
    }
    M_assert(thread_count > 0, "Thread count must be greater than 0!");

    const std::string file_name = "data/small.csv";
    // const std::string file_name = "data/particles-student-1.csv";
    load_data(file_name, thread_count);
    switch (mode) {
        case 1:
            std::cout << "Running in mode 1" << std::endl;
            compute_and_print_force(chunk_boundaries[0]);
            break;

        case 2:
            std::cout << "Running in mode 2" << std::endl;
            parallel_processing(chunk_boundaries);
            break;

        case 3:
            std::cout << "Running in mode 3" << std::endl;
            break;
        default:
            std::cerr << "Invalid mode specified: " << mode << std::endl;
            return 1;
    }

    return 0;
}
