#include "rtweekend.h"
#ifndef INTERVAL_H
#define INTERVAL_H

class interval { //just define an interval
  public:
    double min, max;

    interval() : min(+infinity), max(-infinity) {} // Default interval is empty

    interval(double _min, double _max) : min(_min), max(_max) {}

    bool contains(double x) const {
        return min <= x && x <= max;
    }

    bool surrounds(double x) const {
        return min < x && x < max;
    }

    double clamp(double x) const {
        if (x < min) return min;
        if (x > max) return max;
        return x;
    }

    static const interval empty, universe;
};
// Define the empty and universe intervals
const static interval empty   (+infinity, -infinity);
const static interval universe(-infinity, +infinity);

#endif