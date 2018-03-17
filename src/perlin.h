#pragma once

#include "../random.h"
#include "../vector.h"

void perlin_initialize(RNG& rng);
float perlin_noise(const Vector& p);
float perlin_turbulence(const Vector& p, int depth = 7);
