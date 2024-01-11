#ifndef HITTABLE_H
#define HITTABLE_H

#include "rtweekend.h"

class material;

class hit_record { //This is a record of a hit
  public:
    point3 p;
    vec3 normal;
    double t;
    shared_ptr<material> mat;

    bool front_face; //This is used to determine if the ray comes from the outside or the inside of the object

    void set_face_normal(const ray& r, const vec3& outward_normal) {
      //Outward normal is assumed to be a unit vector

        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal; //If the ray comes from outside, frontface is true, else false
    }
};

class hittable { //This is an abstract class for all hittable objects
  public:
    virtual ~hittable() = default;

    virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;
};

#endif