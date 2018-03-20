#include "perlin.h"
#include "texture.h"
#include <algorithm>
#include <cassert>

Vector Constant_Texture::value(float, float, const Vector&) const {
    return color;
}

Vector Checker_Texture::value(float u, float v, const Vector& p) const {
    float sines = std::sin(10 * p.x) * std::sin(10 * p.y) * std::sin(10 * p.z);
    if (sines < 0.f)
        return odd->value(u, v, p);
    else
        return even->value(u, v, p);
}

Vector Noise_Texture::value(float u, float v, const Vector& p) const {
    //float f = 0.5f * (perlin_noise(p * scale) + 1.f);
    //float f = perlin_turbulence(p * scale);
    float f = 0.5f * (1.f + std::sin(scale*p.x + 5.f * perlin_turbulence(scale*p)));
    return Vector(1) * f;
}

Vector Image_Texture::value(float u, float v, const Vector& p) const {
    assert(u >= 0.f && u <= 1.f);
    assert(v >= 0.f && v <= 1.f);

    int x = static_cast<int>(u * w);
    int y = static_cast<int>((1.f - v) * h);

    assert(x >= 0 && x <= w);
    assert(y >= 0 && y <= h);

    x = std::min(x, w - 1);
    y = std::min(y, h - 1);

    static constexpr float norm_coeff = 1 / 255.f;

    auto pixel = &pixels[3*(x + w*y)];
    return Vector(pixel[0], pixel[1], pixel[2]) * norm_coeff;
}
