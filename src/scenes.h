#pragma once

#include "bvh.h"
#include "camera.h"
#include "shape.h"
#include "texture.h"
#include "hitable_list.h"
#include "material.h"
#include "sphere.h"

struct Scene {
    Shape* shape;
    Camera camera;
};

Scene cornell_box(float aspect);

inline Shape* two_spheres() {
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





//inline Shape* cornell_smoke(RNG& rng) {
//    Shape** list = new Shape*[8];
//    
//    Material* red = new Lambertian(new Constant_Texture(Vector(0.65f, 0.05f, 0.05f)));
//    Material* white = new Lambertian(new Constant_Texture(Vector(0.73f, 0.73f, 0.73f)));
//    Material* green = new Lambertian(new Constant_Texture(Vector(0.12f, 0.45f, 0.15f)));
//    Material* light = new Diffuse_Light(new Constant_Texture(Vector(7, 7, 7)));
//
//    list[0] = new Flip_Normals(new YZ_Rect(0, 555, 0, 555, 555, green));
//    list[1] = new YZ_Rect(0, 555, 0, 555, 0, red);
//    list[2] = new XZ_Rect(113, 443, 127, 432, 554, light);
//    list[3] = new Flip_Normals(new XZ_Rect(0, 555, 0, 555, 555, white));
//    list[4] = new XZ_Rect(0, 555, 0, 555, 0, white);
//    list[5] = new Flip_Normals(new XY_Rect(0, 555, 0, 555, 555, white));
//
//    auto b1 = new Translate(
//                    new Rotate_Y(
//                        new Box(Vector(0), Vector(165, 165, 165), white),
//                        -18),
//                    Vector(130, 0, 65));
//
//    auto b2 = new Translate(
//                new Rotate_Y(
//                    new Box(Vector(0), Vector(165, 330, 165), white),
//                    15),
//                Vector(265, 0, 295));
//
//    list[6] = new Constant_Medium(b1, 0.01f, new Constant_Texture(Vector(1.f)));
//    list[7] = new Constant_Medium(b2, 0.01f, new Constant_Texture(Vector(0.f)));
//
//    return new HitableList(list, 8);
//}

//inline Shape* random_scene(float time0, float time1)
//{
//    const int n = 500;
//    Shape** list = new Shape*[n + 1];
//
//    Texture* checker = new Checker_Texture(
//        new Constant_Texture(Vector(0.2f, 0.3f, 0.1f)),
//        new Constant_Texture(Vector(0.9f, 0.9f, 0.9f))
//    );
//    list[0] = new Sphere(Vector(0, -1000, 0), 1000, new Lambertian(checker));
//
//    RNG rng;
//
//    int i = 1;
//    for (int a = -11; a < 11; a++)
//    {
//        for (int b = -11; b < 11; b++)
//        {
//            float chooseMat = rng.random_float();
//            Vector center(a + 0.9f*rng.random_float(), 0.2f, b + 0.9f*rng.random_float());
//            if ((center - Vector(4, 0.2f, 0)).length() > 0.9f)
//            {
//                if (chooseMat < 0.8f)
//                {
//                    /*list[i++] = new Moving_Sphere(center, center + Vector(0, 0.5f*rng.random_float(), 0), 0.f, 1.f,
//                        0.2f,
//                        new Lambertian(
//                            Vector(rng.random_float()*rng.random_float(), rng.random_float()*rng.random_float(), rng.random_float()*rng.random_float())
//                        )
//                    );*/
//                    list[i++] = new Sphere(center,
//                        0.2f,
//                        new Lambertian(
//                            new Constant_Texture(Vector(rng.random_float()*rng.random_float(), rng.random_float()*rng.random_float(), rng.random_float()*rng.random_float()))
//                        )
//                    );
//                }
//                else if (chooseMat < 0.95)
//                {
//                    list[i++] = new Sphere(center, 0.2f, new Metal(
//                        Vector(0.5f*(1 + rng.random_float()), 0.5f*(1 + rng.random_float()), 0.5f*(1 + rng.random_float())), 0.5f*rng.random_float()));
//                }
//                else
//                {
//                    list[i++] = new Sphere(center, 0.2f, new Dielectric(1.5f));
//                }
//            }
//        }
//    }
//
//    list[i++] = new Sphere(Vector(0, 1, 0), 1.0f, new Dielectric(1.5f));
//    list[i++] = new Sphere(Vector(-4, 1, 0), 1.0f, new Lambertian(new Constant_Texture(Vector(0.4f, 0.2f, 0.1f))));
//    list[i++] = new Sphere(Vector(4, 1, 0), 1.0f, new Metal(Vector(0.7f, 0.6f, 0.5f), 0.0));
//
//    return new BVH_Node(rng, list, i, time0, time1);
//
//   // return new HitableList(list, i);
//}
