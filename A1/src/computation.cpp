#include "computation.h"
#include "log.h"

double compute_force(const Particle& p, const Particle& p_prev, const Particle& p_next);
double distance(const Particle& p1, const Particle& p2);

void batchComputeAndStoreParticleForces(const std::vector<Particle>& sub_chunk) {
    /* Loop through the sub-chunk, skipping the first and last elements */
    for (size_t i = 1; i < sub_chunk.size() - 1; ++i) {
        Particle p = sub_chunk[i];
        Particle p_prev = sub_chunk[i - 1];
        Particle p_next = sub_chunk[i + 1];

        /* Compute the force */
        double force = compute_force(p, p_prev, p_next);
        int index = p.id + 1 - g_config.start_pos;
        g_data.particle_info_list[index] = ParticleInfo(p, force);
#ifdef DEBUG
        log_result(p, force);
#endif
    }
}

double compute_force(const Particle& p, const Particle& p_prev, const Particle& p_next) {
    double dist = std::min(distance(p, p_prev), distance(p, p_next));
    return k * std::pow(q,2) / std::pow(dist,2);
}

double distance(const Particle& p1, const Particle& p2) {
    double dx = (p1.x - p2.x) * angstromToMeter;
    double dy = (p1.y - p2.y) * angstromToMeter;
    return std::sqrt(std::pow(dx,2) + std::pow(dy,2));
}
