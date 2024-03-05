#ifndef QUAD_H
#define QUAD_H
#include "math_materials.h"
#include "hittable.h"
#include "vec3.h"
class Quad : public Hittable {
   private:
    // 基点
    Point3 Q;
    // 四边形的一边向量
    Vec3 u;
    // 四边形的另一边向量
    Vec3 v;
    // 法线 (a,b,c) not unit
    Vec3 normal;
    // D=a*x+b*y+c*z
    double D;
    // 材料
    std::shared_ptr<Material> mat;
    // 四边形的bounding box
    AABB quad_box;
    // w=n/(cross(u,v)*n)
    Vec3 w;

   private:
    void get_texture_uv(const Point3& P, double& tex_u, double& tex_v) {
        auto Q_to_P = P - Q;
    }

   public:
    Quad(const Point3& _Q, const Vec3& _u, const Vec3& _v, std::shared_ptr<Material> _mat)
        : Q(_Q),
          u(_u),
          v(_v),
          mat(_mat) {
        auto n = cross(u, v);
        normal = unit_vector(n);
        D = dot(normal, Q);
        w = n / dot(n, n);

        auto tmp_box = AABB(Q, Q + u + v);
        tmp_box.padding();
        quad_box = tmp_box;
    }
    AABB bounding_box() const override {
        return quad_box;
    }
    bool hit(const Ray& r, Interval ray_t, Hit_record& rec) const override {
        // 如果光线是平行于四边形表面的话
        auto n_dot_d = dot(normal, r.direction());
        if (std::fabs(n_dot_d) < min_double_error) {
            return false;
        }

        // 计算光线与四边形表面的交点
        auto root = (D - dot(normal, r.origin())) / n_dot_d;
        if (ray_t.surrounds(root))
            return false;
        // 验证该交点是否位于四边形的范围内部
        auto P = r.at(root);
        auto p = P - Q;
        auto alpha = dot(cross(p, v), w);
        auto beta = dot(cross(u, p), w);
        Interval inv_0_to_1 = Interval(0.0, 1.0);
        if (!(inv_0_to_1.contains(alpha) && inv_0_to_1.contains(beta)))
            return false;
        // 如果光线成功地击打到了四边形表面，就记录该碰撞点的信息
        rec.u = alpha;
        rec.v = beta;
        rec.t = root;
        rec.p = P;
        rec.mat = mat;
        rec.set_surface_normal(r, normal);

        return true;
    }
};
#endif
