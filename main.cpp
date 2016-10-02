#include <iostream>
#include "vec3.h"

int main()
{
    const int nx = 200;
    const int ny = 100;
    std::cout << "P3\n" << nx << " " << ny << "\n255\n";

    for (int j = ny - 1; j >= 0; j--)
    {
        for (int i = 0; i < nx; i++)
        {
            Vector color(
                static_cast<float>(i) / static_cast<float>(nx),
                static_cast<float>(j) / static_cast<float>(ny),
                0.2f);

            int ir = static_cast<int>(255.99 * color[0]);
            int ig = static_cast<int>(255.99 * color[1]);
            int ib = static_cast<int>(255.99 * color[2]);

            std::cout << ir << " " << ig << " " << ib << "\n";
        }
    }
}
