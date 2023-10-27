#pragma once

const double q = 1.6e-19;
const double k = 8.99e9;
const double angstromToMeter = 1e-10;

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