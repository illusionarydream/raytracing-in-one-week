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

// * 发生漫反射
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

// * 发生反射
class metal : public material {
   private:
    color albedo;
    double fuzz;

   public:
    metal(const color& attenuation, double f) : albedo(attenuation), fuzz(f < 1.0 - min_double_error ? f : 1.0 - min_double_error) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& reflected)
        const override {
        vec3 new_direction = reflect(r_in.direction(), rec.normal);
        reflected = ray(rec.p, unit_vector(new_direction + fuzz * random_in_unit_sphere()));
        attenuation = albedo;
        return true;
    }
};

// * 可以发生折射也可以发生反射
class dielectric : public material {
   private:
    color albedo;
    // *默认是表面外比表面内
    double etai_over_etat;

   public:
    dielectric(const vec3& attentuation, double etai_) : albedo(attentuation), etai_over_etat(etai_) {}
    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& refracted)
        const override {
        attenuation = albedo;
        auto real_etai_over_etat = rec.front_face ? 1 / etai_over_etat : etai_over_etat;
        vec3 new_direction = refraction(r_in.direction(), rec.normal, real_etai_over_etat);
        refracted = ray(rec.p, unit_vector(new_direction));
        return true;
    }
};
#endif