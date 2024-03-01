#ifndef INTERVAL_H
#define INTERVAL_H
#include "math_materials.h"
#include "vec3.h"
class Interval {
   public:
    Interval() : tmin(-infinity), tmax(infinity) {}
    Interval(double t_min, double t_max) : tmin(t_min), tmax(t_max) {}
    Interval(const Interval& i1, const Interval& i2) {
        tmin = std::min(i1.get_tmin(), i2.get_tmin());
        tmax = std::max(i1.get_tmax(), i2.get_tmax());
    }
    inline bool contains(double x) {
        return x >= tmin && x <= tmax;
    }
    inline bool surrounds(double x) {
        return !contains(x);
    }
    double get_tmin() const {
        return tmin;
    }
    double get_tmax() const {
        return tmax;
    }
    double clamp(double x) {
        if (x < tmin) return tmin;
        if (x > tmax) return tmax;
        return x;
    }
    double get_dist() {
        return tmax - tmin;
    }
    Interval expand(double delta) {
        double padding = delta / 2;
        return Interval(tmin - padding, tmax + padding);
    }
    static const Interval empty, universe;

   private:
    double tmin;
    double tmax;
};
const static Interval empty(+infinity, -infinity);
const static Interval universe(-infinity, +infinity);

#endif