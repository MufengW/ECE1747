#pragma once
#include "global.h"

extern GlobalConfig g_config;
extern GlobalData g_data;

void batchComputeAndStoreParticleForces(const std::vector<Particle>& sub_chunk);
