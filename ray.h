#pragma once

#include "vec3.h"

class Ray
{
public:
    Ray() {}
    Ray(const Vector& a, const Vector& b) : a(a), b(b) {}

    const Vector& Origin() const { return a; }
    const Vector& Direction() const { return b; }
    Vector PointAtParameter(float t) const { return a + t*b; }
    
    Vector a;
    Vector b;
};
