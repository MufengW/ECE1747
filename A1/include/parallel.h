#pragma once
#include <iostream>
#include <vector>
#include <thread>

extern size_t g_thread_count;
extern std::vector<std::pair<size_t, size_t>> g_chunk_boundaries;

void parallel_processing();
