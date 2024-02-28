#ifndef BVH_H
#define BVH_H
#include "hittable.h"
#include "math_materials.h"
#include "hittable_list.h"
#include "algorithm"

class BVH_node : public hittable {
   private:
    /* data */
    AABB BVH_node_box;
    shared_ptr<hittable> left_node;
    shared_ptr<hittable> right_node;

   private:
    inline static bool compare_in_x(const shared_ptr<hittable> X, const shared_ptr<hittable> Y) {
        return X->bounding_box().x.get_tmin() < Y->bounding_box().x.get_tmin();
    }
    inline static bool compare_in_y(const shared_ptr<hittable> X, const shared_ptr<hittable> Y) {
        return X->bounding_box().y.get_tmin() < Y->bounding_box().y.get_tmin();
    }
    inline static bool compare_in_z(const shared_ptr<hittable> X, const shared_ptr<hittable> Y) {
        return X->bounding_box().z.get_tmin() < Y->bounding_box().z.get_tmin();
    }

   public:
    BVH_node(const hittable_list& obejcts_list)
        : BVH_node(obejcts_list.get_objects(), 0, obejcts_list.get_objects().size()) {}
    // 构建BVH树
    BVH_node(const std::vector<shared_ptr<hittable>>& src_objects, size_t start, size_t end) {
        std::vector<shared_ptr<hittable>> objects = src_objects;  // modifiedable vector
        // 确定分割轴
        int divide_axis = random_int(0, 3);
        // 确定是哪个轴的比较函数
        auto comparator = divide_axis == 0 ? compare_in_x
                                           : (divide_axis == 1 ? compare_in_y
                                                               : compare_in_z);
        // 确定该box包含的模型数量
        size_t objects_span = end - start;
        // 对左右节点的确定，
        // 1--左右节点直接设置为模型
        // 2--左右节点分别设置为模型
        if (objects_span == 1) {
            left_node = right_node = objects[start];
        } else if (objects_span == 2) {
            left_node = objects[start];
            right_node = objects[start + 1];
        }
        // 对span>2的情况
        else {
            // 先按轴排序
            std::sort(objects.begin() + start, objects.begin() + end, comparator);
            // 取中点
            auto mid_place = (start + end) >> 1;
            // 左节点：start,start+1,...,mid_place-1
            left_node = make_shared<BVH_node>(objects, start, mid_place);
            // 右节点：mid_place,mid_place+1,...,end-1
            right_node = make_shared<BVH_node>(objects, mid_place, end);
        }
        BVH_node_box = AABB(left_node->bounding_box(), right_node->bounding_box());
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        // 整体如果未击中，则必定未击中
        if (BVH_node_box.if_hit(r, ray_t) == false)
            return false;
        // 左侧如果击中，则必定击中
        bool if_hit_left = left_node->hit(r, ray_t, rec);
        // 右侧如果击中，则击中
        auto cur_interval = interval(ray_t.get_tmin(), if_hit_left == true ? rec.t : ray_t.get_tmax());
        bool if_hit_right = right_node->hit(r, cur_interval, rec);

        return if_hit_right || if_hit_left;
    }
    AABB bounding_box() const override {
        return BVH_node_box;
    }
};

#endif