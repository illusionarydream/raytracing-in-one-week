#ifndef SPHERE_H
#define SPHERE_H
#include "vec3.h"
#include "ray.h"
#include "hittable.h"
#include "material.h"
#include "AABB.h"
#include <cmath>
class Sphere : public Hittable {
   public:
    Sphere() {}
    Sphere(const Point3 &cen, double r, std::shared_ptr<Material> _mat)
        : origin_center(cen),
          radius(r),
          mat(_mat),
          if_move(false) {
        produce_bounding_box();
    }
    Sphere(const Point3 &cen, double r, const Vec3 &vel, std::shared_ptr<Material> _mat)
        : origin_center(cen),
          radius(r),
          mat(_mat),
          if_move(false),
          velocity(vel) {
        produce_bounding_box();
    }
    bool hit(const Ray &r, Interval ray_t, Hit_record &rec) const override {
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
        Vec3 outward_normal = (rec.p - C) / radius;
        get_texture_uv(outward_normal, rec.u, rec.v);
        rec.set_surface_normal(r, outward_normal);
        rec.mat = mat;
        return true;
    }
    Vec3 get_origin_center() {
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
    Point3 origin_center;
    // 半径
    double radius;
    // 材质
    std::shared_ptr<Material> mat;
    // 是否移动
    bool if_move;
    // 移动速度，目前仅仅模拟简单的直线运动
    Vec3 velocity;
    // bounding_box
    AABB sphere_box;

   private:
    Vec3 center(double t) const {
        return origin_center + velocity * t;
    }
    void produce_bounding_box() {
        auto rvec = Vec3(radius, radius, radius);
        // 设置曝光的总时长不会超过1，
        // 把bounding_box的运动局限在1中
        auto center0 = center(0);
        auto center1 = center(1);
        AABB box1(center0 - rvec, center0 + rvec);
        AABB box2(center1 - rvec, center1 + rvec);
        sphere_box = AABB(box1, box2);
    }
    void get_texture_uv(const Vec3 &outward_normal, double &u, double &v) const {
        auto unit_center_to_surface = unit_vector(outward_normal);
        auto X = unit_center_to_surface.x();
        auto Y = unit_center_to_surface.y();
        auto Z = unit_center_to_surface.z();

        auto theta = std::acos(-Y);
        auto phi = std::atan2(-Z, X) + Pi;

        u = phi / (2 * Pi);
        v = theta / Pi;
    }
};
#endif