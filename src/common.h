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
Vector random_cosine_direction(RNG& rng);

void get_tangent_vectors_for_direction(const Vector& direction, Vector& tangent1, Vector& tangent2);

struct Axes {
    Vector e1;
    Vector e2;
    Vector e3;

    Axes(const Vector& direction) {
        e3 = direction;
        get_tangent_vectors_for_direction(direction, e1, e2);
    }

    Vector from_local_to_world(const Vector& v) const {
        return v.x * e1 + v.y * e2 + v.z * e3;
    }
};
