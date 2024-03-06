#ifndef CONSTANT_MEDIA_H
#define CONSTANT_MEDIA_H
#include "material.h"
#include "hittable.h"

class Constant_media : public Hittable {
   private:
    // 范围
    shared_ptr<Hittable> boundary;
    // 包围盒
    AABB constant_box;
    // 密度
    double density;
    // 用于计算的1/ln(1-density)
    double ln_1_density;
    // 材料
    shared_ptr<Material> mat;

   public:
    Constant_media(shared_ptr<Hittable> _boundary, double _density, shared_ptr<Texture> _mat)
        : boundary(_boundary),
          density(_density > min_double_error ? _density : min_double_error),
          mat(make_shared<Isotropic>(_mat)),
          constant_box(boundary->bounding_box()) {
        ln_1_density = 1.0 / std::log(1 - density);
    }
    Constant_media(shared_ptr<Hittable> _boundary, double _density, const Color& _mat)
        : boundary(_boundary),
          density(_density > min_double_error ? _density : min_double_error),
          mat(make_shared<Isotropic>(_mat)),
          constant_box(boundary->bounding_box()) {
        ln_1_density = 1.0 / std::log(1 - density);
    }

    bool hit(const Ray& r, Interval ray_t, Hit_record& rec)
        const override {
        // 定义介质进出的两个交点
        Hit_record rec1, rec2;
        // 先计算进入的交点
        if (!boundary->hit(r, universe, rec1))
            return false;
        // 在计算出去的交点
        if (!boundary->hit(r, Interval(rec1.t + min_double_error, infinity), rec2))
            return false;
        // 确定真实可以发生散射的范围
        if (rec1.t < ray_t.get_tmin()) rec1.t = ray_t.get_tmin();
        if (rec2.t > ray_t.get_tmax()) rec2.t = ray_t.get_tmax();
        // 排除不相交可能
        if (rec1.t > rec2.t)
            return false;
        // 计算在光路中的哪个点发生散射
        auto p1 = r.at(rec1.t);
        auto p2 = r.at(rec2.t);
        auto distance_in_boundary = (p2 - p1).length();
        auto hit_distance = std::log(random_double()) * ln_1_density;
        // 判断交点是否在范围内
        if (hit_distance >= distance_in_boundary)
            return false;
        // 如果交点在范围内的话
        auto hit_t = rec1.t + hit_distance / r.direction().length();
        auto hit_p = r.at(hit_t);
        // 更新rec值
        rec.t = hit_t;
        rec.p = hit_p;
        // 更新其他信息，随机即可，因为云雾介质都是各项同性的，所以跟法线信息毫无关系
        rec.normal = Vec3(1, 0, 0);
        rec.front_face = true;
        rec.mat = mat;
        // std::clog << density << std::endl;
        return true;
    }
    AABB bounding_box() const override {
        return constant_box;
    }
};

#endif