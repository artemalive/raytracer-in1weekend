#include "random.h"
#include "vector.h"

Vector random_point_in_unit_disk(RNG& rng) {
    Vector p;
    do {
        p = 2.0f * Vector(rng.random_float(), rng.random_float(), 0.0f) - Vector(1, 1, 0);
    } while (dot_product(p, p) >= 1.0f);
    return p;
}

Vector random_point_in_unit_sphere(RNG& rng)
{
    Vector p;
    do {
        p = 2.0f * Vector(rng.random_float(), rng.random_float(), rng.random_float()) - Vector(1, 1, 1);
    } while (dot_product(p, p) >= 1.0f);
    return p;
}
