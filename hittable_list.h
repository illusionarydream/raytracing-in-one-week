#ifndef HITTABLE_LIST
#define HITTABLE_LIST
#include "hittable.h"
#include <memory>
#include <vector>
using std::make_shared;
using std::shared_ptr;

// *对于刚性物体而言确实如此，一条光线和一系列刚性物体理应只有一个交点
class Hittable_list : public Hittable {
   public:
    Hittable_list() {}
    Hittable_list(shared_ptr<Hittable> new_object) {
        add(new_object);
    }
    void clear() {
        objects.clear();
    }
    void add(shared_ptr<Hittable> new_object) {
        objects.push_back(new_object);
        list_box = AABB(list_box, new_object->bounding_box());
        return;
    }
    bool hit(const Ray &r, Interval ray_t, Hit_record &rec) const override {
        Hit_record tmp_rec;
        double r_closest_tmax = ray_t.get_tmax();
        bool if_hit = false;
        for (auto &object : objects) {
            if (object->hit(r, Interval(ray_t.get_tmin(), r_closest_tmax), tmp_rec) == true) {
                if_hit = true;
                r_closest_tmax = tmp_rec.t;
                rec = tmp_rec;
            }
        }
        return if_hit;
    }
    AABB bounding_box() const override {
        return list_box;
    }
    const std::vector<shared_ptr<Hittable>> get_objects() const {
        return objects;
    }

   private:
    std::vector<shared_ptr<Hittable>> objects;
    AABB list_box;
};
#endif