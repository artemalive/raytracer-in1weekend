#include <algorithm>
#include <array>
#include <chrono>
#include <iostream>
#include <limits>
#include <memory>

#include "bvh.h"
#include "camera.h"
#include "material.h"
#include "perlin.h"
#include "random.h"
#include "sphere.h"
#include "hitable_list.h"
#include "scenes.h"
#include "thread.h"

Shape* light = new XZ_Rect(213, 343, 227, 332, 554, 0);
Shape* glass_sphere = new Sphere(Vector(190, 90, 190), 90, nullptr);

Shape* shapes_to_sample = nullptr;

Vector trace_ray(RNG& rng, const Ray& ray, const Shape* world, Shape* light_shape, int depth)
{
    Intersection hit;
    if (world->hit(ray, 0.001f, std::numeric_limits<float>::max(), hit))
    {
        Vector emitted = hit.material->emitted(ray, hit, hit.u, hit.v, hit.p);
        Scatter_Info scatter_info;
        if (depth < 50 && hit.material->scatter(rng, ray, hit, scatter_info))
        {
            if (scatter_info.is_specular) {
                return scatter_info.attenuation * trace_ray(rng, scatter_info.specular_ray, world, light_shape, depth + 1);
            } else {
                Shape_Pdf plight(light_shape, hit.p);
                Mixture_Pdf p(&plight, scatter_info.pdf);

                Ray scattered = Ray(hit.p, p.generate(rng), ray.time);
                float pdf = p.value(scattered.direction);

                delete scatter_info.pdf;

                return emitted + 
                    scatter_info.attenuation *
                    hit.material->scattering_pdf(ray, hit, scattered) *
                    trace_ray(rng, scattered, world, light_shape, depth + 1) / pdf;
            }
        }
        else
            return emitted;
    }
    else
        return Vector(0);
}

class Render_Rect_Task : public Task {
public:
	Render_Rect_Task(
        const Shape* world,
        const Camera* camera,
        int image_width,
        int image_height,
        int sample_count,
        int x1, int y1, int x2, int y2,
        std::vector<std::array<int, 3>>* results

    )
        : world(world)
        , camera(camera)
        , image_width(image_width)
        , image_height(image_height)
        , sample_count(sample_count)
        , x1(x1), y1(y1), x2(x2), y2(y2)
        , results(results)
    {}

	void run(RNG& rng) override {
        for (int j = y1; j < y2; j++) {
            for (int i = x1; i < x2; i++) {

                Vector color(0.0f, 0.0f, 0.0f);

                for (int s = 0; s < sample_count; s++) {
                    float u = (float(i) + rng.random_float()) / float(image_width);
                    float v = (float(j) + rng.random_float()) / float(image_height);

                    Ray ray = camera->get_ray(rng, u, v);
                    color += trace_ray(rng, ray, world, shapes_to_sample, 0);
                }

                color /= float(sample_count);

                color[0] = clamp(color[0], 0, 1);
                color[1] = clamp(color[1], 0, 1);
                color[2] = clamp(color[2], 0, 1);
                color = Vector(std::sqrt(color[0]), std::sqrt(color[1]), std::sqrt(color[2]));
                
                int ir = static_cast<int>(255.99 * color[0]);
                int ig = static_cast<int>(255.99 * color[1]);
                int ib = static_cast<int>(255.99 * color[2]);
                (*results)[j * image_width + i] = {ir, ig, ib};
            }
        }
    }

private:
    const Shape* world;
	const Camera* camera;
    int image_width, image_height;
    int sample_count;
	int x1, y1;
	int	x2, y2;

    std::vector<std::array<int, 3>>* results;
};

int main()
{
    const int nx = 1280;
    const int ny = 720;
    const int ns = 64;

    float aspect = float(nx) / float(ny);

    std::cout << "P3\n" << nx << " " << ny << "\n255\n";

    RNG rng;
    perlin_initialize(rng);

    float time0 = 0.f;
    float time1 = 1.f;

    //Shape* world = random_scene(time0, time1);
    //Shape* world = two_spheres();
    //Shape* world = cornell_box();
    //Shape* world = cornell_smoke(rng);
    //Shape* world = final_scene(rng);

    Shape* shapes[2] { light, glass_sphere };
    shapes_to_sample = new HitableList(shapes, 2);

    Scene scene = cornell_box(aspect);

    Timestamp t;

    //Vector lookFrom(478, 278, -600);
    //Vector lookAt(278, 278, 0);
    //float distToFocus = 10.0f;
    //float apperture = 0.0f; //0.1f;
    //float vfov = 40.0f;
   

    std::vector<std::array<int, 3>> result(nx * ny);
    int size = 32;

    std::vector<Render_Rect_Task> tasks;
    for (int y = 0; y < ny; y += size) {
        for (int x = 0; x < nx; x += size) {
            tasks.push_back(Render_Rect_Task(scene.shape, &scene.camera, nx, ny, ns, x, y, std::min(x + size, nx), std::min(y + size, ny), &result));
        }
    }

    std::vector<std::unique_ptr<Thread>> threads;
    SYSTEM_INFO si;
	::GetSystemInfo(&si);
	int num_processors = si.dwNumberOfProcessors;
	for (int i = 0; i < num_processors - 1; i++) {
		threads.push_back(std::make_unique<Thread>(i));
	}

    for (auto& task : tasks) {
		Thread::commit_task(&task);
	}
	Thread::wait_for_tasks();

    for (int j = ny - 1; j >= 0; j--)
    {
        for (int i = 0; i < nx; i++)
        {
            int index = j * nx + i;
            int ir = result[index][0];
            int ig = result[index][1];
            int ib = result[index][2];
            std::cout << ir << " " << ig << " " << ib << "\n";
        }
    }

    int64_t time = elapsed_milliseconds(t);
    fprintf(stderr, "Time = %.2fs\n", time / 1000.0f);
}
