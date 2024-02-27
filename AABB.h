#ifndef AABB_H
#define AABB_H
#include "math_materials.h"
#include "interval.h"
#include "ray.h"
class AABB {
   public:
    interval x;
    interval y;
    interval z;
    AABB() {}
    AABB(const interval& _x, const interval& _y, const interval& _z)
        : x(_x),
          y(_y),
          z(_z) {}
    AABB(const point3& a, const point3& b)
        : x(std::min(a.x(), b.x()), std::max(a.x(), b.x())),
          y(std::min(a.y(), b.y()), std::max(a.y(), b.y())),
          z(std::min(a.z(), b.z()), std::max(a.z(), b.z())) {}
    AABB(const AABB& box1, const AABB& box2)
        : x(box1.x, box2.x),
          y(box1.y, box2.y),
          z(box1.z, box2.z) {}
    const interval& axis(int n) const {
        if (n == 0)
            return x;
        if (n == 1)
            return y;
        return z;
    }
    bool if_hit(const ray& r, const interval& ray_t) const {
        std::pair<double, double> cur_t(ray_t.get_tmin(), ray_t.get_tmax());
        for (int i = 0; i < 3; i++) {
            auto inv_D = 1 / r.direction().e[i];
            auto r_orig = r.origin().e[i];
            auto t0 = (axis(i).get_tmin() - r_orig) / inv_D;
            auto t1 = (axis(i).get_tmax() - r_orig) / inv_D;
            t0 = std::min(t0, t1);
            t1 = std::max(t0, t1);
            cur_t.first = std::max(cur_t.first, t0);
            cur_t.second = std::max(cur_t.second, t1);
            if (cur_t.first > cur_t.second - min_double_error)
                return false;
        }
        return true;
    }
};
#endif