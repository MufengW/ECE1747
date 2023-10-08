#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "charge_points.h"

const double angstromToMeter = 1e-10;

std::vector<ChargePoint> load_data(const std::string& filename);
void compute_and_print_force(std::vector<ChargePoint>& points);
