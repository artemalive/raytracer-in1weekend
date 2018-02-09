#pragma once

#include "hitable.h"

class Sphere : public Hitable
{
public:
    Sphere() {}
    Sphere(const Vector& center, float radius, Material* material)
        : center(center), radius(radius), material(material) {}

    bool Hit(const Ray& ray, float tMin, float tMax, HitRecord& hitRecord) const override;

    Vector center;
    float radius;
    Material* material;
};

inline bool Sphere::Hit(const Ray& ray, float tMin, float tMax, HitRecord& hitRecord) const
{
    Vector oc = ray.origin - center;
    float b = Dot(oc, ray.direction);
    float c = Dot(oc, oc) - radius*radius;

    float discriminant = b*b - c;
    if (discriminant <= 0.0f)
        return false;

    float t = -b - sqrt(discriminant);
    if (t <= tMin || t >= tMax)
    {
        t = -b + sqrt(discriminant);
        if (t <= tMin || t >= tMax)
            return false;
    }

    hitRecord.t = t;
    hitRecord.p = ray.PointAtParameter(t);
    hitRecord.normal = (hitRecord.p - center) / radius;
    hitRecord.material = material;
    return true;
}
