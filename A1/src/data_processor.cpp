#include "data_processor.h"

void populate_linesArray(const std::string& data_buffer);
void set_chunk_boundaries();
void print_chunk_boundaries();

void load_data(const std::string file_name) {
    /* Load the entire file into data_buffer */
    std::string data_buffer;
    std::ifstream file(file_name);
    std::stringstream ss;
    ss << file.rdbuf();
    data_buffer = ss.str();
    file.close();
    size_t line_count = std::count(data_buffer.begin(), data_buffer.end(), '\n');
    thread_count = std::min(thread_count, line_count);
    particle_limit = std::min(particle_limit, line_count);
    populate_linesArray(data_buffer);
    set_chunk_boundaries();
    print_chunk_boundaries();
}

void populate_linesArray(const std::string& data_buffer) {
    std::stringstream ss(data_buffer);
    std::string line;
    size_t line_count = 0;
    while (std::getline(ss, line) && line_count < particle_limit) {
        /* Remove trailing \r character if present */
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        linesArray.push_back(line);
        ++line_count;
    }
    M_assert(linesArray.size() > 1, "There is only 1 particle!");
}

/* Helper function to update chunk boundaries */
void set_chunk_boundaries() {
    particles.resize(particle_limit);
    chunk_boundaries.clear();

    size_t avg_lines_per_chunk = linesArray.size() / thread_count;
    size_t extra_lines = linesArray.size() % thread_count;

    size_t start_pos = 0;
    for (size_t i = 0; i < thread_count; ++i) {
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

void print_chunk_boundaries() {
    std::string log = "Chunk Boundaries:\n";
    for (size_t i = 0; i < chunk_boundaries.size(); ++i) {
        log += "Chunk " + std::to_string(i) + ": Start = " +
                      std::to_string(chunk_boundaries[i].first) + ", End = " +
                      std::to_string(chunk_boundaries[i].second) + "\n";
    }
    M_log(log.c_str());
}
