#pragma once

#include <cmath>
#include <cstdlib>
#include <iostream>

class Vector
{
public:
    Vector() {}
    Vector(float e0, float e1, float e2) { e[0] = e0; e[1] = e1; e[2] = e2; }

    float X() const { return e[0]; }
    float Y() const { return e[1]; }
    float Z() const { return e[2]; }

    float R() const { return e[0]; }
    float G() const { return e[1]; }
    float B() const { return e[2]; }

    const Vector& operator+() const { return *this; }
    Vector operator-() const { return Vector(-e[0], -e[1], -e[2]); }
    float operator[](int index) const { return e[index]; }
    float& operator[](int index) { return e[index]; }

    Vector& operator+=(const Vector& v2);
    Vector& operator-=(const Vector& v2);
    Vector& operator*=(const Vector& v2);
    Vector& operator/=(const Vector& v2);
    Vector& operator*=(float t);
    Vector& operator/=(float t);

    float Length() const { return sqrt(SquaredLength()); }
    float SquaredLength() const { return e[0]*e[0] + e[1]*e[1] + e[2]*e[2]; }
    void MakeUnitVector();

    float e[3];
};

inline Vector& Vector::operator+=(const Vector& v2)
{
    e[0] += v2.e[0];
    e[1] += v2.e[1];
    e[2] += v2.e[2];
    return *this;
}

inline Vector& Vector::operator-=(const Vector& v2)
{
    e[0] -= v2.e[0];
    e[1] -= v2.e[1];
    e[2] -= v2.e[2];
    return *this;
}

inline Vector& Vector::operator*=(const Vector& v2)
{
    e[0] *= v2.e[0];
    e[1] *= v2.e[1];
    e[2] *= v2.e[2];
    return *this;
}

inline Vector& Vector::operator/=(const Vector& v2)
{
    e[0] /= v2.e[0];
    e[1] /= v2.e[1];
    e[2] /= v2.e[2];
    return *this;
}

inline Vector& Vector::operator*=(float t)
{
    e[0] *= t;
    e[1] *= t;
    e[2] *= t;
    return *this;
}

inline Vector& Vector::operator/=(float t)
{
    e[0] /= t;
    e[1] /= t;
    e[2] /= t;
    return *this;
}

inline void Vector::MakeUnitVector()
{
    *this /= Length();
}

inline Vector operator+(const Vector& v1, const Vector& v2)
{
    return Vector(v1.e[0] + v2.e[0], v1.e[1] + v2.e[1], v1.e[2] + v2.e[2]);
}

inline Vector operator-(const Vector& v1, const Vector& v2)
{
    return Vector(v1.e[0] - v2.e[0], v1.e[1] - v2.e[1], v1.e[2] - v2.e[2]);
}

inline Vector operator*(const Vector& v1, const Vector& v2)
{
    return Vector(v1.e[0] * v2.e[0], v1.e[1] * v2.e[1], v1.e[2] * v2.e[2]);
}

inline Vector operator/(const Vector& v1, const Vector& v2)
{
    return Vector(v1.e[0] / v2.e[0], v1.e[1] / v2.e[1], v1.e[2] / v2.e[2]);
}

inline Vector operator*(const Vector& v, float t)
{
    return Vector(v.e[0] * t, v.e[1] * t, v.e[2] * t);
}

inline Vector operator*(float t, const Vector& v)
{
    return v * t;
}

inline Vector operator/(const Vector& v, float t)
{
    return Vector(v.e[0] / t, v.e[1] / t, v.e[2] / t);
}

inline float Dot(const Vector& v1, const Vector& v2)
{
    return v1.e[0]*v2.e[0] + v1.e[1]*v2.e[1] + v1.e[2]*v2.e[2];
}

inline Vector Cross(const Vector& v1, const Vector& v2)
{
    return Vector(
        v1.e[1]*v2.e[2] - v1.e[2]*v2.e[1],
        v1.e[2]*v2.e[0] - v1.e[0]*v2.e[2],
        v1.e[0]*v2.e[1] - v1.e[1]*v2.e[0]);
}

inline Vector UnitVector(const Vector& v)
{
    return v / v.Length();
}
