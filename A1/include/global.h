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


struct Particle {
    int id;
    int x;
    int y;
    Particle() : id(-1), x(0), y(0) {}
    Particle(int id_val, int x_val, int y_val) : id(id_val), x(x_val), y(y_val) {}
};

struct ParticleInfo : Particle {
    double force;
    ParticleInfo() : Particle(), force(0.0) {}
    ParticleInfo(Particle p, double f_val) : Particle(p.id, p.x, p.y), force(f_val) {}
};

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
