#pragma once

#include "random.h"
#include "vector.h"
#include <cmath>

inline float perlin_interpolation(Vector c[2][2][2], float u, float v, float w) {
    float uu = u * u*(3 - 2 * u);
    float vv = v * v*(3 - 2 * v);
    float ww = w * w*(3 - 2 * w);

    float accum = 0.f;

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 2; k++) {
                Vector weight_v(u-i, v-j, w-k);
                accum += (i*uu + (1-i)*(1.f-uu))*
                         (j*vv + (1-j)*(1.f-vv))*
                         (k*ww + (1-k)*(1.f-ww)) * dot_product(c[i][j][k], weight_v);
            }
        }
    }

    return accum;
}

static Vector* perlin_generate(RNG& rng) {
    Vector* p = new Vector[256];
    for (int i = 0; i < 256; i++) {
        p[i] = Vector(
            -1 + 2*rng.random_float(),
            -1 + 2*rng.random_float(),
            -1 + 2*rng.random_float()
        ).normalized();
    }
    return p;
}

void permute(RNG& rng, int* p, int n) {

    for (int i = n-1; i > 0; i--) {
        int target = int(rng.random_float() * (i+1));
        int tmp = p[i];
        p[i] = p[target];
        p[target] = tmp;
    }
}

static int* perlin_generate_perm(RNG& rng) {
    int* p = new int[256];
    for (int i = 0; i < 256; i++) {
        p[i] = i;
    }
    permute(rng, p, 256);
    return p;
}

class Perlin {
public:
    Perlin(RNG& rng) {
        if (random_vectors == nullptr) {
            random_vectors = perlin_generate(rng);
            perm_x = perlin_generate_perm(rng);
            perm_y = perlin_generate_perm(rng);
            perm_z = perlin_generate_perm(rng);
        }
    }

    float noise(const Vector& p) const {
        float u = p.x - std::floor(p.x);
        float v = p.y - std::floor(p.y);
        float w = p.z - std::floor(p.z);

        int i = static_cast<int>(std::floor(p.x));
        int j = static_cast<int>(std::floor(p.y));
        int k = static_cast<int>(std::floor(p.z));

        Vector c[2][2][2];
        for (int di = 0; di < 2; di++) {
            for (int dj = 0; dj < 2; dj++) {
                for (int dk = 0; dk < 2; dk++) {
                    c[di][dj][dk] = random_vectors[perm_x[(i + di)&255] ^ perm_y[(j + dj)&255] ^ perm_z[(k + dk)&255]];
                }
            }
        }
        return perlin_interpolation(c, u, v, w);
    }

    float turbulence(const Vector& p, int depth = 7) const {
        float accum = 0.f;
        Vector temp_p = p;
        float weight = 1.f;

        for (int i = 0; i < depth; i++) {
            accum += weight * noise(temp_p);
            weight *= 0.5f;
            temp_p *= 2.f;
        }

        return std::abs(accum);
    }

    static Vector* random_vectors;
    static int* perm_x;
    static int* perm_y;
    static int* perm_z;

    RNG rng;
};

Vector* Perlin::random_vectors;
int* Perlin::perm_x;
int* Perlin::perm_y;
int* Perlin::perm_z;
