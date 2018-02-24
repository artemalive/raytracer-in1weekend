#pragma once

#include "bounding_box.h"
#include "hitable.h"
#include "random.h"
#include <algorithm>

class BVH_Node : public Hitable {
public:
    BVH_Node(){}

    BVH_Node(RNG& rng, Hitable** hitables, int n, float time0, float time1) {
        int axis = rng.random_uint32() % 3;
        std::sort(hitables, hitables + n, Compare_Along_Axis(axis));

        if (n == 1) {
            left = right = hitables[0];
        } else if (n == 2) {
            left = hitables[0];
            right = hitables[1];
        } else {
            left = new BVH_Node(rng, hitables, n/2, time0, time1);
            right = new BVH_Node(rng, hitables + n/2, n - n/2, time0, time1);
        }

        box = Bounding_Box::get_union(
            left->boudning_box(time0, time1),
            right->boudning_box(time0, time1)
        );
    }

    bool hit(const Ray& ray, float t_min, float t_max, Hit_Record& hit_record) const override {
        if (!box.intersect(ray, t_min, t_max))
            return false;

        Hit_Record left_hit_record;
        bool left_hit = left->hit(ray, t_min, t_max, left_hit_record);

        Hit_Record right_hit_record;
        bool right_hit = right->hit(ray, t_min, t_max, right_hit_record);

        if (!left_hit && !right_hit)
            return false;

        if (left_hit && right_hit) {
            hit_record = (left_hit_record.t < right_hit_record.t) ? left_hit_record : right_hit_record;
        } else if (left_hit) {
            hit_record = left_hit_record;
        } else {
            hit_record = right_hit_record;
        }
        return true;
    }

    Bounding_Box boudning_box(float t0, float t1) const override {
        return box;
    }

private:
    struct Compare_Along_Axis {
        int axis;
        Compare_Along_Axis(int axis) : axis(axis) {}
        bool operator()(const Hitable* a, const Hitable* b) const {
            return a->boudning_box(0, 0).min_point[axis] < b->boudning_box(0, 0).min_point[axis];
        }
    };

private:
    Hitable* left;
    Hitable* right;
    Bounding_Box box;
};
