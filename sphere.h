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

class Moving_Sphere : public Hitable
{
public:
    Moving_Sphere() {}
    Moving_Sphere(const Vector& center0, const Vector& center1, float time0, float time1,
        float radius, Material* material)
        : center0(center0), center1(center1), time0(time0), time1(time1), radius(radius), material(material) {}

    bool Hit(const Ray& ray, float tMin, float tMax, HitRecord& hitRecord) const override;

    Vector center0, center1;
    float time0, time1;
    float radius;
    Material* material;
};

inline bool ray_sphere_intersect(
    const Vector& center, float radius, const Ray& ray,
    float tMin, float tMax, Material* material, HitRecord& hitRecord)
{
    Vector oc = ray.origin - center;
    float b = dot_product(oc, ray.direction);
    float c = dot_product(oc, oc) - radius*radius;

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


inline bool Sphere::Hit(const Ray& ray, float tMin, float tMax, HitRecord& hitRecord) const
{
    return ray_sphere_intersect(center, radius, ray, tMin, tMax, material, hitRecord);
}

inline bool Moving_Sphere::Hit(const Ray& ray, float tMin, float tMax, HitRecord& hitRecord) const
{
    Vector center = center0 + ((ray.time - time0) / (time1 - time0)) * (center1 - center0);
    return ray_sphere_intersect(center, radius, ray, tMin, tMax, material, hitRecord);
}
