#include "sphere.h"
#include <cassert>

static void get_sphere_uv(const Vector& p, float& u, float& v) {
    float phi = std::atan2(p.z, p.x);
    float theta = std::asin(clamp(p.y, -1.f, 1.f));
    u = 1.f - (phi + PI) / (2 * PI);
    v = (theta + PI / 2) / PI;
    assert(!std::isnan(u));
    assert(!std::isnan(v));
}

static bool ray_sphere_intersect(
    const Vector& center, float radius, const Ray& ray,
    float tMin, float tMax, Material* material, Intersection& hitRecord)
{
    Vector oc = ray.origin - center;
    float b = dot_product(oc, ray.direction);
    float c = dot_product(oc, oc) - radius*radius;

    float discriminant = b*b - c;
    if (discriminant <= 0.0f)
        return false;

    float t = -b - sqrt(discriminant);
    if (t <= tMin || t >= tMax)
    {
        t = -b + sqrt(discriminant);
        if (t <= tMin || t >= tMax)
            return false;
    }

    hitRecord.t = t;
    hitRecord.p = ray.PointAtParameter(t);
    hitRecord.normal = (hitRecord.p - center) / radius;
    get_sphere_uv((hitRecord.p - center) / radius, hitRecord.u, hitRecord.v);
    hitRecord.material = material;
    return true;
}

bool Sphere::hit(const Ray& ray, float tMin, float tMax, Intersection& hitRecord) const {
    return ray_sphere_intersect(center, radius, ray, tMin, tMax, material, hitRecord);
}

Bounding_Box Sphere::boudning_box(float t0, float t1) const {
    return Bounding_Box(center - Vector(radius), center + Vector(radius));
}

float Sphere::pdf_value(const Vector& o, const Vector& v) const {
    Intersection isect;

    if (hit(Ray(o, v), 1e-3f, FLT_MAX, isect)) {
        float cos_theta_max = std::sqrt(1.f - radius*radius/(center-o).squared_length());
        float solid_angle = 2*PI*(1 - cos_theta_max);
        return 1.f / solid_angle;
    } else {
        return 0;
    }
}

Vector random_to_sphere(RNG& rng, float radius, float distance_sq) {
    float r1 = rng.random_float();
    float r2 = rng.random_float();
    float z = 1.f + r2 * (std::sqrt(1.f - radius*radius/distance_sq) - 1.f);
    float phi = 2*PI*r1;
    float x = std::cos(phi) * std::sqrt(1 - z*z);
    float y = std::sin(phi) * std::sqrt(1 - z*z);
    return Vector(x, y, z);
}

Vector Sphere::random_direction(RNG& rng, const Vector& o) const {
    Vector v = center - o;
    return Axes(v.normalized()).from_local_to_world(random_to_sphere(rng, radius, v.squared_length()));
}


bool Moving_Sphere::hit(const Ray& ray, float tMin, float tMax, Intersection& hitRecord) const {
    return ray_sphere_intersect(get_center(ray.time), radius, ray, tMin, tMax, material, hitRecord);
}

Bounding_Box Moving_Sphere::boudning_box(float t0, float t1) const {
    Bounding_Box box_t0, box_t1;

    Vector center_t0 = get_center(t0);
    Vector center_t1 = get_center(t1);

    box_t0 = Bounding_Box(center_t0 - Vector(radius), center_t0 + Vector(radius));
    box_t1 = Bounding_Box(center_t1 - Vector(radius), center_t1 + Vector(radius));
    return Bounding_Box::get_union(box_t0, box_t1);
}
