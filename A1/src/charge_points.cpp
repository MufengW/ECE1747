#include "charge_points.h"
#include "log.h"
#include <cmath>
#include <thread>
#include <sstream>
double distance(const Particle& p1, const Particle& p2) {
    double dx = (p1.x - p2.x) * angstromToMeter;
    double dy = (p1.y - p2.y) * angstromToMeter;
    return std::sqrt(std::pow(dx,2) + std::pow(dy,2));
}

double compute_force(const Particle& p, const Particle& p_prev, const Particle& p_next) {
    double dist = std::min(distance(p, p_prev), distance(p, p_next));
    return k * std::pow(q,2) / std::pow(dist,2);
}

Particle parseLineToChargePoint(const std::string& line) {
    std::stringstream ss(line);
    std::string token;
    std::vector<std::string> tokens;

    while (std::getline(ss, token, ',')) {
        tokens.push_back(token);
    }

    /* Ensure the line has the correct format: "double,double,char" */
    M_assert(tokens.size() == 3, "Line format is incorrect.");
    M_assert(!tokens[0].empty() && !tokens[1].empty() && tokens[2].size() == 1,
        "Line content does not match expected format.");

    int x_val = std::stod(tokens[0]);
    int y_val = std::stod(tokens[1]);
    return Particle(x_val, y_val);
}

void print_result(const Particle& p, double force, int thread_id) {
    char output_buffer[256];
    snprintf(output_buffer, sizeof(output_buffer),
             "Thread ID: %d, %d %d force = %e",
             thread_id,
             p.x,
             p.y,
             force);

    M_log(output_buffer);
}

void compute_and_print_force(std::pair<size_t, size_t> boundary, int thread_id) {
    Particle p, p_prev, p_next;
    size_t start = boundary.first;
    size_t end = boundary.second;
    for (size_t i = start + 1; i < end + 1; ++i) {
        p = g_dataArray[i];
        p_prev = g_dataArray[i-1];
        p_next = g_dataArray[i+1];
        double force = compute_force(p, p_prev, p_next);
        g_particles[i - 1] = ParticleInfo(p, force);
#ifdef DEBUG_MODE
        print_result(p,force, thread_id);
#endif
    }
}

void printParticles() {
    std::cout << "Printing particle information:\n";
    for (size_t i = 0; i < g_particles.size(); ++i) {
        std::cout << "Particle " << i + 1 << ": "
                  << "x = " << g_particles[i].x << ", "
                  << "y = " << g_particles[i].y << ", "
                  << "force = " << g_particles[i].force << "\n";
    }
}
