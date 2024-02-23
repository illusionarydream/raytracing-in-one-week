#ifndef INTERVAL_H
#define INTERVAL_H
#include "math_materials.h"
#include "vec3.h"
class interval{

public:
    interval():tmin(-infinity),tmax(infinity) {}
    interval(double t_min,double t_max):tmin(t_min),tmax(t_max) {}
    inline bool contains(double x){
        return x>=tmin && x<=tmax;
    }  
    inline bool surrounds(double x){
        return !contains(x);
    }
    double get_tmin() const{
        return tmin;
    }
    double get_tmax() const {
        return tmax;
    }
    static const interval empty, universe;
private:
    double tmin;
    double tmax;

};
const static interval empty (+infinity, -infinity);
const static interval universe (-infinity, +infinity);

#endif