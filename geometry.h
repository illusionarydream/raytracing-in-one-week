#ifndef GEOMETRY_H
#define GEOMETRY_H
#include "hittable_list.h"
#include "quad.h"
// *基础图形的组合：球体和面的组合

inline shared_ptr<Hittable_list> produce_box(const Point3 &a, const Point3 &b, shared_ptr<Material> mat) {
    auto sides = make_shared<Hittable_list>();
    // 定义左下角点，右上角点
    auto min_point = a;
    auto max_point = b;
    // 确定最低点
    for (int i = 0; i < 3; i++)
        if (min_point.e[i] > max_point.e[i])
            std::swap(min_point.e[i], max_point.e[i]);
    // 计算长宽高
    auto dx = max_point.x() - min_point.x();
    auto dy = max_point.y() - min_point.y();
    auto dz = max_point.z() - min_point.z();
    auto vx = Vec3(dx, 0.0, 0.0);
    auto vy = Vec3(0.0, dy, 0.0);
    auto vz = Vec3(0.0, 0.0, dz);
    // 加入面的集合
    sides->add(make_shared<Quad>(Point3(min_point.x(), min_point.y(), max_point.z()), vx, vy, mat));   // front
    sides->add(make_shared<Quad>(Point3(max_point.x(), min_point.y(), max_point.z()), -vz, vy, mat));  // right
    sides->add(make_shared<Quad>(Point3(max_point.x(), min_point.y(), min_point.z()), -vx, vy, mat));  // back
    sides->add(make_shared<Quad>(Point3(min_point.x(), min_point.y(), min_point.z()), vz, vy, mat));   // left
    sides->add(make_shared<Quad>(Point3(min_point.x(), max_point.y(), max_point.z()), vx, -vz, mat));  // top
    sides->add(make_shared<Quad>(Point3(min_point.x(), min_point.y(), min_point.z()), vx, vz, mat));   // bottom

    return sides;
}
#endif