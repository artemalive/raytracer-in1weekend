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

Vector Reflect(const Vector& v, const Vector& normal)
{
    return v - 2.0f*Dot(v, normal)*normal;
}

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

    Sphere sphere1(Vector(0, 0, -1), 0.5f, new Lambertian(Vector(0.8f, 0.3f, 0.3f)));
    Sphere sphere2(Vector(0, -100.5f, -1), 100, new Lambertian(Vector(0.8f, 0.8f, 0.0f)));
    Sphere sphere3(Vector(-1, 0, -1), 0.5f, new Metal(Vector(0.8f, 0.8f, 0.8f), 0.3f));
    Sphere sphere4(Vector(1, 0, -1), 0.5f, new Metal(Vector(0.8f, 0.6f, 0.2f), 1.0f));

    Hitable* list[4] = { &sphere1, &sphere2, &sphere3, &sphere4 };
    HitableList world(list, 4);

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
