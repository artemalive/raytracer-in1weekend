#pragma once

#include "common.h"
#include "bounding_box.h"
#include "shape.h"

class Sphere : public Shape {
public:
    Sphere(const Vector& center, float radius, Material* material)
        : center(center), radius(radius), material(material) {}

    bool hit(const Ray& ray, float tMin, float tMax, Intersection& hitRecord) const override;
    Bounding_Box boudning_box(float t0, float t1) const override;
    float pdf_value(const Vector& o, const Vector& v) const override;
    Vector random_direction(RNG& rng, const Vector& o) const override;

private:
    Vector center;
    float radius;
    Material* material;
};

class Moving_Sphere : public Shape {
public:
    Moving_Sphere(const Vector& center0, const Vector& center1, float time0, float time1, float radius, Material* material)
        : center0(center0), center1(center1), time0(time0), time1(time1), radius(radius), material(material) {}

    bool hit(const Ray& ray, float tMin, float tMax, Intersection& hitRecord) const override;
    Bounding_Box boudning_box(float t0, float t1) const override;

private:
    Vector get_center(float time) const {
        return center0 + ((time - time0) / (time1 - time0)) * (center1 - center0);
    }

private:
    Vector center0, center1;
    float time0, time1;
    float radius;
    Material* material;
};
