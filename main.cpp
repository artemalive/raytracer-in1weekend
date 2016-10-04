#include <iostream>
#include <limits>
#include "camera.h"
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

Vector Color(const Ray& ray, Hitable* world)
{
    HitRecord hitRecord;
    if (world->Hit(ray, 0.0, std::numeric_limits<float>::max(), hitRecord))
    {
        Vector target = hitRecord.p + hitRecord.normal + RandomPointInUnitSphere();
        return 0.5f * Color(Ray(hitRecord.p, target - hitRecord.p), world);
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

    Camera camera;

    Sphere sphere1(Vector(0, 0, -1), 0.5f);
    Sphere sphere2(Vector(0, -100.5f, -1), 100);
    Hitable* list[2] = { &sphere1, &sphere2 };

    HitableList world(list, 2);

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
                color += Color(ray, &world);
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
