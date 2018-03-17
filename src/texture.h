#pragma once

#include "perlin.h"
#include "../random.h"
#include "../vector.h"

class Texture {
public:
    virtual Vector value(float u, float v, const Vector& p) const = 0;
};

class Constant_Texture : public Texture {
public:
    Constant_Texture(Vector color) : color(color) {}
    Vector value(float, float, const Vector&) const override;

private:
    Vector color;
};

class Checker_Texture : public Texture {
public:
    Checker_Texture(Texture* odd, Texture* even) : odd(odd), even(even) {}
    Vector value(float, float, const Vector&) const override;

private:
    Texture* odd;
    Texture* even;
};

class Noise_Texture : public Texture {
public:
    Noise_Texture(float scale) : scale(scale) {}
    Vector value(float u, float v, const Vector& p) const override;

private:
    float scale;
};

class Image_Texture : public Texture {
public:
    Image_Texture(unsigned char* pixels, int w, int h) : pixels(pixels), w(w), h(h) {}
    Vector value(float u, float v, const Vector& p) const override;

private:
    unsigned char* pixels;
    int w, h;
};
