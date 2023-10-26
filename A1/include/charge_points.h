#pragma once
#include <limits>
#include <deque>
#include <string>
#include "global.h"

const double q = 1.6e-19;
const double k = 8.99e9;
const double angstromToMeter = 1e-10;

extern GlobalConfig g_config;
extern GlobalData g_data;

Particle parseLineToChargePoint(const std::string& line);
void compute_and_print_force(std::pair<size_t, size_t> boundary);
void compute_and_print_force2(const std::vector<Particle>& sub_chunk);
void printParticles();
