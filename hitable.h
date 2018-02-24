#pragma once

#include "vector.h"

class Bounding_Box;
class Material;
class Ray;

struct Hit_Record
{
    float t;
    Vector p;
    Vector normal;
    Material* material;
};

class Hitable
{
public:
    virtual bool hit(const Ray& ray, float t_min, float t_max, Hit_Record& hit_record) const = 0;
    virtual Bounding_Box boudning_box(float t0, float t1) const = 0;
};
