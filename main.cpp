#include "rtweekend.h"

#include "camera.h"
#include "hittable.h"
#include "material.h"
#include "hittable_list.h"
#include "sphere.h"

#include <memory>

int main() {
    hittable_list world;

    // ground
    world.add(std::make_unique<sphere>(
        point3(0, -1000, 0), 1000,
        std::unique_ptr<material>( new lambertian(colour(0.5, 0.5, 0.5)) )
    ));

    // random small spheres
    for (int a = -3; a < 3; a++) {
        for (int b = -3; b < 3; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = colour::random() * colour::random();
                    world.add(std::make_unique<sphere>(
                        center, 0.2,
                        std::unique_ptr<material>( new lambertian(albedo) )
                    ));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = colour::random(0.5, 1);
                    auto fuzz   = random_double(0, 0.5);
                    world.add(std::make_unique<sphere>(
                        center, 0.2,
                        std::unique_ptr<material>( new metal(albedo, fuzz) )
                    ));
                } else {
                    // glass
                    world.add(std::make_unique<sphere>(
                        center, 0.2,
                        std::unique_ptr<material>( new dielectric(1.5) )
                    ));
                }
            }
        }
    }

    world.add(std::make_unique<sphere>(
        point3(0, 1, 0), 1.0,
        std::unique_ptr<material>( new dielectric(1.5) )
    ));

    world.add(std::make_unique<sphere>(
        point3(-4, 1, 0), 1.0,
        std::unique_ptr<material>( new lambertian(colour(0.4, 0.2, 0.1)) )
    ));

    world.add(std::make_unique<sphere>(
        point3(4, 1, 0), 1.0,
        std::unique_ptr<material>( new metal(colour(0.7, 0.6, 0.5), 0.0) )
    ));

    camera cam;
    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 1200;
    cam.samples_per_pixel = 20; // when this is set at 500, it will take longer.
    cam.max_depth         = 50;

    cam.vfov          = 20;
    cam.lookfrom      = point3(13, 2, 3);
    cam.lookat        = point3(0, 0, 0);
    cam.vup           = vec3(0, 1, 0);
    cam.defocus_angle = 0.6;
    cam.focus_distance = 10.0;

    cam.render(world);
}