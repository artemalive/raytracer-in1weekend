#pragma once

#include "shape.h"
#include "texture.h"
#include "hitable_list.h"
#include "material.h"
#include "sphere.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../third_party//stb_image.h"

Shape* two_spheres() {
    Texture* checker = new Checker_Texture(
        new Constant_Texture(Vector(0.2f, 0.3f, 0.1f)),
        new Constant_Texture(Vector(0.9f, 0.9f, 0.9f))
    );

    int n = 50;
    Shape** list = new Shape*[n+1];
    list[0] = new Sphere(Vector(0, -10, 0), 10, new Lambertian(checker));
    list[1] = new Sphere(Vector(0, 10, 0), 10, new Lambertian(checker));

    return new HitableList(list, 2);
}

Shape* two_perlin_spheres() {
    int w, h, c;
    unsigned char* pixels = stbi_load("texture.jpg", &w, &h, &c, STBI_rgb);

    Texture* perlin_texture = new Noise_Texture(5.f);
    Shape** list = new Shape*[2];
    list[0] = new Sphere(Vector(0, -1000, 0), 1000, new Lambertian(perlin_texture));
    list[1] = new Sphere(Vector(0, 2, 0), 2, new Lambertian(new Image_Texture(pixels, w, h)/*perlin_texture*/));
    return new HitableList(list, 2);
}

Shape* simple_light() {
    int w, h, c;
    unsigned char* pixels = stbi_load("texture.jpg", &w, &h, &c, STBI_rgb);
    Texture* image_texture = new Image_Texture(pixels, w, h);

    Texture* perlin_texture = new Noise_Texture(4.f);

    Shape** list = new Shape*[4];
    list[0] = new Sphere(Vector(0, -1000, 0), 1000, new Lambertian(perlin_texture));
    list[1] = new Sphere(Vector(0, 2, 0), 2, new Lambertian(image_texture));
    list[2] = new Sphere(Vector(0, 7, -1), 2, new Diffuse_Light(new Constant_Texture(Vector(4, 4, 4))));
    list[3] = new XY_Rect(3, 5, 1, 3, -2, new Diffuse_Light(new Constant_Texture(Vector(4, 4, 4))));
    return new HitableList(list, 4);
}

Shape* cornell_box() {
    Shape** list = new Shape*[8];
    
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

Shape* cornell_smoke(RNG& rng) {
    Shape** list = new Shape*[8];
    
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

Shape* random_scene(float time0, float time1)
{
    const int n = 500;
    Shape** list = new Shape*[n + 1];

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

Shape* final_scene(RNG& rng) {
    rng.random_float();

    int nb = 20;
    Shape** list = new Shape*[30];
    Shape** boxlist = new Shape*[10000];
    Shape** boxlist2 = new Shape*[10000];
    Material* white = new Lambertian(new Constant_Texture(Vector(0.73f)));
    Material* ground = new Lambertian(new Constant_Texture(Vector(0.48f, 0.83f, 0.53f)));
    int b = 0;

    for (int i = 0; i < nb; i++) {
        for (int j = 0; j < nb; j++) {
            float w = 100;
            float x0 = -1000 + i*w;
            float z0 = -1000 + j*w;
            float y0 = 0;
            float x1 = x0 + w;
            float y1 = 100 * (rng.random_float() + 0.01f);
            float z1 = z0 + w;
            boxlist[b++] = new Box(Vector(x0, y0, z0), Vector(x1, y1, z1), ground);
        }
    }

    int l = 0;

    list[l++] = new BVH_Node(rng, boxlist, b, 0, 1);

    Material* light = new Diffuse_Light(new Constant_Texture(Vector(7)));
    list[l++] = new XZ_Rect(123, 423, 147, 412, 554, light);

    Vector center(400, 400, 200);
    list[l++] = new Moving_Sphere(center, center + Vector(30, 0, 0), 0, 1, 50,
        new Lambertian(new Constant_Texture(Vector(0.7f, 0.3f, 0.1f))));

    list[l++] = new Sphere(Vector(260, 150, 45), 50, new Dielectric(1.5f));
    list[l++] = new Sphere(Vector(0, 150, 145), 50, new Metal(Vector(0.8f, 0.8f, 0.9f), 10.f));

    Shape* boundary = new Sphere(Vector(360, 150, 145), 70, new Dielectric(1.5f));
    list[l++] = boundary;
    list[l++] = new Constant_Medium(boundary, 0.2f, new Constant_Texture(Vector(0.2f, 0.4f, 0.9f)));

    boundary = new Sphere(Vector(0), 5000, new Dielectric(1.5));
    list[l++] = new Constant_Medium(boundary, 1e-4f, new Constant_Texture(Vector(1)));

    int nx, ny, nn;
    unsigned char* tex_data = stbi_load("texture.jpg", &nx, &ny, &nn, STBI_rgb);
    Material* mat = new Lambertian(new Image_Texture(tex_data, nx, ny));
    list[l++] = new Sphere(Vector(400, 200, 400), 100, mat);

    Texture* perlin_tex = new Noise_Texture(0.1f);
    list[l++] = new Sphere(Vector(220, 280, 300), 80, new Lambertian(perlin_tex));

    int ns = 1000;
    for (int j = 0; j < ns; j++) {
        boxlist2[j] = new Sphere(
            Vector(165.f * rng.random_float(),
                   165.f * rng.random_float(),
                   165.f * rng.random_float()),
            10.f, white);
    }
    list[l++] = new Translate(new Rotate_Y(new BVH_Node(rng, boxlist2, ns, 0, 1), 15.f), Vector(-100, 270, 395));

    return new HitableList(list, l);
}