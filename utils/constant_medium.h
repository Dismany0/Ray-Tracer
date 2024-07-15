#ifndef CONSTANT_MEDIUM_H
#define CONSTANT_MEDIUM_H

#include "rtweekend.h"

#include "hittable.h"
#include "material.h"
#include "texture.h"

class constant_medium : public hittable {
    public:
    constant_medium(shared_ptr<hittable> boundary, double density, shared_ptr<texture> tex) :
    boundary(boundary), neg_inv_density(-1/density), phase_function(make_shared<isotropic>(tex)) {}

    constant_medium(shared_ptr<hittable> boundary, double density, const color& albedo) :
    boundary(boundary), neg_inv_density(-1/density), phase_function(make_shared<isotropic>(albedo)) {}

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        const bool enableDebug = false;
        const bool debugging = enableDebug && random_double() < 0.00001;

        hit_record rec1, rec2;

        if(!boundary->hit(r, interval(-infinity, infinity), rec1))
            return false;

        if(!boundary->hit(r, interval(rec1.t + 0.0001, infinity), rec2))
            return false;

        if(debugging) std::cerr << "\nt_min=" << rec1.t << ", t_max=" << rec2.t << '\n';

        if(rec1.t < ray_t.min()) rec1.t = ray_t.min();
        if(rec2.t > ray_t.max()) rec2.t = ray_t.max();
    }


    aabb bounding_box() const override { return boundary->bounding_box(); }
    private:
    shared_ptr<hittable> boundary;
    double neg_inv_density;
    shared_ptr<material> phase_function;

};

#endif