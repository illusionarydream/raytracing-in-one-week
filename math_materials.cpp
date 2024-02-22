#ifndef MATH_MATERIALS
#define MATH_MATERIALS
#include <cmath>
#include <limits>
#include <memory>

#define infinity std::numeric_limits<double>::infinity()
#define M_PI 3.14159265358932384636

inline double degree_to_radians(double degrees){
    return degrees*M_PI/180.0;
}

#endif