#pragma once

#include "hitable.h"
#include "random.h"

Vector RandomPointInUnitSphere()
{
    Vector p;
    do
    {
        p = 2.0f * Vector(RandomFloat(), RandomFloat(), RandomFloat()) - Vector(1, 1, 1);
    } while (Dot(p, p) >= 1.0f);
    return p;
}

Vector Reflect(const Vector& v, const Vector& normal)
{
    return v - 2.0f*Dot(v, normal)*normal;
}

bool Refract(const Vector& v, const Vector& normal, float niOverNt, Vector& refracted)
{
    Vector uv = UnitVector(v);
    float dt = Dot(uv, normal);

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
    virtual bool Scatter(const Ray& ray, const HitRecord& hitRecord,
        Vector& attenuation, Ray& scatteredRay) const = 0;
};

class Lambertian : public Material
{
public:
    Lambertian(const Vector& albedo) : albedo(albedo) {}

    bool Scatter(const Ray& ray, const HitRecord& hitRecord,
        Vector& attenuation, Ray& scatteredRay) const override
    {
        Vector target = hitRecord.p + hitRecord.normal + RandomPointInUnitSphere();
        scatteredRay = Ray(hitRecord.p, target - hitRecord.p);
        attenuation = albedo;
        return true;
    }

    Vector albedo;
};

class Metal : public Material
{
public:
    Metal(const Vector& albedo, float fuzz) : albedo(albedo)
    {
        this->fuzz = std::min(fuzz, 1.0f);
    }

    bool Scatter(const Ray& ray, const HitRecord& hitRecord,
        Vector& attenuation, Ray& scatteredRay) const override
    {
        Vector reflected = Reflect(UnitVector(ray.Direction()), hitRecord.normal);
        scatteredRay = Ray(hitRecord.p, reflected + fuzz * RandomPointInUnitSphere());
        attenuation = albedo;
        return Dot(scatteredRay.Direction(), hitRecord.normal) > 0.0f;
    }

    Vector albedo;
    float fuzz;
};

class Dielectric : public Material
{
public:
    Dielectric(float ri) : refractionIndex(ri) {}

    bool Scatter(const Ray& ray, const HitRecord& hitRecord,
        Vector& attenuation, Ray& scatteredRay) const override
    {
        Vector outwardNormal;
        Vector reflected = Reflect(ray.Direction(), hitRecord.normal);

        float n;
        attenuation = Vector(1.0, 1.0, 1.0);
        Vector refracted;
        float reflectProb;
        float cosine;

        if (Dot(ray.Direction(), hitRecord.normal) > 0.0f)
        {
            outwardNormal = -hitRecord.normal;
            n = refractionIndex;
            cosine = Dot(ray.Direction(), hitRecord.normal) / ray.Direction().Length();
            cosine = sqrt(1 - refractionIndex*refractionIndex*(1-cosine*cosine));
        }
        else
        {
            outwardNormal = hitRecord.normal;
            n = 1.0f / refractionIndex;
            cosine = -Dot(ray.Direction(), hitRecord.normal) / ray.Direction().Length();
        }

        if (Refract(ray.Direction(), outwardNormal, n, refracted))
        {
            reflectProb = Schlick(cosine, refractionIndex);
        }
        else
        {
            reflectProb = 1.0f;
        }

        if (RandomFloat() < reflectProb)
        {
            scatteredRay = Ray(hitRecord.p, reflected);
        }
        else
        {
            scatteredRay = Ray(hitRecord.p, refracted);
        }
        return true;
    }

    float refractionIndex;
};
