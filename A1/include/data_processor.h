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
void load_data(const std::string filen_name);
