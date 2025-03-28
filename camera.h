#ifndef CAMERA_H
#define CAMERA_H

#include "hittable.h"
#include "material.h"


#include <thread>
#include <mutex>

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

    double vfov = 90; // vertical field of view in degrees
    point3 lookfrom = point3(0,0,0);   // point that camera is looking from
    point3 lookat   = point3(0,0,-1);  // point that camera is looking at
    vec3   vup      = vec3(0,1,0);     // cam-relative "up" direction

    double defocus_angle = 0;   // variation angle of rays thru each pixek
    double focus_distance = 10; //distance from camera lookfrom pt to perfect focus

    void render(const hittable& world) {
        initialize();

        std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
        

        // find number of threads/cores 
        int thread_count = std::thread::hardware_concurrency();
        if (thread_count == 0) thread_count = 4;

        std::vector<std::thread> threads;
        std::mutex mtx;

        int rows_per_thread = image_height / thread_count;

        //rending lamba func given a start and end row
        auto render_rows = [&](int start_row, int end_row) {
            for (int j = 0; start_row < end_row; j++) {
                // std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
                std::ostringstream oss;
                for (int i = 0; i < image_width; i++) {
                colour pixel_colour(0,0,0);
                for (int s = 0; s < samples_per_pixel; s++) {
                        ray r = get_ray(i, j);
                        pixel_colour += ray_colour(r, max_depth, world);
                }
                write_colour(std::cout, pixel_samples_scale * pixel_colour);
                }
                {
                    std::lock_guard<std::mutex> lock(mtx);
                    std::clog << "\rScanlines remaining: " << (image_height - start_row) << ' ' << std::flush;
                    std::cout << oss.str();
                }
            }
        };
        
        int curr_row = 0;
        for (int i = 0; i < thread_count; i++) {
            int start_row = curr_row;
            int end_row = curr_row + rows_per_thread;
            if (i == thread_count - 1) {
                end_row = image_height;
            }
            threads.push_back(std::thread(render_rows, start_row, end_row));
            curr_row = end_row;
        }

        // join threads togethr
        for (auto& t : threads) {
            t.join();
        }

        std::clog << "\nDone.\n";

    }

  private:
    int    image_height;   // rendered image height
    double pixel_samples_scale; // colour scale factor for a sum of pizel samples
    point3 center;         // cam center
    point3 pixel00_loc;    // loc of pixel 0, 0
    vec3   pixel_delta_u;  // offset to pixel to the right
    vec3   pixel_delta_v;  // offset to pixel below
    vec3   u, v, w;        // camera basis vectors
    vec3   defocus_disk_u, defocus_disk_v; // defocus disk basis vectors (horiz and vert)

    void initialize() {
        image_height = int(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        pixel_samples_scale = 1.0 / samples_per_pixel;

        center = lookfrom;

        // determine viewport dimensions.
        auto theta = degrees_to_radians(vfov);
        auto h = std::tan(theta/2);
        auto viewport_height = 2.0 * h * focus_distance;
        auto viewport_width = viewport_height * (double(image_width)/image_height);

        // calculateing the unit basis vectors for the camera coordinate system
        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        // calc the vectors across the horizontal and down the vertical viewport edges.
        auto viewport_u = viewport_width * u; // vector across viewport horiz edge
        auto viewport_v = viewport_height * -v; // vector down viewport vert edge

        // calc the horizontal and vertical delta vectors from pixel to pixel.
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // calc the location of the upper left pixel.
        auto viewport_upper_left =
            center - (focus_distance * w) - (0.5 * viewport_u) - (0.5 * viewport_v);
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
    
        auto defocus_radius = focus_distance * std::tan(degrees_to_radians(defocus_angle) / 2);
        defocus_disk_u = defocus_radius * u;
        defocus_disk_v = defocus_radius * v;
    }

    ray get_ray(int i, int j) const {
        // construct a camera ray originating from the defocus disk and directed at a randomly
        // sampled point around the pixel location i,j
        auto offset = sample_square();
        auto pixel_sample = pixel00_loc
                    + ((i + offset.x()) * pixel_delta_u)
                    + ((j + offset.y()) * pixel_delta_v);

        auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
        auto ray_direction = pixel_sample - ray_origin;
        return ray(ray_origin, ray_direction);
    }

    vec3 sample_square() const {
        return vec3(random_double() - 0.5, random_double() - 0.5, 0);
    }
    
    point3 defocus_disk_sample() const {
        auto p = random_in_unit_disk();
        return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
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
            ray scattered;
            colour attenuation;
            if (rec.material_ptr->scatter(r, rec, attenuation, scattered))
                return attenuation * ray_colour(scattered, depth-1, world);
            return colour(0,0,0);
        }

        vec3 unit_direction = unit_vector(r.direction());
        auto a = 0.5*(unit_direction.y() + 1.0);
        return (1.0-a)*colour(1.0, 1.0, 1.0) + a*colour(0.5, 0.7, 1.0);
    }
};

#endif