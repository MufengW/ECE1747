#pragma once
#include <queue>
#include <deque>
#include <vector>
#include <limits>
#include <cmath>
#include <chrono>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"

#include <mpi.h>

#pragma GCC diagnostic pop


/* Forward declarations */
struct Particle;
struct ParticleInfo;

struct GlobalConfig {
    size_t thread_count = 1;
    size_t particle_limit = std::numeric_limits<size_t>::max();
    int mode;
    int process_count = 1;
    int world_rank;
    size_t start_pos;  /* Start position for the current process in mode 3 */
    size_t end_pos;    /* End position for the current process in mode 3 */
};

struct GlobalData {
    std::deque<Particle> particleVector;
    std::vector<std::pair<size_t, size_t>> chunk_boundaries;
    std::vector<ParticleInfo> particles;
    std::queue<std::vector<Particle>> particleQueue; /* Queue to hold smaller chunks of particles */
    std::chrono::microseconds duration = std::chrono::microseconds(0);
};
