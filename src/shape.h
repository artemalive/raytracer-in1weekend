#pragma once

#include "bounding_box.h"
#include "vector.h"
#include "material.h"
#include "random.h"

class Bounding_Box;
class Material;
class Ray;
class Texture;

struct Intersection
{
    float t;
    Vector p;
    Vector normal;
    float u, v;
    Material* material;
};

class Shape {
public:
    virtual bool hit(const Ray& ray, float t_min, float t_max, Intersection& hit_record) const = 0;
    virtual Bounding_Box boudning_box(float t0, float t1) const = 0;
};

class XY_Rect : public Shape {
public:
    XY_Rect(float x0, float x1, float y0, float y1, float k, Material* material)
        : x0(x0), x1(x1), y0(y0), y1(y1), k(k), material(material) {}

    bool hit(const Ray& ray, float t_min, float t_max, Intersection& hit) const override;
    Bounding_Box boudning_box(float t0, float t1) const override;

    float x0, x1, y0, y1, k;
    Material* material;
};

class XZ_Rect : public Shape {
public:
    XZ_Rect(float x0, float x1, float z0, float z1, float k, Material* material)
        : x0(x0), x1(x1), z0(z0), z1(z1), k(k), material(material) {}

    bool hit(const Ray& ray, float t_min, float t_max, Intersection& hit) const override;
    Bounding_Box boudning_box(float t0, float t1) const override;

    float x0, x1, z0, z1, k;
    Material* material;
};

class YZ_Rect : public Shape {
public:
    YZ_Rect(float y0, float y1, float z0, float z1, float k, Material* material)
        : y0(y0), y1(y1), z0(z0), z1(z1), k(k), material(material) {}

    bool hit(const Ray& ray, float t_min, float t_max, Intersection& hit) const override;
    Bounding_Box boudning_box(float t0, float t1) const override;

    float y0, y1, z0, z1, k;
    Material* material;
};

class Flip_Normals : public Shape {
public:
    Flip_Normals(Shape* hitable) : hitable(hitable) {}

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

    Shape* hitable;
};

class Box : public Shape {
public:
    Box(const Vector& p0, const Vector& p1, Material* material);
    bool hit(const Ray& ray, float t_min, float t_max, Intersection& hit) const override;
    Bounding_Box boudning_box(float t0, float t1) const override;

private:
    Vector pmin, pmax;
    Shape* list_ptr;
};

class Translate : public Shape {
public:
    Translate(Shape* shape, const Vector& translation) : shape(shape), translation(translation) {}
    bool hit(const Ray& ray, float t_min, float t_max, Intersection& hit) const override;
    Bounding_Box boudning_box(float t0, float t1) const override;

private:
    Shape* shape;
    Vector translation;
};

class Rotate_Y : public Shape {
public:
    Rotate_Y(Shape* p, float angle);
    bool hit(const Ray& ray, float t_min, float t_max, Intersection& hit) const override;
    Bounding_Box boudning_box(float t0, float t1) const override;

private:
    Shape* shape;
    float sin_theta;
    float cos_theta;
    Bounding_Box box;
};

//class Constant_Medium : public Shape {
//public:
//    Constant_Medium(Shape* boundary, float density, Texture* a)
//        : boundary(boundary), density(density), phase_function(new Isotropic(a)) {}
//
//    bool hit(const Ray& ray, float t_min, float t_max, Intersection& hit) const override;
//    Bounding_Box boudning_box(float t0, float t1) const override;
//
//private:
//    Shape* boundary;
//    float density;
//    Material* phase_function;
//
//    static thread_local RNG rng;
//};
