#include "common.h"
#include "hitable_list.h"
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

Box::Box(const Vector& p0, const Vector& p1, Material* material)
    : pmin(p0)
    , pmax(p1)
{
    Hitable** list = new Hitable*[6];
    list[0] = new XY_Rect(p0.x, p1.x, p0.y, p1.y, p1.z, material);
    list[1] = new Flip_Normals(new XY_Rect(p0.x, p1.x, p0.y, p1.y, p0.z, material));
    list[2] = new XZ_Rect(p0.x, p1.x, p0.z, p1.z, p1.y, material);
    list[3] = new Flip_Normals(new XZ_Rect(p0.x, p1.x, p0.z, p1.z, p0.y, material));
    list[4] = new YZ_Rect(p0.y, p1.y, p0.z, p1.z, p1.x, material);
    list[5] = new Flip_Normals(new YZ_Rect(p0.y, p1.y, p0.z, p1.z, p0.x, material));
    list_ptr = new HitableList(list, 6);
}

bool Box::hit(const Ray& ray, float t_min, float t_max, Intersection& hit) const {
    return list_ptr->hit(ray, t_min, t_max, hit);
}

Bounding_Box Box::boudning_box(float t0, float t1) const {
    return Bounding_Box(pmin, pmax);
}

bool Translate::hit(const Ray& ray, float t_min, float t_max, Intersection& hit) const {
    Ray translated_ray(ray.origin - translation, ray.direction, ray.time);
    if (shape->hit(translated_ray, t_min, t_max, hit)) {
        hit.p += translation;
        return true;
    }
    return false;
}

Bounding_Box Translate::boudning_box(float t0, float t1) const {
    auto bounds = shape->boudning_box(t0, t1);
    bounds.min_point += translation;
    bounds.max_point += translation;
    return bounds;
}

Rotate_Y::Rotate_Y(Hitable* p, float angle)
    : shape(p)
{
    float radians = (PI / 180.f) * angle;
    sin_theta = std::sin(radians);
    cos_theta = std::cos(radians);

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 2; k++) {
                float x = i * box.max_point.x + (1 - i) * box.min_point.x;
                float y = j * box.max_point.y + (1 - j) * box.min_point.y;
                float z = k * box.max_point.z + (1 - k) * box.min_point.z;

                float new_x = cos_theta * x + sin_theta * z;
                float new_z = -sin_theta * x + cos_theta * z;

                Vector tester(new_x, y, new_z);
                for (int c = 0; c < 3; c++) {
                    if (tester[c] > box.max_point[c])
                        box.max_point[c] = tester[c];
                    if (tester[c] < box.min_point[c])
                        box.min_point[c] = tester[c];
                }
            }
        }
    }
}

bool Rotate_Y::hit(const Ray& ray, float t_min, float t_max, Intersection& hit) const {
    Vector o = ray.origin;
    Vector d = ray.direction;

    o[0] = cos_theta * ray.origin[0] - sin_theta * ray.origin[2];
    o[2] = sin_theta * ray.origin[0] + cos_theta * ray.origin[2];

    d[0] = cos_theta * ray.direction[0] - sin_theta * ray.direction[2];
    d[2] = sin_theta * ray.direction[0] + cos_theta * ray.direction[2];

    Ray rotated_ray(o, d, ray.time);

    if (shape->hit(rotated_ray, t_min, t_max, hit)) {
        Vector p = hit.p;
        Vector n = hit.normal;

        p[0] = cos_theta * hit.p[0] + sin_theta * hit.p[2];
        p[2] = -sin_theta * hit.p[0] + cos_theta * hit.p[2];

        n[0] = cos_theta * hit.normal[0] + sin_theta * hit.normal[2];
        n[2] = -sin_theta * hit.normal[0] + cos_theta * hit.normal[2];

        hit.p = p;
        hit.normal = n;
        return true;
    }
    return false;
}

Bounding_Box Rotate_Y::boudning_box(float t0, float t1) const {
    return box;
}

thread_local RNG Constant_Medium::rng;

bool Constant_Medium::hit(const Ray& ray, float t_min, float t_max, Intersection& hit) const {
    Intersection hit1, hit2;

    if (boundary->hit(ray, -FLT_MAX, FLT_MAX, hit1)) {
        if (boundary->hit(ray, hit1.t + 1e-4f, FLT_MAX, hit2)) {
            hit1.t = std::max(hit1.t, t_min);
            hit2.t = std::min(hit2.t, t_max);

            if (hit1.t >= hit2.t)
                return false;
            hit1.t = std::max(hit1.t, 0.f);

            float distance_inside_boundary = (hit2.t - hit1.t);
            float hit_distance = -(1 / density) * std::log(rng.random_float());
            if (hit_distance < distance_inside_boundary) {
                hit.t = hit1.t + hit_distance;
                hit.p = ray.PointAtParameter(hit.t);
                hit.normal = Vector(1, 0, 0);
                hit.material = phase_function;
                return true;
            }
        }
    }

    return false;
}

Bounding_Box Constant_Medium::boudning_box(float t0, float t1) const {
    return boundary->boudning_box(t0, t1);
}
