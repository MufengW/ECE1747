#pragma once
#include <limits>
#include <vector>
#include "log.h"
const double q = 1.6e-19;
const double k = 8.99e9;
const double angstromToMeter = 1e-10;

extern std::vector<std::string> linesArray;
extern std::vector<std::pair<size_t, size_t>> chunk_boundaries;

struct ChargePoint {
    int x;
    int y;
    ChargePoint() : x(0.0), y(0.0) {}
    ChargePoint(int x_val, int y_val) : x(x_val), y(y_val) {}
};

void compute_and_print_force(std::pair<size_t, size_t> boundary);
