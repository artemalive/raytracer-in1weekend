#include "common.h"
#include "random.h"
#include "vector.h"

Vector random_point_in_unit_disk(RNG& rng) {
    Vector p;
    do {
        p = 2.0f * Vector(rng.random_float(), rng.random_float(), 0.0f) - Vector(1, 1, 0);
    } while (dot_product(p, p) >= 1.0f);
    return p;
}

Vector random_point_in_unit_sphere(RNG& rng) {
    Vector p;
    do {
        p = 2.0f * Vector(rng.random_float(), rng.random_float(), rng.random_float()) - Vector(1, 1, 1);
    } while (dot_product(p, p) >= 1.0f);
    return p;
}

Vector random_cosine_direction(RNG& rng) {
    float r1 = rng.random_float();
    float r2 = rng.random_float();

    float phi = 2 * PI * r1;
    float cos_theta = std::sqrt(1 - r2);
    float sin_theta = std::sqrt(r2);

    float x = sin_theta * std::cos(phi);
    float y = sin_theta * std::sin(phi);
    float z = cos_theta;

    return Vector(x, y, z);
}

void get_tangent_vectors_for_direction(const Vector& direction, Vector& tangent1, Vector& tangent2) {
    Vector a;
    if (std::abs(direction.x) > 0.9f)
        a = Vector(0, 1, 0);
    else
        a = Vector(1, 0, 0);

    tangent1 = cross_product(direction, a).normalized();
    tangent2 = cross_product(direction, tangent1);
}
