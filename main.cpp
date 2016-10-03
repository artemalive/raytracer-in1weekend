#include <iostream>
#include "ray.h"

bool HitSphere(const Vector& center, float radius, const Ray& ray)
{
    Vector oc = ray.Origin() - center;
    float a = Dot(ray.Direction(), ray.Direction());
    float b = 2.0f * Dot(oc, ray.Direction());
    float c = Dot(oc, oc) - radius*radius;

    float discriminant = b*b - 4*a*c;
    return discriminant > 0.0f;
}

Vector Color(const Ray& ray)
{
    if (HitSphere(Vector(0, 0, -1), 0.5f, ray))
        return Vector(1, 0, 0);

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

    for (int j = ny - 1; j >= 0; j--)
    {
        for (int i = 0; i < nx; i++)
        {
            float u = float(i) / float(nx);
            float v = float(j) / float(ny);

            Ray ray(origin, lowerLeftCorner + u*horizontal + v*vertical);
            Vector color = Color(ray);

            int ir = static_cast<int>(255.99 * color[0]);
            int ig = static_cast<int>(255.99 * color[1]);
            int ib = static_cast<int>(255.99 * color[2]);

            std::cout << ir << " " << ig << " " << ib << "\n";
        }
    }
}
