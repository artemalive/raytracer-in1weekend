#pragma once

#include "vector.h"

class Ray
{
public:
    Ray() {}

    Ray(const Vector& origin, const Vector& direction, float time = 0.f)
        : origin(origin), direction(direction), time(time) {}

    Vector PointAtParameter(float t) const { return origin + t * direction; }
    
    Vector origin;
    Vector direction;
    float time;
};
