#pragma once

#include "hitable.h"
#include "perlin.h"
#include "random.h"

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
};

class Texture {
public:
    virtual Vector value(float u, float v, const Vector& p) const = 0;
};

class Constant_Texture : public Texture {
public:
    Constant_Texture() {}
    Constant_Texture(Vector c) : color(c) {}

    Vector value(float u, float v, const Vector& p) const override {
        return color;
    }

    Vector color;
};

class Checker_Texture : public Texture {
public:
    Checker_Texture() {}
    Checker_Texture(Texture* t0, Texture* t1) : even(t0), odd(t1) {}

    Vector value(float u, float v, const Vector& p) const override {
        float sines = std::sin(10 * p.x) * std::sin(10 * p.y) * std::sin(10 * p.z);
        if (sines < 0.f)
            return odd->value(u, v, p);
        else
            return even->value(u, v, p);
    }

    Texture* odd;
    Texture* even;
};

class Noise_Texture : public Texture {
public:
    Noise_Texture(RNG& rng, float scale) : noise(rng), scale(scale) {}

    Vector value(float u, float v, const Vector& p) const override {
        //float f = 0.5f * (noise.noise(p * scale) + 1.f);
        //float f = noise.turbulence(p * scale);
        float f = 0.5f * (1.f + std::sin(scale*p.z + 10.f * noise.turbulence(p)));

        return Vector(1) * f;
    }

    Perlin noise;
    float scale;
};

class Image_Texture : public Texture {
public:
    Image_Texture(unsigned char* pixels, int w, int h)
        : pixels(pixels),
        w(w), h(h),
        wf(static_cast<float>(w)),
        hf(static_cast<float>(h)) {}

    Vector value(float u, float v, const Vector& p) const override {
        int i = static_cast<int>(u * wf);
        int j = static_cast<int>((1 - v) * hf);
        if (i < 0) i = 0;
        if (j < 0) j = 0;
        if (i > w - 1) i = w - 1;
        if (i > h - 1) j = h - 1;

        int offset = 3*(i + w*j);
        float r = int(pixels[offset]) / 255.f;
        float g = int(pixels[offset+1]) / 255.f;
        float b = int(pixels[offset+2]) / 255.f;
        return Vector(r, g, b);
    }

    unsigned char* pixels;
    int w, h;
    float wf, hf;
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
