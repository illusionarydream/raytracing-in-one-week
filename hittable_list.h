#ifndef HITTABLE_LIST
#define HITTABLE_LIST
#include "hittable.h"
#include <memory>
#include <vector>
#define shared_ptr std::shared_ptr
#define make_shared std::make_shared

// *对于刚性物体而言确实如此，一条光线和一系列刚性物体理应只有一个交点
class hittable_list : public hittable {
   public:
    hittable_list() {}
    hittable_list(shared_ptr<hittable> new_object) {
        add(new_object);
    }
    void clear() {
        objects.clear();
    }
    void add(shared_ptr<hittable> new_object) {
        objects.push_back(new_object);
        return;
    }
    bool hit(const ray &r, interval ray_t, hit_record &rec) const override {
        hit_record tmp_rec;
        double r_closest_tmax = ray_t.get_tmax();
        bool if_hit = false;
        for (auto &object : objects) {
            if (object->hit(r, interval(ray_t.get_tmin(), r_closest_tmax), tmp_rec) == true) {
                if_hit = true;
                r_closest_tmax = rec.t;
                rec = tmp_rec;
            }
        }
        return if_hit;
    }

   private:
    std::vector<shared_ptr<hittable>> objects;
};
#endif