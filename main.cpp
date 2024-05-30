#include <iostream>
#include "utils\rtweekend.h"

#include "utils\bvh.h"
#include "utils\camera.h"
#include "utils\hittable_list.h"
#include "utils\sphere.h"
#include "utils\color.h"
#include "utils\material.h"

void world_1(hittable_list &world){
    auto material_ground = make_shared<lambertian>(color(0.2, 0.6, 0.2));
    auto material_center = make_shared<lambertian>(color(0.5, 0.1, 0.2));
    auto material_left   = make_shared<dielectric>(1.7);
    auto material_right = make_shared<metal>(color(0.7, 0.2, 0.7), 0.05);
    
    world.add(make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, material_ground));
    world.add(make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.5, material_center));
    world.add(make_shared<sphere>(point3(-1.0,    0.0, -1.0),   0.5, material_left));
    world.add(make_shared<sphere>(point3(-1.0,    0.0, -1.0),  -0.4, material_left));
    world.add(make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, material_right));
}

void world_2(hittable_list &world){
    auto material_ground = make_shared<lambertian>(color(0.1, 0.1, 0.1));
    world.add(make_shared<sphere>(point3(0.0, -1000.0, -1.0), 1000.0, material_ground));

    auto matte_white = make_shared<lambertian>(color(0.5, 0.2, 0.2));
    auto mat_glass   = make_shared<dielectric>(1.7);
    auto blue_metal = make_shared<metal>(color(0.7, 0.7, 0.8), 0.05);



    for(double y = 0.0; y <30; ++y){
        for (double rad = 7.0; rad < 10.0; ++rad){
            double mat = random_double();
            
            shared_ptr<material> material_temp;
            
            if (mat < 0.6){
                material_temp = make_shared<lambertian>(color(random_double(), random_double(), random_double()));
            }
            else if (mat < 0.85){
                material_temp = make_shared<metal>(color(random_double(), random_double(), random_double()), 0.05);
            }
            else{
                material_temp = make_shared<dielectric>(1.7);
            }
            
            double offshift = random_double();
            double offshift2 = random_double()*0.2;
            world.add(make_shared<sphere>(point3(sin(y * 6.1 + offshift)*rad, 1.5*y+1, cos(y * 6.1 + offshift)*rad), 0.4 + offshift2, material_temp));

            offshift = random_double();
            offshift2 = random_double()*0.3;
            world.add(make_shared<sphere>(point3(sin(y * 6.1 + offshift + 2 * pi / 3)*rad, 1.5*y+1, cos(y * 6.1 + offshift + 2 * pi / 3)*rad), 0.4 + offshift2, material_temp));

            offshift = random_double();
            offshift2 = random_double()*0.3;
            world.add(make_shared<sphere>(point3(sin(y * 6.1 + offshift + 4 * pi / 3)*rad, 1.5*y+1, cos(y * 6.1 + offshift + 4 * pi / 3)*rad), 0.4 + offshift2, material_temp));

            
        }
    }

    world.add(make_shared<sphere>(point3(0.0, 5, 0.0), point3(5, 5, 0.0), 5, matte_white));
    world.add(make_shared<sphere>(point3(0.0, 15, 0.0), point3(5, 5, 0.0), 5, blue_metal));
    world.add(make_shared<sphere>(point3(0.0, 25, 0.0), point3(5, 5, 0.0), 5, mat_glass));
}


int main() {
    // World
    hittable_list world;

    // I am loading the world through a function here
    world_2(world);
    world = hittable_list(make_shared<bvh_node>(world));
    

    // Camera
    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 800;
    cam.samples_per_pixel = 25;
    cam.max_depth = 25;

    cam.vfov     = 70;
    cam.lookfrom = point3(11,16,14);
    cam.lookat   = point3(0,12,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0.0;
    cam.focus_dist    = (cam.lookfrom - cam.lookat).length();


    cam.render(world);

}