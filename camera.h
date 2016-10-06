#pragma once

#include "random.h"
#include "ray.h"

static const float PI = 3.14159265358979323846f;

Vector RandomInUnitDisk()
{
    Vector p;
    do
    {
        p = 2.0f * Vector(RandomFloat(), RandomFloat(), 0.0f) - Vector(1, 1, 0);
    } while (Dot(p, p) >= 1.0f);
    return p;
}

class Camera
{
public:
    Camera(Vector lookFrom, Vector lookAt, Vector up, float vfov, float aspect, float aperture, float focusDist)
    {
        lensRadius = aperture / 2.0f;

        float theta = vfov * PI / 180.0f;
        float halfHeight = std::tan(theta / 2.0f);
        float halfWidth = aspect * halfHeight;

        origin = lookFrom;
        w = UnitVector(lookFrom - lookAt);
        u = UnitVector(Cross(up, w));
        v = Cross(w, u);

        lowerLeftCorner = origin - halfWidth*focusDist*u - halfHeight*focusDist*v - focusDist*w;
        horizontal = 2.0f*halfWidth*focusDist*u;
        vertical = 2.0f*halfHeight*focusDist*v;
    }

    Ray GetRay(float s, float t)
    {
        Vector rd = lensRadius*RandomInUnitDisk();
        Vector offset = u*rd.X() + v*rd.Y();
        return Ray(origin + offset, lowerLeftCorner + s*horizontal + t*vertical - origin - offset);
    }

    Vector origin;
    Vector lowerLeftCorner;
    Vector horizontal;
    Vector vertical;
    Vector u, v, w;
    float lensRadius;
};
