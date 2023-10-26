#include "charge_points.h"
#include "log.h"
#include <cmath>
#include <thread>
#include <sstream>

double distance(const Particle& p1, const Particle& p2);
double compute_force(const Particle& p, const Particle& p_prev, const Particle& p_next);
void log_result(const Particle& p, double force);

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

void compute_and_print_force(std::pair<size_t, size_t> boundary) {
    Particle p, p_prev, p_next;
    size_t start = boundary.first;
    size_t end = boundary.second;
    for (size_t i = start + 1; i < end + 1; ++i) {
        p = g_data.particleVector[i];
        p_prev = g_data.particleVector[i-1];
        p_next = g_data.particleVector[i+1];
        double force = compute_force(p, p_prev, p_next);
        g_data.particles[p.id] = ParticleInfo(p, force);
#ifdef DEBUG
        log_result(p,force);
#endif
    }
}

void compute_and_print_force2(const std::vector<Particle>& sub_chunk) {
    /* Loop through the sub-chunk, skipping the first and last elements */
    for (size_t i = 1; i < sub_chunk.size() - 1; ++i) {
        Particle p = sub_chunk[i];
        Particle p_prev = sub_chunk[i - 1];
        Particle p_next = sub_chunk[i + 1];

        /* Compute the force */
        double force = compute_force(p, p_prev, p_next);
        int index = p.id + 1 - g_config.start_pos;
        g_data.particles[index] = ParticleInfo(p, force);
#ifdef DEBUG
        log_result(p, force);
#endif
    }
}

void printParticles() {
    for (size_t i = 0; i < g_data.particles.size(); ++i) {
        ParticleInfo p_info = g_data.particles[i];
        std::cout << " Particle " << p_info.id
            << " x = " << p_info.x << ", "
            << "y = " << p_info.y << ", "
            << "force = " << p_info.force << std::endl;
        std::cout.flush();
    }
}

double distance(const Particle& p1, const Particle& p2) {
    double dx = (p1.x - p2.x) * angstromToMeter;
    double dy = (p1.y - p2.y) * angstromToMeter;
    return std::sqrt(std::pow(dx,2) + std::pow(dy,2));
}

double compute_force(const Particle& p, const Particle& p_prev, const Particle& p_next) {
    double dist = std::min(distance(p, p_prev), distance(p, p_next));
    return k * std::pow(q,2) / std::pow(dist,2);
}

void log_result(const Particle& p, double force) {
    char output_buffer[256];
    snprintf(output_buffer, sizeof(output_buffer),
             "%d %d force = %e",
             p.x,
             p.y,
             force);

    M_log(output_buffer);
}
