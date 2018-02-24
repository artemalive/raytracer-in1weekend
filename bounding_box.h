#pragma once

#include "ray.h"

#include <algorithm>
#include <limits>

class Bounding_Box {
public:
    Vector min_point;
    Vector max_point;

    Bounding_Box()
        : min_point(Vector(std::numeric_limits<float>::infinity()))
        , max_point(Vector(-std::numeric_limits<float>::infinity()))
    {}

    Bounding_Box(Vector min_point, Vector max_point)
        : min_point(min_point)
        , max_point(max_point)
    {}

    explicit Bounding_Box(Vector point)
        : max_point(point)
        , min_point(point)
    {}

    void extend(Vector point) {
        min_point.x = std::min(min_point.x, point.x);
        min_point.y = std::min(min_point.y, point.y);
        min_point.z = std::min(min_point.z, point.z);

        max_point.x = std::max(max_point.x, point.x);
        max_point.y = std::max(max_point.y, point.y);
        max_point.z = std::max(max_point.z, point.z);
    }

    bool contain(Vector point) const {
        return  point[0] >= min_point[0] && point[0] <= max_point[0] &&
                point[1] >= min_point[1] && point[1] <= max_point[1] &&
                point[2] >= min_point[2] && point[2] <= max_point[2];
    }

    bool intersect(const Ray& ray, float t_min, float t_max) const {
        for (int i = 0; i < 3; i++) {
            float inv_dir = 1.f / ray.direction[i];
            float t0 = (min_point[i] - ray.origin[i]) * inv_dir;
            float t1 = (max_point[i] - ray.origin[i]) * inv_dir;

            if (inv_dir < 0.f)
                std::swap(t0, t1);

            t_min = t0 > t_min ? t0 : t_min;
            t_max = t1 < t_max ? t1 : t_max;
            if (t_min > t_max)
                return false;
        }
        return true;
    }

    static Bounding_Box get_union(const Bounding_Box& bounds, const Bounding_Box& bounds2)
    {
        return Bounding_Box(
            Vector(std::min(bounds.min_point.x, bounds2.min_point.x),
                std::min(bounds.min_point.y, bounds2.min_point.y),
                std::min(bounds.min_point.z, bounds2.min_point.z)),
            Vector(std::max(bounds.max_point.x, bounds2.max_point.x),
                std::max(bounds.max_point.y, bounds2.max_point.y),
                std::max(bounds.max_point.z, bounds2.max_point.z)));
    }

    static Bounding_Box get_intersection(const Bounding_Box& bounds, const Bounding_Box& bounds2)
    {
        Vector minp;
        minp.x = std::max(bounds.min_point.x, bounds2.min_point.x);
        minp.y = std::max(bounds.min_point.y, bounds2.min_point.y);
        minp.z = std::max(bounds.min_point.z, bounds2.min_point.z);

        Vector maxp;
        maxp.x = std::min(bounds.max_point.x, bounds2.max_point.x);
        maxp.y = std::min(bounds.max_point.y, bounds2.max_point.y);
        maxp.z = std::min(bounds.max_point.z, bounds2.max_point.z);

        return Bounding_Box(minp, maxp);
    }
};
