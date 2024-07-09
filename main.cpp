#include <iostream>
#include "utils\rtweekend.h"

#include "utils\bvh.h"
#include "utils\camera.h"
#include "utils\hittable_list.h"
#include "utils\sphere.h"
#include "utils\color.h"
#include "utils\material.h"
#include "utils\interval.h"
#include "utils\aabb.h"
#include "utils\texture.h"
#include "utils\quad.h"


void world_1()
{
    // Make World
    hittable_list world;

    // Materials
    auto material_ground = make_shared<lambertian>(color(0.2, 0.6, 0.2));
    auto material_center = make_shared<lambertian>(color(0.5, 0.1, 0.2));
    auto material_left = make_shared<dielectric>(1.7);
    auto material_right = make_shared<metal>(color(0.7, 0.2, 0.7), 0.05);

    // Objects
    world.add(make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, material_ground));
    world.add(make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.5, material_center));
    world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.5, material_left));
    world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), -0.4, material_left));
    world.add(make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, material_right));

    // Camera
    world = hittable_list(make_shared<bvh_node>(world));
    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 2000;
    cam.samples_per_pixel = 500;
    cam.max_depth = 25;

    cam.vfov = 70;
    cam.lookfrom = point3(11, 16, 14);
    cam.lookat = point3(0, 12, 0);
    cam.vup = vec3(0, 1, 0);
    cam.background = color(0.70, 0.80, 1.00);

    cam.defocus_angle = 0.0;
    cam.focus_dist = (cam.lookfrom - cam.lookat).length();

    cam.render(world);
}

void world_2()
{
    // Make World
    hittable_list world;

    // Materials
    auto material_ground = make_shared<lambertian>(color(0.1, 0.1, 0.1));
    auto checker_ground = make_shared<checker_texture>(1, color(.1, .1, .1), color(.9, .9, .9));
    auto checker = make_shared<lambertian>(checker_ground);

    world.add(make_shared<sphere>(point3(0.0, -1000.0, -1.0), 1000.0, checker));

    auto matte_white = make_shared<lambertian>(color(0.5, 0.2, 0.2));
    auto mat_glass = make_shared<dielectric>(1.7);
    auto blue_metal = make_shared<metal>(color(0.7, 0.7, 0.8), 0.05);

    // Objects
    for (double y = 0.0; y < 30; ++y)
    {
        for (double rad = 7.0; rad < 10.0; ++rad)
        {
            double mat = random_double();

            shared_ptr<material> material_temp;

            if (mat < 0.6)
            {
                material_temp = make_shared<lambertian>(color(random_double(), random_double(), random_double()));
            }
            else if (mat < 0.85)
            {
                material_temp = make_shared<metal>(color(random_double(), random_double(), random_double()), 0.05);
            }
            else
            {
                material_temp = make_shared<dielectric>(1.7);
            }

            double offshift = random_double();
            double offshift2 = random_double() * 0.2;
            world.add(make_shared<sphere>(point3(sin(y * 6.1 + offshift) * rad, 1.5 * y + 1, cos(y * 6.1 + offshift) * rad), 0.4 + offshift2, material_temp));

            offshift = random_double();
            offshift2 = random_double() * 0.3;
            world.add(make_shared<sphere>(point3(sin(y * 6.1 + offshift + 2 * pi / 3) * rad, 1.5 * y + 1, cos(y * 6.1 + offshift + 2 * pi / 3) * rad), 0.4 + offshift2, material_temp));

            offshift = random_double();
            offshift2 = random_double() * 0.3;
            world.add(make_shared<sphere>(point3(sin(y * 6.1 + offshift + 4 * pi / 3) * rad, 1.5 * y + 1, cos(y * 6.1 + offshift + 4 * pi / 3) * rad), 0.4 + offshift2, material_temp));
        }
    }

    // world.add(make_shared<sphere>(point3(0.0, 5, 0.0), point3(5, 5, 0.0), 5, matte_white));
    // world.add(make_shared<sphere>(point3(0.0, 15, 0.0), point3(5, 5, 0.0), 5, blue_metal));
    // world.add(make_shared<sphere>(point3(0.0, 25, 0.0), point3(5, 5, 0.0), 5, mat_glass));

    world.add(make_shared<sphere>(point3(0.0, 5, 0.0), 5, matte_white));
    world.add(make_shared<sphere>(point3(0.0, 15, 0.0), 5, blue_metal));
    world.add(make_shared<sphere>(point3(0.0, 25, 0.0), 5, mat_glass));

    // Camera
    world = hittable_list(make_shared<bvh_node>(world));
    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 2000;
    cam.samples_per_pixel = 50;
    cam.max_depth = 25;

    cam.vfov = 70;
    cam.lookfrom = point3(11, 16, 14);
    cam.lookat = point3(0, 12, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0.0;
    cam.focus_dist = (cam.lookfrom - cam.lookat).length();
    cam.background = color(0.70, 0.80, 1.00);

    cam.render(world);
}

void world_2_checkered_spheres()
{
    auto earth_texture = make_shared<image_texture>("earthmap.jpg");
    hittable_list world;

    auto checker = make_shared<checker_texture>(0.32, color(.2, .3, .1), color(.9, .9, .9));

    world.add(make_shared<sphere>(point3(0, -10, 0), 10, make_shared<lambertian>(checker)));
    world.add(make_shared<sphere>(point3(0, 10, 0), 10, make_shared<lambertian>(checker)));

    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 1000;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;

    cam.vfov = 20;
    cam.lookfrom = point3(13, 2, 3);
    cam.lookat = point3(0, 0, 0);
    cam.vup = vec3(0, 1, 0);
    cam.background = color(0.70, 0.80, 1.00);

    cam.defocus_angle = 0;

    cam.render(world);
}

void earth()
{
    auto earth_texture = make_shared<image_texture>("earthmap.jpg");
    auto earth_surface = make_shared<lambertian>(earth_texture);
    auto globe = make_shared<sphere>(point3(0, 0, 0), 2, earth_surface);

    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 1200;
    cam.samples_per_pixel = 200;
    cam.max_depth = 50;

    cam.vfov = 20;
    cam.lookfrom = point3(0, 0, 12);
    cam.lookat = point3(0, 0, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0;
    cam.background = color(0.70, 0.80, 1.00);

    cam.render(hittable_list(globe));
}

void perlin_spheres()
{
    hittable_list world;

    auto pertext = make_shared<noise_texture>(4);
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
    world.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 800;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;

    cam.vfov = 20;
    cam.lookfrom = point3(13, 2, 3);
    cam.lookat = point3(0, 0, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0;
    cam.background = color(0.70, 0.80, 1.00);

    cam.render(world);
}

void quads() {
    hittable_list world;

    auto left_red     = make_shared<lambertian>(color(1.0, 0.2, 0.2));
    auto back_green   = make_shared<lambertian>(color(0.2, 1.0, 0.2));
    auto right_blue   = make_shared<lambertian>(color(0.2, 0.2, 1.0));
    auto upper_orange = make_shared<lambertian>(color(1.0, 0.5, 0.0));
    auto lower_teal   = make_shared<lambertian>(color(0.2, 0.8, 0.8));

    // Quads
    world.add(make_shared<quad>(point3(-3,-2, 5), vec3(0, 0,-4), vec3(0, 4, 0), left_red));
    world.add(make_shared<quad>(point3(-2,-2, 0), vec3(4, 0, 0), vec3(0, 4, 0), back_green));
    world.add(make_shared<quad>(point3( 3,-2, 1), vec3(0, 0, 4), vec3(0, 4, 0), right_blue));
    world.add(make_shared<quad>(point3(-2, 3, 1), vec3(4, 0, 0), vec3(0, 0, 4), upper_orange));
    world.add(make_shared<quad>(point3(-2,-3, 5), vec3(4, 0, 0), vec3(0, 0,-4), lower_teal));

    camera cam;

    cam.aspect_ratio      = 1.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 50;
    cam.max_depth         = 50;

    cam.vfov     = 80;
    cam.lookfrom = point3(0,0,9);
    cam.lookat   = point3(0,0,0);
    cam.vup      = vec3(0,1,0);
    cam.background = color(0.70, 0.80, 1.00);

    cam.defocus_angle = 0;

    cam.render(world);
}

void simple_light() {
    hittable_list world;

    auto pertext = make_shared<noise_texture>(4);
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<lambertian>(pertext)));
    world.add(make_shared<sphere>(point3(0,2,0), 2, make_shared<lambertian>(pertext)));

    auto difflight = make_shared<diffuse_light>(color(4,4,4));
    world.add(make_shared<quad>(point3(3,1,-2), vec3(2,0,0), vec3(0,2,0), difflight));

    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;
    cam.background        = color(0,0,0);

    cam.vfov     = 20;
    cam.lookfrom = point3(26,3,6);
    cam.lookat   = point3(0,2,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0;

    cam.render(world);
}

void cornell_box() {
    hittable_list world;

    auto red   = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light = make_shared<diffuse_light>(color(15, 15, 15));

    world.add(make_shared<quad>(point3(555,0,0), vec3(0,555,0), vec3(0,0,555), red)); //left
    world.add(make_shared<quad>(point3(0,0,0), vec3(0,555,0), vec3(0,0,555), green)); //right
    world.add(make_shared<quad>(point3(343, 554, 332), vec3(-130,0,0), vec3(0,0,-105), light)); //light
    world.add(make_shared<quad>(point3(0,0,0), vec3(555,0,0), vec3(0,0,555), white));
    world.add(make_shared<quad>(point3(555,555,555), vec3(-555,0,0), vec3(0,0,-555), white));
    world.add(make_shared<quad>(point3(0,0,555), vec3(555,0,0), vec3(0,555,0), white));

    world.add(box(point3(130, 0, 65), point3(295, 165, 230), white));
    world.add(box(point3(265, 0, 295), point3(430, 330, 460), white));

    camera cam;

    cam.aspect_ratio      = 1.0;
    cam.image_width       = 600;
    cam.samples_per_pixel = 800;
    cam.max_depth         = 200;
    cam.background        = color(0,0,0);

    cam.vfov     = 40;
    cam.lookfrom = point3(278, 278, -800);
    cam.lookat   = point3(278, 278, 0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0;

    cam.render(world);
}


int main()
{
    // world_1();
    // world_2();
    // world_2_checkered_spheres();
    // earth();
    // perlin_spheres();
    // quads();
    // simple_light();
    cornell_box();
    return 0;
}
