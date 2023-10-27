#pragma once
#include "global.h"

extern GlobalConfig g_config;
extern GlobalData g_data;

/* Load data from a file and populate the global data structures */
void loadData(const std::string file_name);
