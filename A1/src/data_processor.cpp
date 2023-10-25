#include "data_processor.h"
#include <chrono>

void populate_dataArray(const std::string& data_buffer);
void set_chunk_boundaries();
void populate_particle_queue();
void print_chunk_boundaries();
void print_particle_queue();

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
    if (g_config.mode == 3){
        populate_particle_queue();
    }

#ifdef DEBUG
    print_chunk_boundaries();
    print_particle_queue();
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
        particle.id = line_count;
        g_data.particleVector.push_back(particle);
        ++line_count;
    }

    if (g_data.particleVector.size() <= 1) {
        throw std::runtime_error("There is only 1 particle!");
    }
}

/* Helper function to update chunk boundaries */
void set_chunk_boundaries() {
    size_t start_pos, end_pos;  /* Start and end positions for the current computational unit */
    size_t num_units = (g_config.mode != 3) ? g_config.thread_count : g_config.process_count;

    /* Common logic for both modes */
    size_t avg_lines_per_chunk = g_data.particleVector.size() / num_units;
    size_t extra_lines = g_data.particleVector.size() % num_units;

    if (g_config.mode != 3) {
        /* Existing logic to populate g_data.chunk_boundaries for mode 2 */
        g_data.particles.resize(g_config.particle_limit);
        g_data.chunk_boundaries.clear();

        start_pos = 0;
        for (size_t i = 0; i < num_units; ++i) {
            size_t lines_for_this_chunk = avg_lines_per_chunk;
            if (extra_lines > 0) {
                lines_for_this_chunk++;
                extra_lines--;
            }

            end_pos = start_pos + lines_for_this_chunk;
            g_data.chunk_boundaries.push_back({start_pos, end_pos});
            start_pos = end_pos;
        }
    } else {
        /* Get the rank of the process */
        MPI_Comm_rank(MPI_COMM_WORLD, &g_config.world_rank);
        /* Logic to find start_pos and end_pos for the current process */
        g_config.start_pos = g_config.world_rank * avg_lines_per_chunk;
        g_config.end_pos = g_config.start_pos + avg_lines_per_chunk;

        /* Distribute extra lines */
        if (g_config.world_rank < static_cast<int>(extra_lines)) {
            g_config.start_pos += g_config.world_rank;
            g_config.end_pos = g_config.start_pos + avg_lines_per_chunk + 1;
        } else {
            g_config.start_pos += extra_lines;
            g_config.end_pos = g_config.start_pos + avg_lines_per_chunk;
        }
        g_config.start_pos += 1;
        g_config.end_pos += 1;
        g_data.particles.resize(g_config.end_pos - g_config.start_pos);
    }

    /* Pad "max" particles for easy computing */
    Particle max_p = Particle(-1, std::numeric_limits<int>::max(), std::numeric_limits<int>::max());
    g_data.particleVector.push_front(max_p);
    g_data.particleVector.push_back(max_p);
}

void populate_particle_queue() {
    size_t fixed_chunk_size = 1024;
    size_t k = std::floor((g_config.end_pos - g_config.start_pos) / fixed_chunk_size);
    size_t remainder = (g_config.end_pos - g_config.start_pos) % fixed_chunk_size;
    if (remainder > 0) {
        k++;
    }

    for (size_t i = 1; i <= k; ++i) {
        size_t start = g_config.start_pos + (i - 1) * fixed_chunk_size;
        size_t end = std::min(start + fixed_chunk_size - 1, g_config.end_pos - 1);

        size_t overlap_start = start - 1;
        size_t overlap_end = end + 1;

        std::vector<Particle> sub_chunk(g_data.particleVector.begin() + overlap_start,
            g_data.particleVector.begin() + overlap_end + 1);
        g_data.particleQueue.push(sub_chunk);
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

void print_particle_queue() {
    /* Check if the mode is not 3, then return */
    if (g_config.mode != 3) return;

    /* Print the entire particleVector first from start_pos to end_pos */
    std::cout << "\nEntire particleVector for process" << g_config.world_rank << ":\n";
    for (size_t i = g_config.start_pos; i < g_config.end_pos; ++i) {
        std::cout << "(" << g_data.particleVector[i].x << ", " << g_data.particleVector[i].y << ") ";
    }
    std::cout << std::endl;

    /* Create a temporary copy of the queue */
    std::queue<std::vector<Particle>> tempQueue = g_data.particleQueue;

    /* Now print each chunk */
    int chunk_count = 1;
    std::cout << "Individual chunks for process" << g_config.world_rank << ":\n";
    while (!tempQueue.empty()) {
        std::vector<Particle> current_chunk = tempQueue.front();
        tempQueue.pop();

        std::cout << "Chunk " << chunk_count << ":\n";

        for (const auto& particle : current_chunk) {
            std::cout << "(" << particle.x << ", " << particle.y << ") ";
        }

        std::cout << std::endl;
        chunk_count++;
    }
    std::cout << std::endl;
}
