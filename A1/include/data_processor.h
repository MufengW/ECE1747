#pragma once
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "global.h"
#include "log.h"
#include "charge_points.h"

extern GlobalConfig g_config;
extern GlobalData g_data;

/* Load data from a file and populate the global data structures */
void load_data(const std::string file_name);
