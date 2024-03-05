#ifndef RAY_H
#define RAY_H
#include "vec3.h"
class Ray {
   public:
    Ray() {}
    Ray(const Point3 &origin, const Vec3 &direction, double _tm = 0.0) : orig(origin), dir(direction), tm(_tm) {}
    Vec3 at(const double &t) const {
        return orig + t * dir;
    }
    Point3 origin() const {
        return orig;
    }
    Vec3 direction() const {
        return dir;
    }
    double time() const {
        return tm;
    }

   private:
    Point3 orig;
    Vec3 dir;
    double tm;
};

#endif