#ifndef SPHERE_H
#define SPHERE_H
#include "vec3.h"
#include "ray.h"
#include "hittable.h"
#include "material.h"
#include "AABB.h"
class sphere : public hittable {
   public:
    sphere() {}
    sphere(const point3 &cen, double r, std::shared_ptr<material> _mat)
        : origin_center(cen),
          radius(r),
          mat(_mat),
          if_move(false) {
        produce_bounding_box();
    }
    sphere(const point3 &cen, double r, const vec3 &vel, std::shared_ptr<material> _mat)
        : origin_center(cen),
          radius(r),
          mat(_mat),
          if_move(false),
          velocity(vel) {
        produce_bounding_box();
    }
    bool hit(const ray &r, interval ray_t, hit_record &rec) const override {
        auto D = r.direction();
        auto O = r.origin();
        auto C = center(r.time());
        auto R = radius;
        auto a = dot(D, D);
        auto half_b = dot(D, O - C);
        auto c = dot(O - C, O - C) - R * R;
        auto discriminant = half_b * half_b - a * c;

        if (discriminant < 0.0)
            return false;
        auto sqrd = sqrt(discriminant);
        auto root = (-half_b - sqrd) / a;
        if (ray_t.surrounds(root)) {
            root = (-half_b + sqrd) / a;
            if (ray_t.surrounds(root))
                return false;
        }
        rec.t = root;
        rec.p = r.at(root);
        vec3 outward_normal = (rec.p - C) / radius;
        rec.set_surface_normal(r, outward_normal);
        rec.mat = mat;
        return true;
    }
    vec3 get_origin_center() {
        return origin_center;
    }
    double get_radius() {
        return radius;
    }
    AABB bounding_box() const override {
        return sphere_box;
    }

   private:
    // 球心
    point3 origin_center;
    // 半径
    double radius;
    // 材质
    std::shared_ptr<material> mat;
    // 是否移动
    bool if_move;
    // 移动速度，目前仅仅模拟简单的直线运动
    vec3 velocity;
    // bounding_box
    AABB sphere_box;

   private:
    vec3 center(double t) const {
        return origin_center + velocity * t;
    }
    void produce_bounding_box() {
        auto rvec = vec3(radius, radius, radius);
        // 设置曝光的总时长不会超过1，
        // 把bounding_box的运动局限在1中
        auto center0 = center(0);
        auto center1 = center(1);
        AABB box1(center0 - rvec, center0 + rvec);
        AABB box2(center1 - rvec, center1 + rvec);
        sphere_box = AABB(box1, box2);
    }
};
#endif