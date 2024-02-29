#ifndef HITTABLE_H
#define HITTABLE_H
#include "ray.h"
#include "interval.h"
#include "AABB.h"
class material;

class hit_record {
   public:
    // 交点信息
    point3 p;
    // 交点法线信息
    vec3 normal;
    // 交点距离信息
    double t;
    // 交点法线朝向
    bool front_face;
    // 交点材质信息
    std::shared_ptr<material> mat;
    // texture平面坐标
    double u;
    double v;

   public:
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
    virtual AABB bounding_box() const = 0;
};

#endif