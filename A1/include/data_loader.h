#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <limits>
#include "charge_points.h"

std::vector<ChargePoint> load_data(const std::string& filename);
void set_neighbour_distances(std::vector<ChargePoint>& points);
