#pragma once
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "log.h"
#include "charge_points.h"

extern size_t g_thread_count;
extern size_t g_particle_limit;
void load_data(const std::string filen_name);
