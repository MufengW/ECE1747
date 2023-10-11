#include "data_processor.h"

void print_chunk_boundaries() {
    std::cout << "Chunk Boundaries:" << std::endl;
    for (size_t i = 0; i < chunk_boundaries.size(); ++i) {
        std::cout << "Chunk " << (i + 1) << ": Start = " << chunk_boundaries[i].first
                  << ", End = " << chunk_boundaries[i].second << std::endl;
    }
}

void print_linesArray() {
    for (const std::string& line : linesArray) {
        std::cout << line << std::endl;
    }
}

void load_data(const std::string file_name, size_t thread_count) {
    /* Load the entire file into data_buffer */
    std::string data_buffer;
    std::ifstream file(file_name);
    std::stringstream ss;
    ss << file.rdbuf();
    data_buffer = ss.str();
    file.close();
    populate_linesArray(data_buffer);
    set_chunk_boundaries(thread_count);
#ifndef NDEBUG
    print_chunk_boundaries();
    print_linesArray();
#endif
}

void populate_linesArray(const std::string& data_buffer) {
    std::stringstream ss(data_buffer);
    std::string line;
    while (std::getline(ss, line)) {
        /* Remove trailing \r character if present */
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        linesArray.push_back(line);
    }
    M_assert(linesArray.size() > 1, "There is only 1 partical!");
}

/* Helper function to update chunk boundaries */
void set_chunk_boundaries(size_t num_threads) {
    chunk_boundaries.clear();  /* Clear any previous boundaries */

    /* Determine the average number of lines per chunk */
    size_t avg_lines_per_chunk = linesArray.size() / num_threads;
    size_t extra_lines = linesArray.size() % num_threads;

    size_t start_pos = 0;
    for (size_t i = 0; i < num_threads; ++i) {
        size_t lines_for_this_chunk = avg_lines_per_chunk;
        if (extra_lines > 0) {
            lines_for_this_chunk++;
            extra_lines--;
        }

        size_t end_pos = start_pos + lines_for_this_chunk;

        chunk_boundaries.push_back({start_pos, end_pos});
        start_pos = end_pos;
    }
}
