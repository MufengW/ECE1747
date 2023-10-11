#include "log.h"
#include "charge_points.h"
#include "data_processor.h"

std::vector<std::string> linesArray;
std::vector<std::pair<size_t, size_t>> chunk_boundaries;

void print_chunk_boundaries() {
    std::cout << "Chunk Boundaries:" << std::endl;
    for (size_t i = 0; i < chunk_boundaries.size(); ++i) {
        std::cout << "Chunk " << (i + 1) << ": Start = " << chunk_boundaries[i].first
                  << ", End = " << chunk_boundaries[i].second << std::endl;
    }
}

int main(int argc, char** argv) {
    M_assert(argc == 3, "Usage: %s <mode> <thread_count>", argv[0]);

    int mode = std::atoi(argv[1]);
    size_t thread_count = 1;
    M_assert(mode <= 3 && mode >= 1, "mode has to be an integer between 1 to 3!");
    if (mode != 1) {
        thread_count = std::atoi(argv[2]);
        M_log("thread_count = %d", thread_count);
    } else {
        M_log("thread_count set to 1 by default under mode 1.");
    }
    M_assert(thread_count > 0, "Thread count must be greater than 0!");

    /* Load the entire file into data_buffer */
    std::string data_buffer;
    // std::ifstream file("data/particles-student-1.csv");
    std::ifstream file("data/small.csv");
    std::stringstream ss;
    ss << file.rdbuf();
    data_buffer = ss.str();
    file.close();
    populate_linesArray(data_buffer);
    set_chunk_boundaries(thread_count);
    print_chunk_boundaries();

    // std::vector<ChargePoint> points = load_data("data/particles-student-1.csv", thread_count);

    switch (mode) {
        case 1:
            std::cout << "Running in mode 1" << std::endl;
            compute_and_print_force(chunk_boundaries[0]);
            break;

        case 2:
            std::cout << "Running in mode 2" << std::endl;
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

