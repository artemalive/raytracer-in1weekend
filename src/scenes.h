#pragma once

#include "shape.h"
#include "texture.h"
#include "hitable_list.h"
#include "material.h"
#include "sphere.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../third_party//stb_image.h"

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
    int w, h, c;
    unsigned char* pixels = stbi_load("texture.jpg", &w, &h, &c, STBI_rgb);

    Texture* perlin_texture = new Noise_Texture(5.f);
    Hitable** list = new Hitable*[2];
    list[0] = new Sphere(Vector(0, -1000, 0), 1000, new Lambertian(perlin_texture));
    list[1] = new Sphere(Vector(0, 2, 0), 2, new Lambertian(new Image_Texture(pixels, w, h)/*perlin_texture*/));
    return new HitableList(list, 2);
}

Hitable* simple_light() {
    int w, h, c;
    unsigned char* pixels = stbi_load("texture.jpg", &w, &h, &c, STBI_rgb);
    Texture* image_texture = new Image_Texture(pixels, w, h);

    Texture* perlin_texture = new Noise_Texture(4.f);

    Hitable** list = new Hitable*[4];
    list[0] = new Sphere(Vector(0, -1000, 0), 1000, new Lambertian(perlin_texture));
    list[1] = new Sphere(Vector(0, 2, 0), 2, new Lambertian(image_texture));
    list[2] = new Sphere(Vector(0, 7, -1), 2, new Diffuse_Light(new Constant_Texture(Vector(4, 4, 4))));
    list[3] = new XY_Rect(3, 5, 1, 3, -2, new Diffuse_Light(new Constant_Texture(Vector(4, 4, 4))));
    return new HitableList(list, 4);
}

Hitable* cornell_box() {
    Hitable** list = new Hitable*[8];
    
    Material* red = new Lambertian(new Constant_Texture(Vector(0.65f, 0.05f, 0.05f)));
    Material* white = new Lambertian(new Constant_Texture(Vector(0.73f, 0.73f, 0.73f)));
    Material* green = new Lambertian(new Constant_Texture(Vector(0.12f, 0.45f, 0.15f)));
    Material* light = new Diffuse_Light(new Constant_Texture(Vector(15, 15, 15)));

    list[0] = new Flip_Normals(new YZ_Rect(0, 555, 0, 555, 555, green));
    list[1] = new YZ_Rect(0, 555, 0, 555, 0, red);
    list[2] = new XZ_Rect(213, 343, 227, 332, 554, light);
    list[3] = new Flip_Normals(new XZ_Rect(0, 555, 0, 555, 555, white));
    list[4] = new XZ_Rect(0, 555, 0, 555, 0, white);
    list[5] = new Flip_Normals(new XY_Rect(0, 555, 0, 555, 555, white));

    list[6] = new Translate(
                    new Rotate_Y(
                        new Box(Vector(0), Vector(165, 165, 165), white),
                        -18),
                    Vector(130, 0, 65));

    list[7] = new Translate(
                new Rotate_Y(
                    new Box(Vector(0), Vector(165, 330, 165), white),
                    15),
                Vector(265, 0, 295));

    return new HitableList(list, 8);
}

Hitable* cornell_smoke(RNG& rng) {
    Hitable** list = new Hitable*[8];
    
    Material* red = new Lambertian(new Constant_Texture(Vector(0.65f, 0.05f, 0.05f)));
    Material* white = new Lambertian(new Constant_Texture(Vector(0.73f, 0.73f, 0.73f)));
    Material* green = new Lambertian(new Constant_Texture(Vector(0.12f, 0.45f, 0.15f)));
    Material* light = new Diffuse_Light(new Constant_Texture(Vector(7, 7, 7)));

    list[0] = new Flip_Normals(new YZ_Rect(0, 555, 0, 555, 555, green));
    list[1] = new YZ_Rect(0, 555, 0, 555, 0, red);
    list[2] = new XZ_Rect(113, 443, 127, 432, 554, light);
    list[3] = new Flip_Normals(new XZ_Rect(0, 555, 0, 555, 555, white));
    list[4] = new XZ_Rect(0, 555, 0, 555, 0, white);
    list[5] = new Flip_Normals(new XY_Rect(0, 555, 0, 555, 555, white));

    auto b1 = new Translate(
                    new Rotate_Y(
                        new Box(Vector(0), Vector(165, 165, 165), white),
                        -18),
                    Vector(130, 0, 65));

    auto b2 = new Translate(
                new Rotate_Y(
                    new Box(Vector(0), Vector(165, 330, 165), white),
                    15),
                Vector(265, 0, 295));

    list[6] = new Constant_Medium(b1, 0.01f, new Constant_Texture(Vector(1.f)));
    list[7] = new Constant_Medium(b2, 0.01f, new Constant_Texture(Vector(0.f)));

    return new HitableList(list, 8);
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