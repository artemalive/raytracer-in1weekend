#include <iostream>
#include <limits>
#include "sphere.h"
#include "hitable_list.h"

Vector Color(const Ray& ray, Hitable* world)
{
    HitRecord hitRecord;
    if (world->Hit(ray, 0.0, std::numeric_limits<float>::max(), hitRecord))
        return 0.5*(hitRecord.normal + Vector(1, 1, 1));

    Vector unitDirection = UnitVector(ray.Direction());
    float t = 0.5f * (unitDirection.Y() + 1.0f);
    return (1.0f - t)*Vector(1.0f, 1.0f, 1.0f) + t*Vector(0.5f, 0.7f, 1.0f);
}

int main()
{
    const int nx = 200;
    const int ny = 100;
    std::cout << "P3\n" << nx << " " << ny << "\n255\n";

    Vector lowerLeftCorner(-2.0f, -1.0f, -1.0f);
    Vector horizontal(4.0f, 0.0f, 0.0f);
    Vector vertical(0.0f, 2.0f, 0.0f);
    Vector origin(0.0f, 0.0f, 0.0f);

    Sphere sphere1(Vector(0, 0, -1), 0.5f);
    Sphere sphere2(Vector(0, -100.5f, -1), 100);
    Hitable* list[2] = { &sphere1, &sphere2 };

    HitableList world(list, 2);

    for (int j = ny - 1; j >= 0; j--)
    {
        for (int i = 0; i < nx; i++)
        {
            float u = float(i) / float(nx);
            float v = float(j) / float(ny);
            Ray ray(origin, lowerLeftCorner + u*horizontal + v*vertical);

            Vector color = Color(ray, &world);

            int ir = static_cast<int>(255.99 * color[0]);
            int ig = static_cast<int>(255.99 * color[1]);
            int ib = static_cast<int>(255.99 * color[2]);

            std::cout << ir << " " << ig << " " << ib << "\n";
        }
    }
}
