#ifndef MATH_MATERIALS_H
#define MATH_MATERIALS_H
#include <cmath>
#include <limits>
#include <memory>
#include <cstdlib>

#define infinity std::numeric_limits<double>::infinity()
#define Pi 3.14159265358932384636
#define min_double_error 1e-10

inline double degree_to_radians(double degrees) {
    return degrees * Pi / 180.0;
}

// random function [0,1)
inline double random_double() {
    return double(std::rand()) / (RAND_MAX + 1.0);
}

// random function [min,max)
inline double random_double(double min, double max) {
    return min + random_double() * (max - min);
}

// random function {min,min+1,...,max-1}
inline int random_int(int min, int max) {
    return int(random_double(min, max));
}

inline bool if_next_bounce(double next_bounce_ratio) {
    if (random_double() < next_bounce_ratio)
        return true;
    return false;
}
#endif