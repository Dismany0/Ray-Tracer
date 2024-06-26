#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"

class sphere : public hittable
{
public:
  // Define a sphere with a center, a radius and a material
  // Not moving
  sphere(point3 _center, double _radius, shared_ptr<material> _material)
      : center1(_center), radius(fmax(0, _radius)), mat(_material), is_moving(false)
  { //A sphere bounding box is easy, just draw a cube with sidelength = radius
    auto rvec = vec3(radius, radius, radius);
    bbox = aabb(center1 - rvec, center1 + rvec);
  }

  sphere(point3 _center1, point3 _center2, double _radius, shared_ptr<material> _material)
      : center1(_center1), radius(fmax(0, _radius)), mat(_material), is_moving(true)
  {
    //Moving sphere bounding box just take the start and end points, whole box
    auto rvec = vec3(radius, radius, radius);
    aabb box1(_center1 - rvec, _center1 + rvec);
    aabb box2(_center2 - rvec, _center2 + rvec);
    bbox = aabb(box1, box2);

    center_vec = _center2 - _center1;
  }

  // Define a hit function for a sphere
  bool hit(const ray &r, interval ray_t, hit_record &rec) const override
  {
    // we can solve the equation for a ray and a sphere to find the intersection points
    point3 center = is_moving ? sphere_center(r.time()) : center1;
    vec3 oc = r.origin() - center;
    auto a = r.direction().length_squared();
    auto half_b = dot(oc, r.direction());
    auto c = oc.length_squared() - radius * radius;

    auto discriminant = half_b * half_b - a * c;
    if (discriminant < 0)
      return false;
    auto sqrtd = sqrt(discriminant);

    // Find the nearest root that lies in the acceptable range.
    auto root = (-half_b - sqrtd) / a;

    if (!ray_t.surrounds(root))
    {                               // root is outside of the acceptable range
      root = (-half_b + sqrtd) / a; // check other root
      if (!ray_t.surrounds(root))
        return false;
    }

    rec.t = root;
    rec.p = r.at(rec.t);
    vec3 outward_normal = (rec.p - center) / radius; // we can skip square root because we know the radius
    rec.set_face_normal(r, outward_normal);
    get_sphere_uv(outward_normal, rec.u, rec.v);
    rec.mat = mat;

    return true;
  }

  aabb bounding_box() const override {return bbox;}

private:
  point3 center1;
  double radius;
  shared_ptr<material> mat;
  bool is_moving;
  vec3 center_vec;  
  aabb bbox;

  point3 sphere_center(double time) const
  {
    // Linearly interpolate from center1 to center2 according to time, where t=0 yields
    // center1, and t=1 yields center2.
    return center1 + time * center_vec;
  }

  static void get_sphere_uv(const point3 &p, double &u, double &v)
  {
    // p is a point on the unit sphere centered at the origin.
    // return a u and v value using the spherical coordinates of p
    // u and v lie between 0 and 1
    auto theta = acos(-p.y());
    auto phi = atan2(-p.z(), p.x()) + pi;

    u = phi / (2 * pi);
    v = theta / pi;
  }
};

#endif