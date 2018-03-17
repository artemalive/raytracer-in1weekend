#pragma once

#include "ray.h"

class RNG;

class Camera {
public:
    Camera(
        Vector look_from, 
        Vector look_at,
        Vector up_direction,
        float vfov,
        float aspect,
        float aperture,
        float focus_distance,
        float time0,
        float time1
    );

    Ray get_ray(RNG& rng, float s, float t) const;

private:
    float lens_radius;
    float focus_distance;

    float time0;
    float time1;

    Vector origin;
    Vector forward_dir;
    Vector right_dir;
    Vector up_dir;

    Vector half_width_vector;
    Vector half_height_vector;
};
