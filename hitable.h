#pragma once

#include "ray.h"

class Material;

struct HitRecord
{
    float t;
    Vector p;
    Vector normal;
    Material* material;
};

class Hitable
{
public:
    virtual bool Hit(const Ray& ray, float tMin, float tMax, HitRecord& hitRecord) const = 0;
};
