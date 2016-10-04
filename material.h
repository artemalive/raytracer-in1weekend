#pragma once

#include "hitable.h"

class Material
{
public:
    virtual bool Scatter(const Ray& ray, const HitRecord& hitRecord,
        Vector& attenuation, Ray& scatteredRay) const = 0;
};
