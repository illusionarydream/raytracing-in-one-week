#ifndef TRANSLATE_H
#define TRANSLATE_H
#include "hittable.h"
// *实际上是对已经存在hittable类型的一个包装
class Translate : public Hittable {
   private:
    // 偏移量
    Vec3 offset;
    // 包装物体
    std::shared_ptr<Hittable> objects;
    // 包围盒
    AABB trans_box;

   public:
    Translate(std::shared_ptr<Hittable> _obj, const Vec3& _offset)
        : objects(_obj),
          offset(_offset) {
        trans_box = objects->bounding_box() + offset;
    }
    bool hit(const Ray& r, Interval ray_t, Hit_record& rec)
        const override {
        // 先将光线移动-offset的距离
        auto offset_ray = Ray(r.origin() - offset, r.direction(), r.time());
        // 判断相交
        auto if_hit = objects->hit(offset_ray, ray_t, rec);
        if (!if_hit)
            return false;
        // 将相交得到的交点重新给移回原来的位置
        rec.p += offset;

        return true;
    }
    AABB bounding_box()
        const override {
        return trans_box;
    }
};

class Rotate_y : public Hittable {
   private:
    // 偏移角度,弧度制度
    double theta;
    double cos_theta;
    double sin_theta;
    // 包装物体
    std::shared_ptr<Hittable> objects;
    // 包围盒
    AABB rotate_box;

   private:
    // 对一个点进行旋转变换
    void rotate_point_positive(Point3& p) {
        p[0] = cos_theta * p[0] + sin_theta * p[2];
        p[2] = -sin_theta * p[0] + cos_theta * p[2];
        return;
    }

    void rotate_point_negative(Point3& p) {
        p[0] = cos_theta * p[0] - sin_theta * p[2];
        p[2] = sin_theta * p[0] + cos_theta * p[2];
    }

    Point3 rotate_point_positive(const Point3& _p) const {
        auto p = _p;
        p[0] = cos_theta * p[0] + sin_theta * p[2];
        p[2] = -sin_theta * p[0] + cos_theta * p[2];
        return p;
    }

    Point3 rotate_point_negative(const Point3& _p) const {
        auto p = _p;
        p[0] = cos_theta * p[0] - sin_theta * p[2];
        p[2] = sin_theta * p[0] + cos_theta * p[2];
        return p;
    }

   public:
    Rotate_y(std::shared_ptr<Hittable> _obj, const double& angle)
        : objects(_obj),
          theta(degree_to_radians(angle)) {
        cos_theta = std::cos(theta);
        sin_theta = std::sin(theta);
        rotate_box = objects->bounding_box();
        // 移动box的各个极点
        Point3 min_inv(infinity, infinity, infinity);
        Point3 max_inv(-infinity, -infinity, -infinity);
        for (int i = 0; i < 2; i++)
            for (int j = 0; j < 2; j++)
                for (int k = 0; k < 2; k++) {
                    auto x = i * rotate_box.x.get_tmax() + (1 - i) * rotate_box.x.get_tmin();
                    auto y = i * rotate_box.y.get_tmax() + (1 - i) * rotate_box.y.get_tmin();
                    auto z = i * rotate_box.z.get_tmax() + (1 - i) * rotate_box.z.get_tmin();

                    auto p = Point3(x, y, z);
                    rotate_point_positive(p);

                    for (int i = 0; i < 3; i++) {
                        min_inv.e[i] = std::min(min_inv.e[i], p.e[i]);
                        max_inv.e[i] = std::max(max_inv.e[i], p.e[i]);
                    }
                }
        // 更新box
        rotate_box = AABB(min_inv, max_inv);
    }

    bool hit(const Ray& r, Interval ray_t, Hit_record& rec)
        const override {
        // 旋转光线
        auto rotate_origin = rotate_point_negative(r.origin());
        auto rotate_direction = rotate_point_negative(r.direction());
        auto rotate_ray = Ray(rotate_origin, rotate_direction, r.time());
        // 判断相交
        bool if_hit = objects->hit(rotate_ray, ray_t, rec);
        if (!if_hit)
            return false;
        // 将相交的点重新旋转回来
        rotate_point_positive(rec.p);
        rotate_point_positive(rec.normal);

        return true;
    }
    AABB bounding_box() const override {
        return rotate_box;
    }
};
#endif
