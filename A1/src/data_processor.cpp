#include "data_processor.h"
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include "log.h"
#include "particles.h"

/* Populate the particle_list from the data buffer */
void populateParticleList(const std::string& data_buffer);

/* Set the boundaries for each chunk of data */
void setChunkBoundaries();

#ifdef USE_MPI
/* Populate the particle queue for mode 3 */
void populateParticleQueue();
#endif

/* Parses a string line to create and return a Particle type */
Particle parseLineToChargePoint(const std::string& line);

/* Load data from a file into the data buffer and then into the particle_list */
void loadData(const std::string file_name) {
    std::string data_buffer;
    std::ifstream file(file_name);
    std::stringstream ss;
    ss << file.rdbuf();
    data_buffer = ss.str();
    file.close();

    /* Determine the minimum between thread_count and actual line_count */
    size_t line_count = std::count(data_buffer.begin(), data_buffer.end(), '\n');
    g_config.thread_count = std::min(g_config.thread_count, line_count);
    g_config.particle_limit = std::min(g_config.particle_limit, line_count);

    populateParticleList(data_buffer);
    setChunkBoundaries();
#ifdef USE_MPI
    /* If in mode 3, populate the particle queue */
    if (g_config.mode == 3){
        populateParticleQueue();
    }
#endif
}

/* Populate the particle_list from the data buffer */
void populateParticleList(const std::string& data_buffer) {
    std::stringstream ss(data_buffer);
    std::string line;
    size_t line_count = 0;

    /* Loop through each line in the data buffer */
    while (std::getline(ss, line) && line_count < g_config.particle_limit) {
        /* Remove any trailing carriage return character */
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        /* Parse the line to create a Particle object and add it to particle_list */
        Particle particle = parseLineToChargePoint(line);
        particle.id = line_count;
        g_data.particle_list.push_back(particle);

        ++line_count;
    }

    if (g_data.particle_list.size() <= 1) {
        throw std::runtime_error("There is only 1 particle!");
    }
}

/* Update the boundaries for chunks of data that will be processed in parallel */
void setChunkBoundaries() {
    size_t start_pos, end_pos;  /* Define the start and end positions for each computational unit */
    size_t num_units = (g_config.mode != 3) ? g_config.thread_count : g_config.process_count;

    /* Calculate the average number of lines per chunk and any extra lines */
    size_t avg_lines_per_chunk = g_data.particle_list.size() / num_units;
    size_t extra_lines = g_data.particle_list.size() % num_units;

    /* Handle mode-specific logic for setting chunk boundaries */
    if (g_config.mode != 3) {
        /* Resize the particles vector based on the particle limit */
        g_data.particle_info_list.resize(g_config.particle_limit);
        /* Clear any existing chunk boundaries */
        g_data.chunk_boundary_map.clear();

        start_pos = 0;
        /* Loop through each computational unit to set its boundaries */
        for (size_t i = 0; i < num_units; ++i) {
            size_t lines_for_this_chunk = avg_lines_per_chunk;
            /* Distribute any extra lines among the chunks */
            if (extra_lines > 0) {
                lines_for_this_chunk++;
                extra_lines--;
            }

            end_pos = start_pos + lines_for_this_chunk;
            /* Add the boundaries to the chunk_boundaries vector */
            g_data.chunk_boundary_map.push_back({start_pos, end_pos});
            start_pos = end_pos;
        }
    } else {
#ifdef USE_MPI
        /* Calculate the start and end positions for this process */
        g_config.start_pos = g_config.world_rank * avg_lines_per_chunk;
        g_config.end_pos = g_config.start_pos + avg_lines_per_chunk;

        /* Distribute any extra lines among the processes */
        if (g_config.world_rank < static_cast<int>(extra_lines)) {
            g_config.start_pos += g_config.world_rank;
            g_config.end_pos = g_config.start_pos + avg_lines_per_chunk + 1;
        } else {
            g_config.start_pos += extra_lines;
            g_config.end_pos = g_config.start_pos + avg_lines_per_chunk;
        }
        g_config.start_pos += 1;
        g_config.end_pos += 1;
        /* Resize the particles vector for this process */
        g_data.particle_info_list.resize(g_config.end_pos - g_config.start_pos);
#endif
    }

    /* Add padding particles to the particle_list for easier computation */
    Particle max_p = Particle(-1, std::numeric_limits<int>::max(), std::numeric_limits<int>::max());
    g_data.particle_list.push_front(max_p);
    g_data.particle_list.push_back(max_p);
}
#ifdef USE_MPI
/* Populate the particle queue with sub-chunks of particles for parallel processing */
void populateParticleQueue() {
    /* Calculate the total number of particles that the current process will handle */
    size_t particle_num = g_config.end_pos - g_config.start_pos;

    /* Define a fixed chunk size, ensuring it does not exceed the total number of particles */
    size_t fixed_chunk_size = std::min(static_cast<size_t>(particle_num), static_cast<size_t>(512));

    /* Calculate the number of fixed-size chunks (k) and any remaining particles */
    size_t k = std::floor(particle_num / fixed_chunk_size);
    size_t remainder = particle_num % fixed_chunk_size;

    /* If there are remaining particles, increment the number of chunks by 1 */
    if (remainder > 0) {
        k++;
    }

    /* Loop through each chunk to populate the particle queue */
    for (size_t i = 1; i <= k; ++i) {
        /* Calculate the start and end positions for the current chunk */
        size_t start = g_config.start_pos + (i - 1) * fixed_chunk_size;
        size_t end = std::min(start + fixed_chunk_size - 1, g_config.end_pos - 1);

        /* Add an overlap of one particle on each side of the chunk */
        size_t overlap_start = start - 1;
        size_t overlap_end = end + 1;

        /* Create a sub-chunk with the overlapping particles */
        std::vector<Particle> sub_chunk(g_data.particle_list.begin() + overlap_start,
            g_data.particle_list.begin() + overlap_end + 1);

        /* Add the sub-chunk to the particle queue */
        g_data.particle_queue.push(sub_chunk);
    }
}
#endif

Particle parseLineToChargePoint(const std::string& line) {
    std::stringstream ss(line);
    std::string token;
    std::vector<std::string> tokens;

    while (std::getline(ss, token, ',')) {
        tokens.push_back(token);
    }

    /* Ensure the line has the correct format: "int,int,char" */
    M_assert(tokens.size() == 3, "Line format is incorrect.");
    M_assert(!tokens[0].empty() && !tokens[1].empty() && tokens[2].size() == 1,
        "Line content does not match expected format.");

    int x_val = std::stod(tokens[0]);
    int y_val = std::stod(tokens[1]);

    /* particle id unset yet. */
    return Particle(-1, x_val, y_val);
}
