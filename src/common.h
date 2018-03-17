#pragma once

#include "vector.h"

class RNG;

constexpr float PI = 3.14159265358979323846f;

inline float clamp(float value, float min, float max) {
    if (value < min)
        return min;
    else if (value > max)
        return max;
    else
        return value;
}

Vector random_point_in_unit_disk(RNG& rng);
Vector random_point_in_unit_sphere(RNG& rng);