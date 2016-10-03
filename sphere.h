#pragma once

#include "hitable.h"

class Sphere : public Hitable
{
public:
    Sphere() {}
    Sphere(const Vector& center, float radius) : center(center), radius(radius) {}

    bool Hit(const Ray& ray, float tMin, float tMax, HitRecord& hitRecord) const override;

    Vector center;
    float radius;
};

inline bool Sphere::Hit(const Ray& ray, float tMin, float tMax, HitRecord& hitRecord) const
{
    Vector oc = ray.Origin() - center;
    float a = Dot(ray.Direction(), ray.Direction());
    float b = Dot(oc, ray.Direction());
    float c = Dot(oc, oc) - radius*radius;

    float discriminant = b*b - a*c;
    if (discriminant <= 0.0f)
        return false;

    float t = (-b - sqrt(discriminant)) / a;
    if (t <= tMin || t >= tMax)
    {
        t = (-b + sqrt(discriminant)) / a;
        if (t <= tMin || t >= tMax)
            return false;
    }

    hitRecord.t = t;
    hitRecord.p = ray.PointAtParameter(t);
    hitRecord.normal = (hitRecord.p - center) / radius;
    return true;
}
