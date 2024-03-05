#ifndef MATERIAL_H
#define MATERIAL_H
#include "vec3.h"
#include "ray.h"
#include "color.h"
#include "texture.h"
// *material表征模型的光学信息
class Hit_record;

class Material {
   public:
    virtual ~Material() = default;
    virtual Color emitted(double u, double v, const Point3& p) const {
        return Color(0.0, 0.0, 0.0);
    }
    // 根据入射光，反射点，最后得到散射光对RGB三通道的削弱程度和反射光线
    virtual bool scatter(const Ray& r_in, const Hit_record& rec, Color& attenuation, Ray& scattered) const = 0;
};

// * 发生漫反射
class Lambertian : public Material {
   private:
    // 吸收率
    Color albedo;
    // 材质贴图
    shared_ptr<Texture> tex;
    bool if_tex;

   public:
    Lambertian(const Color& attenuation)
        : albedo(attenuation),
          if_tex(false) {}
    Lambertian(const Color& attenuation, shared_ptr<Texture> _tex)
        : albedo(attenuation),
          if_tex(true),
          tex(_tex) {}
    Lambertian(shared_ptr<Texture> _tex)
        : albedo(Color(1.0, 1.0, 1.0)),
          if_tex(true),
          tex(_tex) {}

    Color get_attenuation(const Hit_record& rec) const {
        if (if_tex == false)
            return albedo;
        auto tex_attenuation = tex->get_texture_color(rec.u, rec.v, rec.p);
        return albedo * tex_attenuation;
    }
    bool scatter(const Ray& r_in, const Hit_record& rec, Color& attenuation, Ray& scattered)
        const override {
        auto new_direction = rec.normal + random_unit_vector();

        // if the random vec is opposite of the rec.normal
        if (new_direction.near_zero() == true) {
            new_direction = rec.normal;
        }
        scattered = Ray(rec.p, unit_vector(new_direction), r_in.time());
        attenuation = get_attenuation(rec);
        return true;
    }
};

// * 发生反射
class Metal : public Material {
   private:
    // 吸收率
    Color albedo;
    // 材质贴图
    bool if_tex;
    shared_ptr<Texture> tex;
    // 漫反射性质
    double fuzz;

   public:
    Metal(double f)
        : albedo(Color(1.0, 1.0, 1.0)),
          fuzz(f < 1.0 - min_double_error ? f : 1.0 - min_double_error),
          if_tex(false) {}
    Metal(const Color& attenuation, double f)
        : albedo(attenuation),
          fuzz(f < 1.0 - min_double_error ? f : 1.0 - min_double_error),
          if_tex(false) {}
    Metal(const Color& attenuation, double f, shared_ptr<Texture> _tex)
        : albedo(attenuation),
          fuzz(f < 1.0 - min_double_error ? f : 1.0 - min_double_error),
          if_tex(true),
          tex(_tex) {}
    Metal(double f, shared_ptr<Texture> _tex)
        : albedo(Color(1.0, 1.0, 1.0)),
          fuzz(f),
          if_tex(true),
          tex(_tex) {}

    Color get_attenuation(const Hit_record& rec) const {
        if (if_tex == false)
            return albedo;
        auto tex_attenuation = tex->get_texture_color(rec.u, rec.v, rec.p);
        return albedo * tex_attenuation;
    }
    bool scatter(const Ray& r_in, const Hit_record& rec, Color& attenuation, Ray& reflected)
        const override {
        Vec3 new_direction = reflect(r_in.direction(), rec.normal);
        reflected = Ray(rec.p, unit_vector(new_direction + fuzz * random_in_unit_sphere()), r_in.time());
        attenuation = get_attenuation(rec);
        return true;
    }
};

// * 可以发生折射也可以发生反射
class Dielectric : public Material {
   private:
    Color albedo;
    // *默认是表面外比表面内
    bool if_tex;
    shared_ptr<Texture> tex;
    // 折射反射情况
    double etai_over_etat;
    // *判断是折射还是反射，1-反射，0-折射
    bool judge_reflect_or_refract(double refraction_ratio, double cosine_theta) const {
        // Use Schlick's approximation for reflectance.
        auto r0 = (1 - refraction_ratio) / (1 + refraction_ratio);
        r0 = r0 * r0;
        auto p = r0 + (1 - r0) * pow((1 - cosine_theta), 5);
        if (random_double() > p)
            return 0;
        return 1;
    }

   public:
    Dielectric(double etai_)
        : albedo(Color(1.0, 1.0, 1.0)),
          etai_over_etat(etai_),
          if_tex(false) {}
    Dielectric(const Vec3& attentuation, double etai_)
        : albedo(attentuation),
          etai_over_etat(etai_),
          if_tex(false) {}
    Dielectric(const Vec3& attentuation, double etai_, shared_ptr<Texture> _tex)
        : albedo(attentuation),
          etai_over_etat(etai_),
          if_tex(true),
          tex(_tex) {}
    Dielectric(double etai_, shared_ptr<Texture> _tex)
        : albedo(Color(1.0, 1.0, 1.0)),
          etai_over_etat(etai_),
          if_tex(true),
          tex(_tex) {}

    Color get_attenuation(const Hit_record& rec) const {
        if (if_tex == false)
            return albedo;
        auto tex_attenuation = tex->get_texture_color(rec.u, rec.v, rec.p);
        return albedo * tex_attenuation;
    }
    bool scatter(const Ray& r_in, const Hit_record& rec, Color& attenuation, Ray& refracted)
        const override {
        Vec3 new_direction;
        attenuation = get_attenuation(rec);
        // 归一化入射光向量
        auto unit_dir = unit_vector(r_in.direction());
        // 确定光线是在内部发生折射还是外部发生折射
        auto real_etai_over_etat = rec.front_face ? 1 / etai_over_etat : etai_over_etat;
        // 计算法向量和入射光向量的余弦
        auto cos_theta = -dot(unit_dir, rec.normal);
        // 确定是发生反射还是发生折射

        if (judge_reflect_or_refract(real_etai_over_etat, cos_theta) == true) {
            new_direction = reflect(unit_dir, rec.normal);
        } else {
            new_direction = refraction(unit_dir, rec.normal, real_etai_over_etat);
        }
        refracted = Ray(rec.p, unit_vector(new_direction), r_in.time());
        return true;
    }
};
class Diffuse_light : public Material {
   private:
    shared_ptr<Texture> emit;
    Color albedo;

   public:
    Diffuse_light(shared_ptr<Texture> _emit)
        : albedo(Color(1.0, 1.0, 1.0)),
          emit(_emit) {}
    Diffuse_light(const Vec3& attentuation, shared_ptr<Texture> _emit)
        : albedo(attentuation),
          emit(_emit) {}
    Diffuse_light(const Color& c)
        : albedo(Color(1.0, 1.0, 1.0)),
          emit(make_shared<solid_color>(c)) {}

    bool scatter(const Ray& r_in, const Hit_record& rec, Color& attenuation, Ray& refracted)
        const override {
        return false;
    }
    Color emitted(double u, double v, const Point3& p)
        const override {
        return emit->get_texture_color(u, v, p);
    }
};

#endif