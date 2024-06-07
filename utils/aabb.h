#ifndef AABB_H
#define AABB_H

#include "rtweekend.h"

class aabb
{
public:
    interval x, y, z;
    aabb() {} // By default, intervals are empty
    aabb(const interval &_x, const interval &_y, const interval &_z) : x(_x), y(_y), z(_z) {}

    aabb(const point3 &a, const point3 &b)
    {
        // a and b are two opposite corners of the box
        x = (a[0] <= b[0]) ? interval(a[0], b[0]) : interval(b[0], a[0]);
        y = (a[1] <= b[1]) ? interval(a[1], b[1]) : interval(b[1], a[1]);
        z = (a[2] <= b[2]) ? interval(a[2], b[2]) : interval(b[2], a[2]);
    }

    aabb(const aabb &box1, const aabb &box2)
    {
        x = interval(box1.x, box2.x);
        y = interval(box1.y, box2.y);
        z = interval(box1.z, box2.z);
    }

    const interval &axis_interval(int n) const
    {
        if (n == 1)
            return y;
        if (n == 2)
            return z;
        return x;
    }

    bool hit(const ray &r, interval ray_t) const
    {
        const point3 &ray_orig = r.origin();
        const vec3 &ray_dir = r.direction();

        for (int axis = 0; axis < 3; axis++)
        {
            const interval &ax = axis_interval(axis);
            const double adinv = 1.0 / ray_dir[axis];

            auto t0 = (ax.min - ray_orig[axis]) * adinv; // Intervals along the axis
            auto t1 = (ax.max - ray_orig[axis]) * adinv;

            if (t0 < t1)
            {
                if (t0 > ray_t.min)
                    ray_t.min = t0;
                if (t1 < ray_t.max)
                    ray_t.max = t1;
            }
            else
            {
                if (t1 > ray_t.min)
                    ray_t.min = t1;
                if (t0 < ray_t.max)
                    ray_t.max = t0;
            }
            if (ray_t.max <= ray_t.min) // In the case the intervals do not overlap
                return false;
        }
        return true; // If the intervals overlap on all 3 axis, hit
    }

    int longest_axis() const
    {

        if (x.size() > y.size())
            return x.size() > z.size() ? 0 : 2; // return 0 if x > z, otherwise 2
        else
            return y.size() > z.size() ? 1 : 2; // return 1 if y > z, otherwise 2
    }
    static const aabb empty, universe;
};
const aabb aabb::empty = aabb(interval::empty, interval::empty, interval::empty);
const aabb aabb::universe = aabb(interval::universe, interval::universe, interval::universe);
#endif
