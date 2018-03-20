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

struct Timestamp {
    Timestamp() : t(std::chrono::steady_clock::now()) {}
    const std::chrono::time_point<std::chrono::steady_clock> t;
};

int64_t elapsed_milliseconds(Timestamp timestamp) {
    auto duration = std::chrono::steady_clock::now() - timestamp.t;
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    return static_cast<int64_t>(milliseconds);
}

Vector calculate_color(RNG& rng, const Ray& ray, const Hitable* world, int depth)
{
    Intersection hit;
    if (world->hit(ray, 0.001f, std::numeric_limits<float>::max(), hit))
    {
        Ray scattered;
        Vector attenuation;
        Vector emitted = hit.material->emitted(hit.u, hit.v, hit.p);
        if (depth < 50 && hit.material->scatter(rng, ray, hit, attenuation, scattered))
        {
            return emitted + attenuation * calculate_color(rng, scattered, world, depth + 1);
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
        const Hitable* world,
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
                    color += calculate_color(rng, ray, world, 0);
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
    const Hitable* world;
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

    std::cout << "P3\n" << nx << " " << ny << "\n255\n";

    RNG rng;
    perlin_initialize(rng);

    float time0 = 0.f;
    float time1 = 1.f;

    //Hitable* world = random_scene(time0, time1);
    //Hitable* world = two_spheres();
    //Hitable* world = cornell_box();
    //Hitable* world = cornell_smoke(rng);
    Hitable* world = final_scene(rng);

    Timestamp t;

    Vector lookFrom(478, 278, -600);
    Vector lookAt(278, 278, 0);
    float distToFocus = 10.0f;
    float apperture = 0.0f; //0.1f;
    float vfov = 40.0f;
    
    Camera camera(lookFrom, lookAt, Vector(0, 1, 0), vfov, float(nx) / float(ny), apperture, distToFocus, time0, time1);

    std::vector<std::array<int, 3>> result(nx * ny);
    int size = 32;

    std::vector<Render_Rect_Task> tasks;
    for (int y = 0; y < ny; y += size) {
        for (int x = 0; x < nx; x += size) {
            tasks.push_back(Render_Rect_Task(world, &camera, nx, ny, ns, x, y, std::min(x + size, nx), std::min(y + size, ny), &result));
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
