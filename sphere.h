#ifndef SPHERE_H
#define SPHERE_H
#include "vec3.h"
#include "ray.h"
#include "hittable.h"
#include "material.h"
class sphere : public hittable {
   public:
    sphere() {}
    sphere(const point3 &cen, double r, std::shared_ptr<material> _mat) : center(cen), radius(r), mat(_mat) {}
    bool if_hit_sphere(const ray &r) {
        auto D = r.direction();
        auto O = r.origin();
        auto C = center;
        auto R = radius;
        auto a = dot(D, D);
        auto half_b = dot(D, O - C);
        auto c = dot(O - C, O - C) - R * R;
        auto discriminant = half_b * half_b - a * c;

        return (discriminant >= 0);
    }  // 1--hit the sphere, 0--non-hit the sphere
    double hit_sphere(const ray &r) {
        auto D = r.direction();
        auto O = r.origin();
        auto C = center;
        auto R = radius;
        auto a = dot(D, D);
        auto half_b = dot(D, O - C);
        auto c = dot(O - C, O - C) - R * R;
        auto discriminant = half_b * half_b - a * c;

        if (discriminant >= 0)
            return (-half_b - sqrt(discriminant)) / a;
        else
            return -1.0;
    }
    bool hit(const ray &r, interval ray_t, hit_record &rec) const override {
        auto D = r.direction();
        auto O = r.origin();
        auto C = center;
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
        vec3 outward_normal = (rec.p - center) / radius;
        rec.set_surface_normal(r, outward_normal);
        rec.mat = mat;
        return true;
    }
    vec3 get_center() {
        return center;
    }
    double get_radius() {
        return radius;
    }

   private:
    point3 center;
    double radius;
    std::shared_ptr<material> mat;
};
#endif