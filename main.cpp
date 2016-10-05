#include <algorithm>
#include <iostream>
#include <limits>
#include "camera.h"
#include "material.h"
#include "random.h"
#include "sphere.h"
#include "hitable_list.h"

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

Vector Color(const Ray& ray, Hitable* world, int depth)
{
    HitRecord hitRecord;
    if (world->Hit(ray, 0.001f, std::numeric_limits<float>::max(), hitRecord))
    {
        Ray scattered;
        Vector attenuation;
        if (depth < 50 && hitRecord.material->Scatter(ray, hitRecord, attenuation, scattered))
        {
            return attenuation * Color(scattered, world, depth + 1);
        }
        return Vector(0, 0, 0);
    }

    Vector unitDirection = UnitVector(ray.Direction());
    float t = 0.5f * (unitDirection.Y() + 1.0f);
    return (1.0f - t)*Vector(1.0f, 1.0f, 1.0f) + t*Vector(0.5f, 0.7f, 1.0f);
}

int main()
{
    const int nx = 200;
    const int ny = 100;
    const int ns = 100;

    std::cout << "P3\n" << nx << " " << ny << "\n255\n";

    Sphere sphere1(Vector(0, 0, -1), 0.5f, new Lambertian(Vector(0.1f, 0.2f, 0.5f)));
    Sphere sphere2(Vector(0, -100.5f, -1), 100, new Lambertian(Vector(0.8f, 0.8f, 0.0f)));
    Sphere sphere3(Vector(1, 0, -1), 0.5f, new Metal(Vector(0.8f, 0.6f, 0.2f), 1.0f));
    Sphere sphere4(Vector(-1.0, 0, -1), 0.5f, new Dielectric(1.5f));
    Sphere sphere5(Vector(-1.0, 0, -1), -0.45f, new Dielectric(1.5f));

    Hitable* list[5] = { &sphere1, &sphere2, &sphere3, &sphere4, &sphere5 };
    HitableList world(list, 5);

    Camera camera;
    for (int j = ny - 1; j >= 0; j--)
    {
        for (int i = 0; i < nx; i++)
        {
            Vector color(0.0f, 0.0f, 0.0f);
            for (int s = 0; s < ns; s++)
            {
                float u = (float(i) + RandomFloat()) / float(nx);
                float v = (float(j) + RandomFloat()) / float(ny);

                Ray ray = camera.GetRay(u, v);
                color += Color(ray, &world, 0);
            }
            color /= float(ns);
            color = Vector(sqrt(color[0]), sqrt(color[1]), sqrt(color[2]));
            int ir = static_cast<int>(255.99 * color[0]);
            int ig = static_cast<int>(255.99 * color[1]);
            int ib = static_cast<int>(255.99 * color[2]);
            std::cout << ir << " " << ig << " " << ib << "\n";
        }
    }
}
