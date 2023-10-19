#pragma once
#include <deque>
#include <vector>
#include <limits>


/* Forward declarations */
struct Particle;
struct ParticleInfo;

struct GlobalConfig {
    size_t thread_count = 1;
    size_t particle_limit = std::numeric_limits<size_t>::max();
    int mode;
    int process_count = 1;
    int world_rank;
};

struct GlobalData {
    std::deque<Particle> particleVector;
    std::vector<std::pair<size_t, size_t>> chunk_boundaries;
    std::vector<ParticleInfo> particles;
};
