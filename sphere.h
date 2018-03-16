#pragma once

#include "bounding_box.h"
#include "hitable.h"

inline void get_sphere_uv(const Vector& p, float& u, float& v) {
    float phi = std::atan2(p.z, p.x);
    float theta = std::asin(p.y);
    u = 1.f - (phi + PI) / (2 * PI);
    v = (theta + PI / 2) / PI;
}

inline bool ray_sphere_intersect(
    const Vector& center, float radius, const Ray& ray,
    float tMin, float tMax, Material* material, Hit_Record& hitRecord)
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
    get_sphere_uv((hitRecord.p - center) / radius, hitRecord.u, hitRecord.v);
    hitRecord.material = material;
    return true;
}

class Sphere : public Hitable
{
public:
    Sphere() {}
    Sphere(const Vector& center, float radius, Material* material)
        : center(center), radius(radius), material(material) {}

    bool hit(const Ray& ray, float tMin, float tMax, Hit_Record& hitRecord) const override {
        return ray_sphere_intersect(center, radius, ray, tMin, tMax, material, hitRecord);
    }

    Bounding_Box boudning_box(float t0, float t1) const override {
        return Bounding_Box(center - Vector(radius), center + Vector(radius));
    }

    Vector center;
    float radius;
    Material* material;
};

class Moving_Sphere : public Hitable
{
private:
    Vector get_center(float time) const {
        return center0 + ((time - time0) / (time1 - time0)) * (center1 - center0);
    }

public:
    Moving_Sphere() {}
    Moving_Sphere(const Vector& center0, const Vector& center1, float time0, float time1,
        float radius, Material* material)
        : center0(center0), center1(center1), time0(time0), time1(time1), radius(radius), material(material) {}

    bool hit(const Ray& ray, float tMin, float tMax, Hit_Record& hitRecord) const override {
        return ray_sphere_intersect(get_center(ray.time), radius, ray, tMin, tMax, material, hitRecord);
    }

    Bounding_Box boudning_box(float t0, float t1) const override {
        Bounding_Box box_t0, box_t1;

        Vector center_t0 = get_center(t0);
        Vector center_t1 = get_center(t1);

        box_t0 = Bounding_Box(center_t0 - Vector(radius), center_t0 + Vector(radius));
        box_t1 = Bounding_Box(center_t1 - Vector(radius), center_t1 + Vector(radius));
        return Bounding_Box::get_union(box_t0, box_t1);
    }

    Vector center0, center1;
    float time0, time1;
    float radius;
    Material* material;
};
