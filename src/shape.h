#pragma once

#include "bounding_box.h"
#include "vector.h"

class Bounding_Box;
class Material;
class Ray;

struct Intersection
{
    float t;
    Vector p;
    Vector normal;
    float u, v;
    Material* material;
};

class Hitable {
public:
    virtual bool hit(const Ray& ray, float t_min, float t_max, Intersection& hit_record) const = 0;
    virtual Bounding_Box boudning_box(float t0, float t1) const = 0;
};

class XY_Rect : public Hitable {
public:
    XY_Rect(float x0, float x1, float y0, float y1, float k, Material* material)
        : x0(x0), x1(x1), y0(y0), y1(y1), k(k), material(material) {}

    bool hit(const Ray& ray, float t_min, float t_max, Intersection& hit) const override;
    Bounding_Box boudning_box(float t0, float t1) const override;

    float x0, x1, y0, y1, k;
    Material* material;
};

class XZ_Rect : public Hitable {
public:
    XZ_Rect(float x0, float x1, float z0, float z1, float k, Material* material)
        : x0(x0), x1(x1), z0(z0), z1(z1), k(k), material(material) {}

    bool hit(const Ray& ray, float t_min, float t_max, Intersection& hit) const override;
    Bounding_Box boudning_box(float t0, float t1) const override;

    float x0, x1, z0, z1, k;
    Material* material;
};

class YZ_Rect : public Hitable {
public:
    YZ_Rect(float y0, float y1, float z0, float z1, float k, Material* material)
        : y0(y0), y1(y1), z0(z0), z1(z1), k(k), material(material) {}

    bool hit(const Ray& ray, float t_min, float t_max, Intersection& hit) const override;
    Bounding_Box boudning_box(float t0, float t1) const override;

    float y0, y1, z0, z1, k;
    Material* material;
};

class Flip_Normals : public Hitable {
public:
    Flip_Normals(Hitable* hitable) : hitable(hitable) {}

    bool hit(const Ray& ray, float t_min, float t_max, Intersection& hit) const override {
        if (hitable->hit(ray, t_min, t_max, hit)) {
            hit.normal = -hit.normal;
            return true;
        }
        return false;
    }

    Bounding_Box boudning_box(float t0, float t1) const override {
        return hitable->boudning_box(t0, t1);
    }

    Hitable* hitable;
};
