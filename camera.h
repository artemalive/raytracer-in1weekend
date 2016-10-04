#pragma once

#include "ray.h"

class Camera
{
public:
    Camera()
        : lowerLeftCorner(-2.0f, -1.0f, -1.0)
        , horizontal(4.0f, 0.0f, 0.0f)
        , vertical(0.0f, 2.0f, 0.0f)
        , origin(0.0f, 0.0f, 0.0f)
    {}

    Ray GetRay(float u, float v) { return Ray(origin, lowerLeftCorner + u*horizontal + v*vertical - origin); }

    Vector origin;
    Vector lowerLeftCorner;
    Vector horizontal;
    Vector vertical;
};
