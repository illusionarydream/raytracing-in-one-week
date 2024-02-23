#ifndef MATH_MATERIALS_H
#define MATH_MATERIALS_H
#include <cmath>
#include <limits>
#include <memory>

#define infinity std::numeric_limits<double>::infinity()
#define Pi 3.14159265358932384636

inline double degree_to_radians(double degrees){
    return degrees*Pi/180.0;
}

#endif