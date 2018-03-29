#pragma once

#include "shape.h"
#include <cassert>

class HitableList : public Shape
{
public:
    HitableList() {}
    HitableList(Shape** list, int listSize) : list(list), listSize(listSize) {}

    bool hit(const Ray& ray, float tMin, float tMax, Intersection& hitRecord) const override;

    Bounding_Box boudning_box(float t0, float t1) const override {
        assert(!"should not be called");
        return Bounding_Box();
    }

    Shape** list;
    int listSize;
};

inline bool HitableList::hit(const Ray& ray, float tMin, float tMax, Intersection& hitRecord) const
{
    Intersection tempHit;
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
