#ifndef AABB_H
#define AABB_H

#include "rtweekend.h"

class aabb 
{
public:
interval x, y, z;
aabb() {} //By default, intervals are empty
aabb(const interval& _x, const interval& _y, const interval& _z) : x(_x), y(_y), z(_z) {}

aabb(const point3& a, const point3& b) {
    //a and b are two opposite corners of the box
    x = (a[0] <= b[0]) ? interval(a[0], b[0]) : interval(b[0], a[0]);
    y = (a[1] <= b[1]) ? interval(a[1], b[1]) : interval(b[1], a[1]);
    z = (a[2] <= b[2]) ? interval(a[2], b[2]) : interval(b[2], a[2]);
}

const interval& axis_interval(int n) const {
    if (n == 1) return y;
    if (n == 2) return z;
    return x;
}

bool hit(const ray& r, interval ray_t) const{

}



};
#endif