#ifndef CAMERA_H
#define CAMERA_H

#include "rtweekend.h"

#include "color.h"
#include "hittable.h"
#include "material.h"

#include <iostream>

class camera
{
public:

    //Public camera parameters
    double aspect_ratio = 1.0; // Aspect ratio width/height
    int image_width = 100;     // Width in pixels
    int samples_per_pixel = 10; // Number of samples per pixel
    int max_depth = 10; // Maximum number of bounces for each ray
    color background = color(0, 0, 0); // Background color

    double vfov = 90.0; // Vertical field of view in degrees
    point3 lookfrom = point3(0, 0, 0);
    point3 lookat = point3(0, 0, -1);
    vec3 vup = vec3(0, 1, 0);

    double defocus_angle = 0.0;
    double focus_dist = 10;

    void render(const hittable &world)
    {
        initialize();
        
        freopen("output.ppm", "w", stdout); // Redirects the output to a file
        std::cout << "P3\n"
                  << image_width << ' ' << image_height << "\n255\n"; //I have no idea what P3 means for ppm but it works

        for (int j = 0; j < image_height; ++j)
        {
            std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush; //progress
            for (int i = 0; i < image_width; ++i)
            {

                color pixel_color(0, 0, 0); //Get the color of the pixel
                for (int sample = 0; sample < samples_per_pixel; ++sample){
                    ray r = get_ray(i,j);
                    pixel_color += ray_color(r, max_depth, world);
                }
                write_color(std::cout, pixel_color, samples_per_pixel);
            }
        }

        std::clog << "\rDone.                 \n";
    }

private:
    int image_height;
    point3 center;
    point3 pixel00_loc;
    vec3 pixel_delta_u;
    vec3 pixel_delta_v;
    vec3 u,v,w;

    vec3 defocus_disk_u;
    vec3 defocus_disk_v;

    void initialize()
    {
        // Calculate the camera basis vectors.
        image_height = static_cast<int>(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        center = lookfrom;

        // auto focal_length = (lookfrom - lookat).length();
        auto theta = degrees_to_radians(vfov);
        auto h = tan(theta / 2);
        auto viewport_height = 2.0 * h * focus_dist;
        auto viewport_width = viewport_height * (static_cast<double>(image_width)/image_height);

        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);


        // Calculate the pixel location vectors.
        auto viewport_u = u * viewport_width;
        auto viewport_v = -v * viewport_height;

        pixel_delta_u = viewport_u / image_width;  // pixel width
        pixel_delta_v = viewport_v / image_height; // pixel height

        auto viewport_upper_left = center - viewport_u / 2 - viewport_v / 2 - w * focus_dist;
        pixel00_loc = viewport_upper_left + pixel_delta_u / 2 + pixel_delta_v / 2; // Center of the top left pixel

        // Calculate the camera defocus disk basis vectors.
        auto defocus_radius = focus_dist * tan(degrees_to_radians(defocus_angle / 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
    }
    point3 defocus_disk_sample() const {
        // Returns a random point in the camera defocus disk
        auto p = random_in_unit_disk();
        return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }
    color ray_color(const ray &r, int depth, const hittable &world) const
    {
        hit_record rec;
        // if exceed bounce limit, no more light is gathered
        if (depth <= 0)
            return color(0,0,0);

        if (!world.hit(r, interval(0.001, infinity), rec))
            return background;
            
        // if hit something, scatter the ray based on material, does not scatter if the bounce is too close
        ray scattered;
        color attenuation;
        color color_from_emission = rec.mat->emitted(rec.u, rec.v, rec.p);
        if (!rec.mat->scatter(r, rec, attenuation, scattered))
            return color_from_emission;

        color color_from_scatter = attenuation * ray_color(scattered, depth - 1, world);
        return color_from_emission + color_from_scatter;
    }

    ray get_ray(int i, int j) const { // i is the horizontal pixel index, j is the vertical pixel index
        auto pixel_center = pixel00_loc + i * pixel_delta_u + j * pixel_delta_v;
        auto pixel_sample = pixel_center + pixel_sample_square();

        // ray origin is center unless we have a defocus angle
        auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
        auto ray_direction = unit_vector(pixel_sample - ray_origin);

        // ray time is random between 0 and 1,
        auto ray_time = random_double();

        return ray(ray_origin, ray_direction, ray_time);
    }

    vec3 pixel_sample_square() const {
        // Returns a random point in the square surrounding a pixel at the origin.
        auto px = -0.5 + random_double();
        auto py = -0.5 + random_double();
        return (px * pixel_delta_u) + (py * pixel_delta_v);
    }
};

#endif