#ifndef TEXTURE_H
#define TEXTURE_H
#include "vec3.h"
#include "color.h"
#include "rtw_image.h"
#include "math_materials.h"
#include "perlin.h"
#include <memory>
using std::make_shared;
using std::shared_ptr;
class Texture {
   public:
    virtual ~Texture() = default;
    virtual Color get_texture_color(double u, double v, const Point3 &p) const = 0;
};
class solid_color : public Texture {
   private:
    Color color_value;

   public:
    solid_color(const Color &_value) : color_value(_value) {}

    solid_color(double _r, double _g, double _b) : color_value(_r, _g, _b) {}

    Color get_texture_color(double u, double v, const Point3 &p) const override {
        return color_value;
    }
};
class Check_board : public Texture {
   private:
    shared_ptr<Texture> even_color;
    shared_ptr<Texture> odd_color;
    double scale;

   public:
    Check_board(double _scale, shared_ptr<Texture> _even_col, shared_ptr<Texture> _odd_col)
        : scale(_scale),
          even_color(_even_col),
          odd_color(_odd_col) {}
    Check_board(double _scale, const Color &_even_col, const Color &_odd_col)
        : scale(_scale),
          even_color(make_shared<solid_color>(_even_col)),
          odd_color(make_shared<solid_color>(_odd_col)) {}
    Color get_texture_color(double u, double v, const Point3 &p) const override {
        double inv_scale = 1.0 / scale;
        int integer_x = int(p.x() * inv_scale);
        int integer_y = int(p.y() * inv_scale);
        int integer_z = int(p.z() * inv_scale);
        int odd_or_even = (integer_x + integer_y + integer_z) % 2;
        return odd_or_even == 0 ? even_color->get_texture_color(u, v, p)
                                : odd_color->get_texture_color(u, v, p);
    }
};
class Image_texture : public Texture {
   private:
    Rtw_image image;

   public:
    Image_texture(const char *filename) : image(filename) {}
    Color get_texture_color(double u, double v, const Point3 &p) const override {
        auto standard_inv = Interval(0.0, 1.0);
        u = standard_inv.clamp(u);
        v = 1.0 - standard_inv.clamp(v);

        auto i = int(u * image.get_width());
        auto j = int(v * image.get_height());

        // std::clog << "i=" << i << " j=" << j << std::endl;
        return image.get_pixel_color(i, j);
    }
};
class Noise_texture : public Texture {
   private:
    Perlin noise_tex;
    double scale;

   public:
    Noise_texture() : scale(1.0) {}
    Noise_texture(double _sc) : scale(_sc) {}
    Color get_texture_color(double u, double v, const Point3 &p) const override {
        // scale
        auto p_after_scale = p * scale;
        // get grey noise color
        auto noise_color = noise_tex.get_turb_grey_noise_color(p_after_scale, 4);
        // 一些后期变换
        noise_color = Color(1.0, 1.0, 1.0) * 0.5 * (1 + std::sin(p_after_scale.z() + 10 * noise_color.x()));

        return noise_color;
    }
};
#endif