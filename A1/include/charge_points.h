#pragma once
#include <limits>
#include <vector>
#include "log.h"
extern std::vector<std::string> linesArray;
extern std::vector<std::pair<size_t, size_t>> chunk_boundaries;
struct ChargePoint {
    double x;
    double y;
    ChargePoint() : x(0.0), y(0.0) {}
    ChargePoint(double x_val, double y_val) : x(x_val), y(y_val) {}
};
static double distance(const ChargePoint& p1, const ChargePoint& p2);
double compute_force(const ChargePoint& p1, const ChargePoint& p2);
ChargePoint parseLineToChargePoint(const std::string& line);
const double q = 1.6e-19;
const double k = 8.99e9;
const double angstromToMeter = 1e-10;
