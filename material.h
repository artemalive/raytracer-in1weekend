#pragma once

#include "shape.h"
#include "src/perlin.h"
#include "random.h"
#include "src/texture.h"

Vector RandomPointInUnitSphere(RNG& rng)
{
    Vector p;
    do
    {
        p = 2.0f * Vector(rng.random_float(), rng.random_float(), rng.random_float()) - Vector(1, 1, 1);
    } while (dot_product(p, p) >= 1.0f);
    return p;
}

Vector Reflect(const Vector& v, const Vector& normal)
{
    return v - 2.0f*dot_product(v, normal)*normal;
}

bool Refract(const Vector& v, const Vector& normal, float niOverNt, Vector& refracted)
{
    Vector uv = v.normalized();
    float dt = dot_product(uv, normal);

    float discriminant = 1.0f - niOverNt*niOverNt*(1.0f - dt*dt);
    if (discriminant <= 0.0f)
        return false;

    refracted = niOverNt*(uv - normal*dt) - normal*sqrt(discriminant);
    return true;
}

float Schlick(float cosine, float refractionIndex)
{
    float r0 = (1 - refractionIndex) / (1 + refractionIndex);
    r0 = r0*r0;
    return r0 + (1 - r0)*std::pow(1 - cosine, 5);
}

class Material
{
public:
    virtual bool Scatter(RNG& rng, const Ray& ray, const Hit_Record& hit_record,
        Vector& attenuation, Ray& scatteredRay) const = 0;

    virtual Vector Emitted(float u, float v, const Vector& p) const {
        return Vector(0);
    }
};

class Lambertian : public Material
{
public:
    Lambertian(Texture* albedo) : albedo(albedo) {}

    bool Scatter(RNG& rng, const Ray& ray, const Hit_Record& hit_record,
        Vector& attenuation, Ray& scatteredRay) const override
    {
        scatteredRay = Ray(hit_record.p, (hit_record.normal + RandomPointInUnitSphere(rng)).normalized(), ray.time);
        attenuation = albedo->value(hit_record.u, hit_record.v, hit_record.p);
        return true;
    }

    Texture* albedo;
};

class Metal : public Material
{
public:
    Metal(const Vector& albedo, float fuzz) : albedo(albedo)
    {
        this->fuzz = std::min(fuzz, 1.0f);
    }

    bool Scatter(RNG& rng, const Ray& ray, const Hit_Record& hitRecord,
        Vector& attenuation, Ray& scatteredRay) const override
    {
        Vector reflected = Reflect(ray.direction, hitRecord.normal);
        scatteredRay = Ray(hitRecord.p, (reflected + fuzz * RandomPointInUnitSphere(rng)).normalized(), ray.time);
        attenuation = albedo;
        return dot_product(scatteredRay.direction, hitRecord.normal) > 0.0f;
    }

    Vector albedo;
    float fuzz;
};

class Dielectric : public Material
{
public:
    Dielectric(float ri) : refractionIndex(ri) {}

    bool Scatter(RNG& rng, const Ray& ray, const Hit_Record& hitRecord,
        Vector& attenuation, Ray& scatteredRay) const override
    {
        Vector outwardNormal;
        Vector reflected = Reflect(ray.direction, hitRecord.normal);

        float n;
        attenuation = Vector(1.0, 1.0, 1.0);
        Vector refracted;
        float reflectProb;
        float cosine;

        if (dot_product(ray.direction, hitRecord.normal) > 0.0f)
        {
            outwardNormal = -hitRecord.normal;
            n = refractionIndex;
            cosine = dot_product(ray.direction, hitRecord.normal);
            cosine = sqrt(1 - refractionIndex*refractionIndex*(1-cosine*cosine));
        }
        else
        {
            outwardNormal = hitRecord.normal;
            n = 1.0f / refractionIndex;
            cosine = -dot_product(ray.direction, hitRecord.normal);
        }

        if (Refract(ray.direction, outwardNormal, n, refracted))
        {
            reflectProb = Schlick(cosine, refractionIndex);
        }
        else
        {
            reflectProb = 1.0f;
        }

        if (rng.random_float() < reflectProb)
        {
            scatteredRay = Ray(hitRecord.p, reflected, ray.time);
        }
        else
        {
            scatteredRay = Ray(hitRecord.p, refracted, ray.time);
        }
        return true;
    }

    float refractionIndex;
};

class Diffuse_Light : public Material {
public:
    Diffuse_Light(Texture* emit) : emit(emit) {}

    bool Scatter(RNG& rng, const Ray& ray, const Hit_Record& hitRecord,
        Vector& attenuation, Ray& scatteredRay) const override
    {
        return false;
    }

    Vector Emitted(float u, float v, const Vector& p) const override {
        return emit->value(u, v, p);
    }

    Texture* emit;
};
