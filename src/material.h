#pragma once

#include "common.h"
#include "shape.h"
#include "vector.h"
#include <algorithm>

class RNG;
class Ray;
class Texture;
struct Intersection;

class Pdf {
public:
    virtual ~Pdf() {}
    virtual float value(const Vector& direction) const = 0;
    virtual Vector generate(RNG& rng) const = 0;
};

class Cosine_Pdf : public Pdf {
public:
    Cosine_Pdf(const Vector& direction)
    : axes(direction) {}

    float value(const Vector& direction) const override {
        float cosine = dot_product(direction, axes.e3);
        if (cosine > 0.f)
            return cosine / PI;
        else
            return 0.f;
    }
    Vector generate(RNG& rng) const override {
        return axes.from_local_to_world(random_cosine_direction(rng));
    }
    Axes axes;
};

class Shape_Pdf : public Pdf {
public:
    Shape_Pdf(Shape* shape, const Vector& origin)
        : shape(shape), origin(origin) {}

    float value(const Vector& direction) const override {
        return shape->pdf_value(origin, direction);
    }
    Vector generate(RNG& rng) const override {
        return shape->random_direction(rng, origin);
    }

    Shape* shape;
    Vector origin;
};

class Mixture_Pdf : public Pdf {
public:
    Mixture_Pdf(Pdf* p0, Pdf* p1)
        : p0(p0), p1(p1) {}

    float value(const Vector& direction) const override {
        return 0.5f * p0->value(direction) + 0.5f * p1->value(direction);
    }
    Vector generate(RNG& rng) const override {
        if (rng.random_float() < 0.5f)
            return p0->generate(rng);
        else
            return p1->generate(rng);
    }

    Pdf* p0;
    Pdf* p1;
};

struct Scatter_Info {
    Ray specular_ray;
    bool is_specular;
    Vector attenuation;
    Pdf* pdf;
};

class Material {
public:
    virtual bool scatter(RNG& rng, const Ray& ray, const Intersection& hit, Scatter_Info& scatter_info) const {
        return false;
    }
    virtual float scattering_pdf(const Ray& ray, const Intersection& hit, const Ray& scattered_ray) const {
        return 0.f;
    }
    virtual Vector emitted(const Ray& ray_in, const Intersection& isect, float u, float v, const Vector& p) const {
        return Vector(0);
    }
};

class Lambertian : public Material {
public:
    Lambertian(Texture* albedo) : albedo(albedo) {}
    bool scatter(RNG& rng, const Ray& ray, const Intersection& hit,  Scatter_Info& scatter_info) const override;
    float scattering_pdf(const Ray& ray_in, const Intersection& isect, const Ray& scattered_ray) const override;

private:
    Texture* albedo;
};

class Metal : public Material {
public:
    Metal(const Vector& albedo, float fuzz) : albedo(albedo), fuzz(std::min(fuzz, 1.f)) {}
    bool scatter(RNG& rng, const Ray& ray, const Intersection& hit, Scatter_Info& scatter_info) const override;

private:
    Vector albedo;
    float fuzz;
};

//class Dielectric : public Material {
//public:
//    Dielectric(float ri) : refraction_index(ri) {}
//    bool scatter(RNG& rng, const Ray& ray, const Intersection& hit, Vector& attenuation, Ray& scattered_ray) const override;
//
//private:
//    float refraction_index;
//};
//
class Diffuse_Light : public Material {
public:
    Diffuse_Light(Texture* emit) : emit(emit) {}

    bool scatter(RNG& rng, const Ray& ray, const Intersection& hit, Scatter_Info& scatter_info) const override {
        return false;
    }

    Vector emitted(const Ray& ray_in, const Intersection& isect, float u, float v, const Vector& p) const override;

private:
    Texture* emit;
};
//
//class Isotropic : public Material {
//public:
//    Isotropic(Texture *a) : albedo(a) {}
//    bool scatter(RNG& rng, const Ray& ray, const Intersection& hit, Vector& attenuation, Ray& scattered_ray) const override;
//
//private:
//    Texture *albedo;
//};
