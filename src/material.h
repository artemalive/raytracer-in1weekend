#pragma once

#include "vector.h"
#include <algorithm>

class RNG;
class Ray;
class Texture;
struct Intersection;


class Material {
public:
    virtual bool scatter(RNG& rng, const Ray& ray, const Intersection& isect, Vector& attenuation, Ray& scattered_ray) const = 0;
    virtual Vector emitted(float u, float v, const Vector& p) const { return Vector(0); }
};

class Lambertian : public Material {
public:
    Lambertian(Texture* albedo) : albedo(albedo) {}
    bool scatter(RNG& rng, const Ray& ray, const Intersection& hit, Vector& attenuation, Ray& scattered_ray) const override;

private:
    Texture* albedo;
};

class Metal : public Material {
public:
    Metal(const Vector& albedo, float fuzz) : albedo(albedo), fuzz(std::min(fuzz, 1.f)) {}
    bool scatter(RNG& rng, const Ray& ray, const Intersection& hit, Vector& attenuation, Ray& scattered_ray) const override;

private:
    Vector albedo;
    float fuzz;
};

class Dielectric : public Material {
public:
    Dielectric(float ri) : refraction_index(ri) {}
    bool scatter(RNG& rng, const Ray& ray, const Intersection& hit, Vector& attenuation, Ray& scattered_ray) const override;

private:
    float refraction_index;
};

class Diffuse_Light : public Material {
public:
    Diffuse_Light(Texture* emit) : emit(emit) {}

    bool scatter(RNG& rng, const Ray& ray, const Intersection& hit, Vector& attenuation, Ray& scattered_ray) const override { return false; }
    Vector emitted(float u, float v, const Vector& p) const override;

private:
    Texture* emit;
};
