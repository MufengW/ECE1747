#pragma once
#include <iostream>
#include <functional>
#include <vector>
#include <thread>
#include "global.h"

extern GlobalConfig g_config;
extern GlobalData g_data;

void parallel_threading();
void parallel_processing();
#ifdef USE_MPI
void synchronized_execution(std::function<void()> func_to_execute);
#endif
