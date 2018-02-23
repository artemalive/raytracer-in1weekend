#include <algorithm>
#include <array>
#include <chrono>
#include <iostream>
#include <limits>
#include <memory>

#include "camera.h"
#include "material.h"
#include "random.h"
#include "sphere.h"
#include "hitable_list.h"
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

Vector Color(RNG& rng, const Ray& ray, const Hitable* world, int depth)
{
    HitRecord hitRecord;
    if (world->Hit(ray, 0.001f, std::numeric_limits<float>::max(), hitRecord))
    {
        Ray scattered;
        Vector attenuation;
        if (depth < 50 && hitRecord.material->Scatter(rng, ray, hitRecord, attenuation, scattered))
        {
            return attenuation * Color(rng, scattered, world, depth + 1);
        }
        return Vector(0, 0, 0);
    }

    float t = 0.5f * (ray.direction.y + 1.0f);
    return (1.0f - t)*Vector(1.0f, 1.0f, 1.0f) + t*Vector(0.5f, 0.7f, 1.0f);
}

Hitable* RandomScene()
{
    const int n = 500;
    Hitable** list = new Hitable*[n + 1];
    list[0] = new Sphere(Vector(0, -1000, 0), 1000, new Lambertian(Vector(0.5, 0.5, 0.5)));

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
                    list[i++] = new Moving_Sphere(center, center + Vector(0, 0.5f*rng.random_float(), 0), 0.f, 1.f,
                        0.2f,
                        new Lambertian(
                            Vector(rng.random_float()*rng.random_float(), rng.random_float()*rng.random_float(), rng.random_float()*rng.random_float())
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
    list[i++] = new Sphere(Vector(-4, 1, 0), 1.0f, new Lambertian(Vector(0.4f, 0.2f, 0.1f)));
    list[i++] = new Sphere(Vector(4, 1, 0), 1.0f, new Metal(Vector(0.7f, 0.6f, 0.5f), 0.0));

    return new HitableList(list, i);
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
                    color += Color(rng, ray, world, 0);
                }

                color /= float(sample_count);
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
    const int ns = 2;

    std::cout << "P3\n" << nx << " " << ny << "\n255\n";

   /* Sphere sphere1(Vector(0, 0, -1), 0.5f, new Lambertian(Vector(0.1f, 0.2f, 0.5f)));
    Sphere sphere2(Vector(0, -100.5f, -1), 100, new Lambertian(Vector(0.8f, 0.8f, 0.0f)));
    Sphere sphere3(Vector(1, 0, -1), 0.5f, new Metal(Vector(0.8f, 0.6f, 0.2f), 1.0f));
    Sphere sphere4(Vector(-1.0, 0, -1), 0.5f, new Dielectric(1.5f));
    Sphere sphere5(Vector(-1.0, 0, -1), -0.45f, new Dielectric(1.5f));

    Hitable* list[5] = { &sphere1, &sphere2, &sphere3, &sphere4, &sphere5 };
    HitableList world(list, 5);*/

    Hitable* world = RandomScene();

    Timestamp t;

    Vector lookFrom(13, 2, 3);
    Vector lookAt(0, 0, 0);
    float distToFocus = 10.0f;
    float apperture = 0.1f;
    Camera camera(lookFrom, lookAt, Vector(0, 1, 0), 20.0f, float(nx) / float(ny), apperture, distToFocus, 0.f, 1.f);

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
	for (int i = 0; i < num_processors; i++) {
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
