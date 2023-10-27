#pragma once
#include <chrono>
#include <cmath>
#include <deque>
#include <limits>
#include <queue>
#include <vector>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
#include <mpi.h>
#pragma GCC diagnostic pop

#include "particles.h"

struct GlobalConfig {
    size_t thread_count = 1;
    size_t particle_limit = std::numeric_limits<size_t>::max();
    int mode;
    int process_count = 1;
    int world_rank;
    size_t start_pos = 1;  /* Start position for the current process in mode 3 */
    size_t end_pos;    /* End position for the current process in mode 3 */
};

struct GlobalData {
    std::deque<Particle> particle_list;
    std::vector<std::pair<size_t, size_t>> chunk_boundary_map;
    std::vector<ParticleInfo> particle_info_list;
    std::queue<std::vector<Particle>> particle_queue; /* Queue to hold smaller chunks of particles */
    std::chrono::microseconds duration = std::chrono::microseconds(0);
};
