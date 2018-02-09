#pragma once

#include "vec3.h"

class Ray
{
public:
    Ray() {}

    Ray(const Vector& origin, const Vector& direction)
        : origin(origin), direction(direction) {}

    Vector PointAtParameter(float t) const { return origin + t * direction; }
    
    Vector origin;
    Vector direction;
};
