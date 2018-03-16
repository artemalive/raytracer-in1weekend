#pragma once

#include "hitable.h"
#include "hitable_list.h"
#include "material.h"
#include "sphere.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Hitable* two_spheres() {
    Texture* checker = new Checker_Texture(
        new Constant_Texture(Vector(0.2f, 0.3f, 0.1f)),
        new Constant_Texture(Vector(0.9f, 0.9f, 0.9f))
    );

    int n = 50;
    Hitable** list = new Hitable*[n+1];
    list[0] = new Sphere(Vector(0, -10, 0), 10, new Lambertian(checker));
    list[1] = new Sphere(Vector(0, 10, 0), 10, new Lambertian(checker));

    return new HitableList(list, 2);
}

Hitable* two_perlin_spheres() {
    RNG rng;

    int w, h, c;
    unsigned char* pixels = stbi_load("texture.jpg", &w, &h, &c, STBI_rgb);

    Texture* perlin_texture = new Noise_Texture(rng, 5.f);
    Hitable** list = new Hitable*[2];
    list[0] = new Sphere(Vector(0, -1000, 0), 1000, new Lambertian(perlin_texture));
    list[1] = new Sphere(Vector(0, 2, 0), 2, new Lambertian(new Image_Texture(pixels, w, h)/*perlin_texture*/));
    return new HitableList(list, 2);
}

Hitable* random_scene(float time0, float time1)
{
    const int n = 500;
    Hitable** list = new Hitable*[n + 1];

    Texture* checker = new Checker_Texture(
        new Constant_Texture(Vector(0.2f, 0.3f, 0.1f)),
        new Constant_Texture(Vector(0.9f, 0.9f, 0.9f))
    );
    list[0] = new Sphere(Vector(0, -1000, 0), 1000, new Lambertian(checker));

    RNG rng;

    int i = 1;
    for (int a = -11; a < 11; a++)
    {
        for (int b = -11; b < 11; b++)
        {
            float chooseMat = rng.random_float();
            Vector center(a + 0.9f*rng.random_float(), 0.2f, b + 0.9f*rng.random_float());
            if ((center - Vector(4, 0.2f, 0)).length() > 0.9f)
            {
                if (chooseMat < 0.8f)
                {
                    /*list[i++] = new Moving_Sphere(center, center + Vector(0, 0.5f*rng.random_float(), 0), 0.f, 1.f,
                        0.2f,
                        new Lambertian(
                            Vector(rng.random_float()*rng.random_float(), rng.random_float()*rng.random_float(), rng.random_float()*rng.random_float())
                        )
                    );*/
                    list[i++] = new Sphere(center,
                        0.2f,
                        new Lambertian(
                            new Constant_Texture(Vector(rng.random_float()*rng.random_float(), rng.random_float()*rng.random_float(), rng.random_float()*rng.random_float()))
                        )
                    );
                }
                else if (chooseMat < 0.95)
                {
                    list[i++] = new Sphere(center, 0.2f, new Metal(
                        Vector(0.5f*(1 + rng.random_float()), 0.5f*(1 + rng.random_float()), 0.5f*(1 + rng.random_float())), 0.5f*rng.random_float()));
                }
                else
                {
                    list[i++] = new Sphere(center, 0.2f, new Dielectric(1.5f));
                }
            }
        }
    }

    list[i++] = new Sphere(Vector(0, 1, 0), 1.0f, new Dielectric(1.5f));
    list[i++] = new Sphere(Vector(-4, 1, 0), 1.0f, new Lambertian(new Constant_Texture(Vector(0.4f, 0.2f, 0.1f))));
    list[i++] = new Sphere(Vector(4, 1, 0), 1.0f, new Metal(Vector(0.7f, 0.6f, 0.5f), 0.0));

    return new BVH_Node(rng, list, i, time0, time1);

   // return new HitableList(list, i);
}