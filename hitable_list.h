#pragma once

#include "hitable.h"

class HitableList : public Hitable
{
public:
    HitableList() {}
    HitableList(Hitable** list, int listSize) : list(list), listSize(listSize) {}

    bool Hit(const Ray& ray, float tMin, float tMax, HitRecord& hitRecord) const override;

    Hitable** list;
    int listSize;
};

inline bool HitableList::Hit(const Ray& ray, float tMin, float tMax, HitRecord& hitRecord) const
{
    HitRecord tempHit;
    bool hitAnything = false;
    float closestSoFar = tMax;

    for (int i = 0; i < listSize; i++)
    {
        if (list[i]->Hit(ray, tMin, closestSoFar, tempHit))
        {
            hitAnything = true;
            closestSoFar = tempHit.t;
            hitRecord = tempHit;
        }
    }
    return hitAnything;
}
