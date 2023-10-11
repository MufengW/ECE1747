#pragma once
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "log.h"
#include "charge_points.h"

extern std::vector<std::string> linesArray;
extern std::vector<std::pair<size_t, size_t>> chunk_boundaries;
void load_data(const std::string filen_name, size_t thread_count);
void populate_linesArray(const std::string& data_buffer);
void set_chunk_boundaries(size_t num_threads);
