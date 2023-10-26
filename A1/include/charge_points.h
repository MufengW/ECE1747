#pragma once
#include <limits>
#include <deque>
#include <string>
#include "global.h"

const double q = 1.6e-19;
const double k = 8.99e9;
const double angstromToMeter = 1e-10;

extern GlobalConfig g_config;
extern GlobalData g_data;

Particle parseLineToChargePoint(const std::string& line);
void computeAndStoreForce(const std::vector<Particle>& sub_chunk);
void printParticleInfo();
