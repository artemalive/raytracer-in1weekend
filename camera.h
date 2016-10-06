#pragma once

#include "ray.h"

static const float PI = 3.14159265358979323846f;

class Camera
{
public:
    Camera(Vector lookFrom, Vector lookAt, Vector up, float vfov, float aspect)
    {
        Vector u, v, w;

        float theta = vfov * PI / 180.0f;
        float halfHeight = std::tan(theta / 2.0f);
        float halfWidth = aspect * halfHeight;

        origin = lookFrom;
        w = UnitVector(lookFrom - lookAt);
        u = UnitVector(Cross(up, w));
        v = Cross(w, u);

        lowerLeftCorner = origin - halfWidth*u - halfHeight*v - w;
        horizontal = 2.0f*halfWidth*u;
        vertical = 2.0f*halfHeight*v;
    }

    Ray GetRay(float u, float v) { return Ray(origin, lowerLeftCorner + u*horizontal + v*vertical - origin); }

    Vector origin;
    Vector lowerLeftCorner;
    Vector horizontal;
    Vector vertical;
};
