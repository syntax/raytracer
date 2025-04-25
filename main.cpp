#include "rtweekend.h"
#include "camera.h"
#include "hittable.h"
#include "sphere.h"
#include "kd_node.h"
#include "aabb.h"

#include <memory>
#include <vector>

int main() {

    // first building kd tree obj by obj
    std::vector<std::shared_ptr<hittable>> objects;

    objects.push_back(std::make_shared<sphere>(
        point3(0, -1000, 0), 1000,
        std::make_unique<lambertian>(colour(0.5, 0.5, 0.5))
    ));

    for (int a = -3; a < 3; ++a) {
        for (int b = -3; b < 3; ++b) {
            auto choose_mat = random_double();
            point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());
            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                if (choose_mat < 0.8) {
                    auto albedo = colour::random() * colour::random();
                    objects.push_back(std::make_shared<sphere>(
                        center, 0.2,
                        std::make_unique<lambertian>(albedo)
                    ));
                }
                else if (choose_mat < 0.95) {
                    auto albedo = colour::random(0.5, 1);
                    auto fuzz   = random_double(0, 0.5);
                    objects.push_back(std::make_shared<sphere>(
                        center, 0.2,
                        std::make_unique<metal>(albedo, fuzz)
                    ));
                }
                else {
                    objects.push_back(std::make_shared<sphere>(
                        center, 0.2,
                        std::make_unique<dielectric>(1.5)
                    ));
                }
            }
        }
    }

    objects.push_back(std::make_shared<sphere>(
        point3(0, 1, 0), 1.0,
        std::make_unique<dielectric>(1.5)
    ));
    objects.push_back(std::make_shared<sphere>(
        point3(-4, 1, 0), 1.0,
        std::make_unique<lambertian>(colour(0.4, 0.2, 0.1))
    ));
    objects.push_back(std::make_shared<sphere>(
        point3(4, 1, 0), 1.0,
        std::make_unique<metal>(colour(0.7, 0.6, 0.5), 0.0)
    ));

    kd_node world_tree(objects);

    camera cam;
    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 1200;
    cam.samples_per_pixel = 20;
    cam.max_depth         = 50;
    cam.vfov              = 20;
    cam.lookfrom          = point3(13, 2, 3);
    cam.lookat            = point3(0, 0, 0);
    cam.vup               = vec3(0, 1, 0);
    cam.defocus_angle     = 0.6;
    cam.focus_distance    = 10.0;

    cam.render(world_tree);
    return 0;
}