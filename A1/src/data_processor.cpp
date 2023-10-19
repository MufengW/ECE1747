#include "data_processor.h"
#include <chrono>

void populate_dataArray(const std::string& data_buffer);
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
    g_config.thread_count = std::min(g_config.thread_count, line_count);
    g_config.particle_limit = std::min(g_config.particle_limit, line_count);

    populate_dataArray(data_buffer);

    set_chunk_boundaries();
#ifdef DEBUG
    print_chunk_boundaries();
#endif
}

void populate_dataArray(const std::string& data_buffer) {
    std::stringstream ss(data_buffer);
    std::string line;
    size_t line_count = 0;

    /* Remove trailing \r character if present */
    while (std::getline(ss, line) && line_count < g_config.particle_limit) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        Particle particle = parseLineToChargePoint(line);
        g_data.particleVector.push_back(particle);
        ++line_count;
    }

    if (g_data.particleVector.size() <= 1) {
        throw std::runtime_error("There is only 1 particle!");
    }
}

/* Helper function to update chunk boundaries */
void set_chunk_boundaries() {
    g_data.particles.resize(g_config.particle_limit);
    g_data.chunk_boundaries.clear();

    size_t avg_lines_per_chunk = g_data.particleVector.size() / g_config.thread_count;
    size_t extra_lines = g_data.particleVector.size() % g_config.thread_count;

    size_t start_pos = 0;
    for (size_t i = 0; i < g_config.thread_count; ++i) {
        size_t lines_for_this_chunk = avg_lines_per_chunk;
        if (extra_lines > 0) {
            lines_for_this_chunk++;
            extra_lines--;
        }

        size_t end_pos = start_pos + lines_for_this_chunk;

        g_data.chunk_boundaries.push_back({start_pos, end_pos});
        start_pos = end_pos;
    }
    if(g_config.mode != 3) {
        /* Pad inf particle at beginning and end for easy computing */
        Particle max_p = Particle(std::numeric_limits<int>::max(), std::numeric_limits<int>::max());
        g_data.particleVector.push_front(max_p);
        g_data.particleVector.push_back(max_p);
    }
}

void print_chunk_boundaries() {
    std::string log = "Chunk Boundaries:\n";
    for (size_t i = 0; i < g_data.chunk_boundaries.size(); ++i) {
        log += "Chunk " + std::to_string(i) + ": Start = " +
                      std::to_string(g_data.chunk_boundaries[i].first) + ", End = " +
                      std::to_string(g_data.chunk_boundaries[i].second) + "\n";
    }
    M_log(log.c_str());
}
