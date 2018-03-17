#pragma once

#include "shape.h"
#include <cassert>

class HitableList : public Hitable
{
public:
    HitableList() {}
    HitableList(Hitable** list, int listSize) : list(list), listSize(listSize) {}

    bool hit(const Ray& ray, float tMin, float tMax, Hit_Record& hitRecord) const override;

    Bounding_Box boudning_box(float t0, float t1) const override {
        assert(!"should not be called");
        return Bounding_Box();
    }

    Hitable** list;
    int listSize;
};

inline bool HitableList::hit(const Ray& ray, float tMin, float tMax, Hit_Record& hitRecord) const
{
    Hit_Record tempHit;
    bool hitAnything = false;
    float closestSoFar = tMax;

    for (int i = 0; i < listSize; i++)
    {
        if (list[i]->hit(ray, tMin, closestSoFar, tempHit))
        {
            hitAnything = true;
            closestSoFar = tempHit.t;
            hitRecord = tempHit;
        }
    }
    return hitAnything;
}
