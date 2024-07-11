#ifndef HITTABLE_H
#define HITTABLE_H

#include "rtweekend.h"

#include "aabb.h"

class material;

class hit_record { //This is a record of a hit
  public:
    point3 p;
    vec3 normal;
    double t;
    double u;
    double v;
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

    virtual aabb bounding_box() const = 0;
};

class translate: public hittable {
  public:
  translate(shared_ptr<hittable>object, const vec3& offset) : object(object), offset(offset) {
    bbox = object->bounding_box() + offset;
  }

  bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
    //move ray backwards by offset
    ray offset_r(r.origin() - offset, r.direction(), r.time());

    //find intersections
    if (!object->hit(offset_r, ray_t, rec))
      return false;

    //move forward by offset if intersection
    rec.p += offset;
    return true;
  }

  aabb bounding_box() const override { return bbox; }

  private:
  shared_ptr<hittable> object;
  vec3 offset;
  aabb bbox;
};

class rotate_y : public hittable {
  public:

  private:

};

class rotate_x : public rotate_y {
  public:

  private:

};

class rotate_z : public rotate_y {
  public:

  private:

};

#endif