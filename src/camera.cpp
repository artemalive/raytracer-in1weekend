#include "common.h"
#include "camera.h"
#include "random.h"

Camera::Camera(
    Vector look_from,
    Vector look_at,
    Vector up_direction,
    float vfov,
    float aspect,
    float aperture,
    float focus_distance,
    float time0,
    float time1
)
    : lens_radius(aperture / 2.0f)
    , focus_distance(focus_distance)
    , origin(look_from)
    , time0(time0)
    , time1(time1)
{
    forward_dir = (look_at - look_from).normalized();
    right_dir = cross_product(forward_dir, up_direction).normalized();
    up_dir = cross_product(right_dir, forward_dir);

    float theta = vfov * PI / 180.0f;
    float tn = std::tan(theta / 2.0f);
    half_width_vector = aspect * tn * right_dir;
    half_height_vector = tn * up_dir;
}

Ray Camera::get_ray(RNG& rng, float s, float t) const {
    Vector lens_point = lens_radius * random_point_in_unit_disk(rng);
    Vector origin_offset = right_dir * lens_point.x + up_dir * lens_point.y;

    float u = 2.f * s - 1.f;
    float v = 2.f * t - 1.f;

    Vector sample_vector = forward_dir + u * half_width_vector + v * half_height_vector;
    sample_vector *= focus_distance;

    float time = time0 + rng.random_float() * (time1 - time0);

    return Ray(origin + origin_offset, (sample_vector - origin_offset).normalized(), time);
}
