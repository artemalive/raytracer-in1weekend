#pragma once

#include "random.h"
#include "ray.h"

static const float PI = 3.14159265358979323846f;

static Vector random_in_unit_disk()
{
    Vector p;
    do
    {
        p = 2.0f * Vector(RandomFloat(), RandomFloat(), 0.0f) - Vector(1, 1, 0);
    } while (dot_product(p, p) >= 1.0f);
    return p;
}

class Camera
{
public:
    Camera(
        Vector look_from, 
        Vector look_at,
        Vector up_direction,
        float vfov,
        float aspect,
        float aperture,
        float focus_distance
    )
        : lens_radius(aperture / 2.0f)
        , focus_distance(focus_distance)
        , origin(look_from)
    {
        forward_dir = (look_at - look_from).normalized();
        right_dir = cross_product(forward_dir, up_direction).normalized();
        up_dir = cross_product(right_dir, forward_dir);

        float theta = vfov * PI / 180.0f;
        float tn = std::tan(theta / 2.0f);
        half_width_vector = aspect * tn * right_dir;
        half_height_vector = tn * up_dir;
    }

    Ray get_ray(float s, float t) const
    {
        Vector lens_point = lens_radius * random_in_unit_disk();
        Vector origin_offset = right_dir * lens_point.x + up_dir * lens_point.y;

        float u = 2.f * s - 1.f;
        float v = 2.f * t - 1.f;

        Vector sample_vector = forward_dir + u * half_width_vector + v * half_height_vector;
        sample_vector *= focus_distance;

        return Ray(origin + origin_offset, (sample_vector - origin_offset).normalized());
    }

    float lens_radius;
    float focus_distance;

    Vector origin;
    Vector forward_dir;
    Vector right_dir;
    Vector up_dir;

    Vector half_width_vector;
    Vector half_height_vector;
};
