#ifndef MATH_MATERIALS_H
#define MATH_MATERIALS_H
#include <cmath>
#include <limits>
#include <memory>
#include <cstdlib>

#define infinity std::numeric_limits<double>::infinity()
#define Pi 3.14159265358932384636

inline double degree_to_radians(double degrees) {
    return degrees * Pi / 180.0;
}

// random function [0,1)
inline double random_double() {
    return double(std::rand()) / (RAND_MAX + 1.0);
}

// random function [0,1)
inline double random_double(double min, double max) {
    return min + random_double() * (max - min);
}
#endif