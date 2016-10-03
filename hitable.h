#pragma once

#include "ray.h"

struct HitRecord
{
    float t;
    Vector p;
    Vector normal;
};

class Hitable
{
public:
    virtual bool Hit(const Ray& ray, float tMin, float tMax, HitRecord& hitRecord) const = 0;
};
