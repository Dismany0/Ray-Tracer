#ifndef QUAD_H
#define QUAD_H

#include "rtweekend.h"
#include "hittable.h"

class quad : public hittable {
    public:
    quad(const point3& Q, const vec3& u, const vec3& v, sared_ptr<material> mat) :
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

            //otherwise hits the plane
            auto intersection = r.at(t);

            rec.t = t;
            rec.p = intersection;
            rec.mat = mat;
            rec.set_face_normal(r, normal);
            
            return true;

        return false; 
    }

    private:
    point3 Q;
    vec3 u, v;
    vec3 w;
    shared_ptr<material> mat;
    aabb bbox;
    vec3 normal;
    double D;

}