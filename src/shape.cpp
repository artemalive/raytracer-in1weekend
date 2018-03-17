#include "ray.h"
#include "shape.h"
#include "bounding_box.h"

bool XY_Rect::hit(const Ray& ray, float t_min, float t_max, Intersection& hit) const {
    float t = (k - ray.origin.z) / ray.direction.z;
    if (t < t_min || t > t_max)
        return false;

    float x = ray.origin.x + t * ray.direction.x;
    float y = ray.origin.y + t * ray.direction.y;
    if (x < x0 || x > x1 || y < y0 || y > y1)
        return false;

    hit.u = (x - x0) / (x1 - x0);
    hit.v = (y - y0) / (y1 - y0);
    hit.t = t;
    hit.material = material;
    hit.p = ray.PointAtParameter(t);
    hit.normal = Vector(0, 0, 1);
    return true;
}

Bounding_Box XY_Rect::boudning_box(float t0, float t1) const {
    return Bounding_Box(Vector(x0, y0, k - 1e-4f), Vector(x1, y1, k + 1e-4f));
}

bool XZ_Rect::hit(const Ray& ray, float t_min, float t_max, Intersection& hit) const {
    float t = (k - ray.origin.y) / ray.direction.y;
    if (t < t_min || t > t_max)
        return false;

    float x = ray.origin.x + t * ray.direction.x;
    float z = ray.origin.z + t * ray.direction.z;
    if (x < x0 || x > x1 || z < z0 || z > z1)
        return false;

    hit.u = (x - x0) / (x1 - x0);
    hit.v = (z - z0) / (z1 - z0);
    hit.t = t;
    hit.material = material;
    hit.p = ray.PointAtParameter(t);
    hit.normal = Vector(0, 1, 0);
    return true;
}

Bounding_Box XZ_Rect::boudning_box(float t0, float t1) const {
    return Bounding_Box(Vector(x0, k - 1e-4f, z0), Vector(x1, k + 1e-4f, z1));
}

bool YZ_Rect::hit(const Ray& ray, float t_min, float t_max, Intersection& hit) const {
    float t = (k - ray.origin.x) / ray.direction.x;
    if (t < t_min || t > t_max)
        return false;

    float y = ray.origin.y + t * ray.direction.y;
    float z = ray.origin.z + t * ray.direction.z;
    if (y < y0 || y > y1 || z < z0 || z > z1)
        return false;

    hit.u = (y - y0) / (y1 - y0);
    hit.v = (z - z0) / (z1 - z0);
    hit.t = t;
    hit.material = material;
    hit.p = ray.PointAtParameter(t);
    hit.normal = Vector(1, 0, 0);
    return true;
}

Bounding_Box YZ_Rect::boudning_box(float t0, float t1) const {
    return Bounding_Box(Vector(k - 1e-4f, y0, z0), Vector(k + 1e-4f, y1, z1));
}
