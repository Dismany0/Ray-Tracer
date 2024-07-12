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
  rotate_y(shared_ptr<hittable> object, double angle) : object(object) {
    auto radians = degrees_to_radians(angle);
    sin_theta = sin(radians);
    cos_theta = cos(radians);
    bbox = object->bounding_box();

    point3 min(infinity, infinity, infinity);
    point3 max(-infinity, -infinity, -infinity);

    for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 2; k++) {
                    auto x = i*bbox.x.max + (1-i)*bbox.x.min;
                    auto y = j*bbox.y.max + (1-j)*bbox.y.min;
                    auto z = k*bbox.z.max + (1-k)*bbox.z.min;

                    auto newx =  cos_theta*x + sin_theta*z;
                    auto newz = -sin_theta*x + cos_theta*z;

                    vec3 tester(newx, y, newz);

                    for (int c = 0; c < 3; c++) {
                        min[c] = fmin(min[c], tester[c]);
                        max[c] = fmax(max[c], tester[c]);
                    }
                }
            }
        }

        bbox = aabb(min, max);
    }

  bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
    //world to local space
    auto origin = r.origin();
    auto direction = r.direction();

    origin[0] = cos_theta * r.origin()[0] - sin_theta * r.origin()[2];
    origin[2] = sin_theta * r.origin()[0] + cos_theta * r.origin()[2];

    direction[0] = cos_theta * r.direction()[0] - sin_theta * r.direction()[2];
    direction[2] = sin_theta * r.direction()[0] + cos_theta * r.direction()[2];

    ray rotated_r(origin, direction, r.time());

  //find intersections
    if (!object->hit(rotated_r, ray_t, rec))
      return false;

    //intersection point local to world
    auto p = rec.p;
    p[0] = cos_theta * rec.p[0] + sin_theta * rec.p[2];
    p[2] = -sin_theta * rec.p[0] + cos_theta * rec.p[2];

    //normal local to world
    auto normal = rec.normal;
    normal[0] = cos_theta * rec.normal[0] + sin_theta * rec.normal[2];
    normal[2] = -sin_theta * rec.normal[0] + cos_theta * rec.normal[2];

    rec.p = p;
    rec.normal = normal;

    return true;
  }

  aabb bounding_box() const override { return bbox; }

  private:
  shared_ptr<hittable> object;
    double sin_theta;
    double cos_theta;
    aabb bbox;

};

class rotate_x : public hittable {
  public:

  rotate_x(shared_ptr<hittable> object, double angle) : object(object) {
    auto radians = degrees_to_radians(angle);
    sin_theta = sin(radians);
    cos_theta = cos(radians);
    bbox = object->bounding_box();

    point3 min(infinity, infinity, infinity);
    point3 max(-infinity, -infinity, -infinity);

    for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 2; k++) {
                    auto x = i*bbox.x.max + (1-i)*bbox.x.min;
                    auto y = j*bbox.y.max + (1-j)*bbox.y.min;
                    auto z = k*bbox.z.max + (1-k)*bbox.z.min;

                    auto newy =  cos_theta*y + sin_theta*z;
                    auto newz = -sin_theta*y + cos_theta*z;

                    vec3 tester(x, newy, newz);

                    for (int c = 0; c < 3; c++) {
                        min[c] = fmin(min[c], tester[c]);
                        max[c] = fmax(max[c], tester[c]);
                    }
                }
            }
        }

        bbox = aabb(min, max);
    }

  bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
    //world to local space
    auto origin = r.origin();
    auto direction = r.direction();

    origin[1] = cos_theta * r.origin()[1] - sin_theta * r.origin()[2];
    origin[2] = sin_theta * r.origin()[1] + cos_theta * r.origin()[2];

    direction[1] = cos_theta * r.direction()[1] - sin_theta * r.direction()[2];
    direction[2] = sin_theta * r.direction()[1] + cos_theta * r.direction()[2];

    ray rotated_r(origin, direction, r.time());

  //find intersections

    if (!object->hit(rotated_r, ray_t, rec))
      return false;

    //intersection point local to world
    auto p = rec.p;
    p[1] = cos_theta * rec.p[1] + sin_theta * rec.p[2];
    p[2] = -sin_theta * rec.p[1] + cos_theta * rec.p[2];

    //normal local to world
    auto normal = rec.normal;
    normal[1] = cos_theta * rec.normal[1] + sin_theta * rec.normal[2];
    normal[2] = -sin_theta * rec.normal[1] + cos_theta * rec.normal[2];

    rec.p = p;
    rec.normal = normal;

    return true;
  }

  aabb bounding_box() const override { return bbox; }
  private:
  shared_ptr<hittable> object;
    double sin_theta;
    double cos_theta;
    aabb bbox;

};

class rotate_z : public hittable {
  public:

  rotate_z(shared_ptr<hittable> object, double angle) : object(object) {
    auto radians = degrees_to_radians(angle);
    sin_theta = sin(radians);
    cos_theta = cos(radians);
    bbox = object->bounding_box();

    point3 min(infinity, infinity, infinity);
    point3 max(-infinity, -infinity, -infinity);

    for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 2; k++) {
                    auto x = i*bbox.x.max + (1-i)*bbox.x.min;
                    auto y = j*bbox.y.max + (1-j)*bbox.y.min;
                    auto z = k*bbox.z.max + (1-k)*bbox.z.min;

                    auto newx =  cos_theta*x + sin_theta*y;
                    auto newy = -sin_theta*x + cos_theta*y;

                    vec3 tester(newx, newy, z);

                    for (int c = 0; c < 3; c++) {
                        min[c] = fmin(min[c], tester[c]);
                        max[c] = fmax(max[c], tester[c]);
                    }
                }
            }
        }

        bbox = aabb(min, max);
    }

  bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
    //world to local space
    auto origin = r.origin();
    auto direction = r.direction();

    origin[0] = cos_theta * r.origin()[0] - sin_theta * r.origin()[1];
    origin[1] = sin_theta * r.origin()[0] + cos_theta * r.origin()[1];

    direction[0] = cos_theta * r.direction()[0] - sin_theta * r.direction()[1];
    direction[1] = sin_theta * r.direction()[0] + cos_theta * r.direction()[1];

    ray rotated_r(origin, direction, r.time());

  //find intersections
  
      if (!object->hit(rotated_r, ray_t, rec))
        return false;
  
      //intersection point local to world
      auto p = rec.p;
      p[0] = cos_theta * rec.p[0] + sin_theta * rec.p[1];
      p[1] = -sin_theta * rec.p[0] + cos_theta * rec.p[1];
  
      //normal local to world
      auto normal = rec.normal;
      normal[0] = cos_theta * rec.normal[0] + sin_theta * rec.normal[1];
      normal[1] = -sin_theta * rec.normal[0] + cos_theta * rec.normal[1];
  
      rec.p = p;
      rec.normal = normal;
  
      return true;
    }

    aabb bounding_box() const override { return bbox; }

  private:
  shared_ptr<hittable> object;
    double sin_theta;
    double cos_theta;
    aabb bbox;

};

#endif