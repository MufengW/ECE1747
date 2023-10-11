#pragma once
#include <limits>
#include <vector>
#include "log.h"
const double q = 1.6e-19;
const double k = 8.99e9;
const double angstromToMeter = 1e-10;

extern std::vector<std::string> linesArray;
extern std::vector<std::pair<size_t, size_t>> chunk_boundaries;

struct Particle {
    int x;
    int y;
    Particle() : x(0.0), y(0.0) {}
    Particle(int x_val, int y_val) : x(x_val), y(y_val) {}
};

struct ParticleInfo : Particle {
    double force;
    ParticleInfo() : Particle(), force(0.0) {}
    ParticleInfo(Particle p, double f_val) : Particle(p.x, p.y), force(f_val) {}
};
extern std::vector<ParticleInfo> particles;
void compute_and_print_force(std::pair<size_t, size_t> boundary, int thread_id);
void printParticles(const std::vector<ParticleInfo>& particles);
