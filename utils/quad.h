#ifndef QUAD_H
#define QUAD_H

#include "rtweekend.h"
#include "hittable.h"
#include "hittable_list.h"

class quad : public hittable {
    public:
    quad(const point3& Q, const vec3& u, const vec3& v, shared_ptr<material> mat) :
    Q(Q), u(u), v(v), mat(mat)
    {

        //Plnar coords
        auto n = cross(u, v);
        normal = unit_vector(n);
        D = dot(normal, Q);
        w = n / dot(n,n);

        set_bounding_box();
    }

    virtual void set_bounding_box() {
        auto bbox_diagonal1 = aabb(Q, Q+u + v);
        auto bbox_diagonal2 = aabb(Q+u, Q+v);
        bbox = aabb(bbox_diagonal1, bbox_diagonal2);
    }

    aabb bounding_box() const override { return bbox; }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        auto denom = dot(normal, r.direction());

        //parallel to plane = nohit
        if (fabs(denom) < 1e-8)
            return false;

            //return false if t is outside interval for ray
            auto t = (D - dot(normal, r.origin())) / denom;
            if (!ray_t.contains(t))
                return false;

            //otherwise hits the plane, check for shape intersection
            auto intersection = r.at(t);
            vec3 planar_hitpt_vector = intersection - Q;
            auto alpha = dot(w, cross(planar_hitpt_vector, v));
            auto beta = dot(w, cross(u, planar_hitpt_vector));

            if (!is_interior(alpha, beta, rec))
                return false;

            // Hit shape, get rec
            rec.t = t;
            rec.p = intersection;
            rec.mat = mat;
            rec.set_face_normal(r, normal);
            
            return true;

        return false; 
    }

    virtual bool is_interior(double a, double b, hit_record& rec) const {
        interval unit_interval = interval(0, 1);
        // return if hit lands, false otherwise

        if (!unit_interval.contains(a) || !unit_interval.contains(b))
            return false;

        rec.u = a;
        rec.v = b;
        return true;
    }
    

    protected:
    point3 Q; //origin
    vec3 u, v; //uv directions
    vec3 w;
    shared_ptr<material> mat;
    aabb bbox;
    vec3 normal;
    double D;

};
inline shared_ptr<hittable_list> box(const point3& a, const point3& b, shared_ptr<material> mat) {
    //Box defined by 2 opposite vertices a and b

    auto sides = make_shared<hittable_list>();

    auto min = point3(fmin(a.x(), b.x()), fmin(a.y(),b.y()), fmin(a.z(), b.z()));
    auto max = point3(fmax(a.x(), b.x()), fmax(a.y(),b.y()), fmax(a.z(), b.z()));

    auto dx = vec3(max.x() - min.x(), 0, 0);
    auto dy = vec3(0, max.y() - min.y(), 0);
    auto dz = vec3(0, 0, max.z() - min.z());

    sides->add(make_shared<quad>(point3(min.x(), min.y(), max.z()),  dx,  dy, mat)); // front
    sides->add(make_shared<quad>(point3(max.x(), min.y(), max.z()), -dz,  dy, mat)); // right
    sides->add(make_shared<quad>(point3(max.x(), min.y(), min.z()), -dx,  dy, mat)); // back
    sides->add(make_shared<quad>(point3(min.x(), min.y(), min.z()),  dz,  dy, mat)); // left
    sides->add(make_shared<quad>(point3(min.x(), max.y(), max.z()),  dx, -dz, mat)); // top
    sides->add(make_shared<quad>(point3(min.x(), min.y(), min.z()),  dx,  dz, mat)); // bottom

    return sides;
}

class tri : public quad {
  public:
    tri(const point3& o, const vec3& aa, const vec3& ab, shared_ptr<material> m)
      : quad(o, aa, ab, m)
    {}

    virtual bool is_interior(double a, double b, hit_record& rec) const override {
        if ((a < 0) || (b < 0) || (a + b > 1))
            return false;

        rec.u = a;
        rec.v = b;
        return true;
    }
};

class ellipse : public quad {
  public:
    ellipse(
        const point3& center, const vec3& side_A, const vec3& side_B, shared_ptr<material> m
    ) : quad(center, side_A, side_B, m)
    {}

    virtual void set_bounding_box() override {
        bbox = aabb(Q - u - v, Q + u + v);
    }

    virtual bool is_interior(double a, double b, hit_record& rec) const override {
        if ((a*a + b*b) > 1)
            return false;

        rec.u = a/2 + 0.5;
        rec.v = b/2 + 0.5;
        return true;
    }
};

class annulus : public quad {
  public:
    annulus(
        const point3& center, const vec3& side_A, const vec3& side_B, double _inner,
        shared_ptr<material> m)
      : quad(center, side_A, side_B, m), inner(_inner)
    {}

    virtual void set_bounding_box() override {
        bbox = aabb(Q - u - v, Q + u + v);
    }

    virtual bool is_interior(double a, double b, hit_record& rec) const override {
        auto center_dist = sqrt(a*a + b*b);
        if ((center_dist < inner) || (center_dist > 1))
            return false;

        rec.u = a/2 + 0.5;
        rec.v = b/2 + 0.5;
        return true;
    }

  private:
    double inner;
};

#endif