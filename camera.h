#ifndef CAMERA_H
#define CAMERA_H

#include "hittable.h"


/*
This class represents a camera in the scene. 
It constructs and dispatches rays into the world.
Uses the results of the ray  to construct the rendered image.
*/
class camera {
  public:
    double aspect_ratio = 1.0;  // ratio of image width over height
    int    image_width  = 100;  // rendered image width in pixel count
    int    samples_per_pixel = 10;  // number of samples per pixel
    int    max_depth = 10; // max recursion depth

    void render(const hittable& world) {
        initialize();

        std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

        for (int j = 0; j < image_height; j++) {
            std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
            for (int i = 0; i < image_width; i++) {
               colour pixel_colour(0,0,0);
               for (int s = 0; s < samples_per_pixel; s++) {
                    ray r = get_ray(i, j);
                    pixel_colour += ray_colour(r, max_depth, world);
               }
               write_colour(std::cout, pixel_samples_scale * pixel_colour);
            }
        }

        std::clog << "\rDone.                 \n";
    }

  private:
    int    image_height;   // rendered image height
    double pixel_samples_scale; // colour scale factor for a sum of pizel samples
    point3 center;         // cam center
    point3 pixel00_loc;    // loc of pixel 0, 0
    vec3   pixel_delta_u;  // offset to pixel to the right
    vec3   pixel_delta_v;  // offset to pixel below

    void initialize() {
        image_height = int(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        pixel_samples_scale = 1.0 / samples_per_pixel;

        center = point3(0, 0, 0);

        // determine viewport dimensions.
        auto focal_length = 1.0;
        auto viewport_height = 2.0;
        auto viewport_width = viewport_height * (double(image_width)/image_height);

        // calc the vectors across the horizontal and down the vertical viewport edges.
        auto viewport_u = vec3(viewport_width, 0, 0);
        auto viewport_v = vec3(0, -viewport_height, 0);

        // calc the horizontal and vertical delta vectors from pixel to pixel.
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // calc the location of the upper left pixel.
        auto viewport_upper_left =
            center - vec3(0, 0, focal_length) - viewport_u/2 - viewport_v/2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
    }

    ray get_ray(int i, int j) const {
        auto offset = sample_square();
        auto pixel_sample = pixel00_loc
                    + ((i + offset.x()) * pixel_delta_u)
                    + ((j + offset.y()) * pixel_delta_v);

        return ray(center, unit_vector(pixel_sample - center));
    }

    vec3 sample_square() const {
        return vec3(random_double() - 0.5, random_double() - 0.5, 0);
    }

    colour ray_colour(const ray& r, int depth, const hittable& world) const {
        if (depth <= 0) {
            // max recursion depth exceeded
            return colour(0, 0, 0);
        }

        hit_record rec;
        
        // ignoring hits that are very close to zero i.e. removes shadow acne
        // https://digitalrune.github.io/DigitalRune-Documentation/html/3f4d959e-9c98-4a97-8d85-7a73c26145d7.htm
        if (world.hit(r, interval(0.001, infinity), rec)) {
            vec3 direction = rec.normal + random_unit_vector();
            return 0.9 * ray_colour(ray(rec.p, direction), depth -1, world);
        }

        vec3 unit_direction = unit_vector(r.direction());
        auto a = 0.5*(unit_direction.y() + 1.0);
        return (1.0-a)*colour(1.0, 1.0, 1.0) + a*colour(0.5, 0.7, 1.0);
    }
};

#endif