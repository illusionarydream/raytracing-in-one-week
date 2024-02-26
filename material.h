#ifndef MATERIAL_H
#define MATERIAL_H
#include "vec3.h"
#include "ray.h"
#include "color.h"
// *material表征模型的光学信息
class hit_record;

class material {
   public:
    virtual ~material() = default;

    // 根据入射光，反射点，最后得到散射光对RGB三通道的削弱程度和反射光线
    virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const = 0;
};

class Lambertian : public material {
   private:
    /* data */
    color albedo;

   public:
    Lambertian(const color& attenuation) : albedo(attenuation) {}
    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered)
        const override {
        auto new_direction = rec.normal + random_in_unit_sphere();

        // if the random vec is opposite of the rec.normal
        if (new_direction.near_zero() == true)
            new_direction = rec.normal;
        scattered = ray(rec.p, unit_vector(new_direction));
        attenuation = albedo;
        return true;
    }
};

#endif