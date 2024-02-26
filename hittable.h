#ifndef HITTABLE_H
#define HITTABLE_H
#include "ray.h"
#include "interval.h"
class material;

class hit_record {
   public:
    point3 p;
    vec3 normal;
    double t;
    bool front_face;
    std::shared_ptr<material> mat;
    // *默认法线是朝向面外的
    void set_surface_normal(const ray& r, const vec3& outward_normal) {
        if (dot(r.direction(), outward_normal) > 0.0) {
            normal = -outward_normal;
            front_face = false;
        } else {
            normal = outward_normal;
            front_face = true;
        }
        return;
    }
};
// *hittable刻画模型的集合信息
class hittable {
   public:
    virtual ~hittable() = default;
    virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;
};

#endif