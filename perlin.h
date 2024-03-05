#ifndef PERLIN_H
#define PERLIN_H
#include "vec3.h"
#include "math_materials.h"
#include "color.h"
#include "algorithm"
class Perlin {
   private:
    static const int point_count = 256;
    Vec3 *ranvec;
    int *perm_x;
    int *perm_y;
    int *perm_z;
    int *generate_permute_list() {
        auto p = new int[point_count];
        for (int i = 0; i < point_count; i++)
            p[i] = i;
        for (int i = 0; i < point_count - 1; i++) {
            auto place = random_int(i + 1, point_count);
            std::swap(p[i], p[place]);
        }
        return p;
    }

   public:
    Perlin() {
        ranvec = new Vec3[point_count];
        for (int i = 0; i < 256; i++)
            ranvec[i] = random_unit_vector();

        perm_x = generate_permute_list();
        perm_y = generate_permute_list();
        perm_z = generate_permute_list();
    }
    Color get_smooth_grey_noise_color(const Point3 &p) const {
        int x_floor = int(std::floor(p.x()));
        int y_floor = int(std::floor(p.y()));
        int z_floor = int(std::floor(p.z()));

        double u = p.x() - x_floor;
        double v = p.y() - y_floor;
        double w = p.z() - z_floor;

        double uu = u;
        double vv = v;
        double ww = w;

        u = u * u * (3 - 2 * u);
        v = v * v * (3 - 2 * v);
        w = w * w * (3 - 2 * w);

        Vec3 c[2][2][2];
        for (int i = 0; i < 2; i++)
            for (int j = 0; j < 2; j++)
                for (int k = 0; k < 2; k++)
                    c[i][j][k] = ranvec[perm_x[(x_floor + i) & 255] ^
                                        perm_y[(y_floor + j) & 255] ^
                                        perm_z[(z_floor + k) & 255]];

        double grey_sum = 0;
        for (int i = 0; i < 2; i++)
            for (int j = 0; j < 2; j++)
                for (int k = 0; k < 2; k++) {
                    Vec3 weight_vec(uu - i, vv - j, ww - k);
                    grey_sum += (i * u + (1 - i) * (1 - u)) *
                                (j * v + (1 - j) * (1 - v)) *
                                (k * w + (1 - k) * (1 - w)) *
                                dot(c[i][j][k], weight_vec);
                }
        grey_sum = (grey_sum + 1.0) / 2;
        return Color(grey_sum, grey_sum, grey_sum);
    }

    Color get_turb_grey_noise_color(const Point3 &p, int depth = 7) const {
        Color accum(0.0, 0.0, 0.0);
        auto temp_p = p;
        auto weight = 1.0;
        while (depth--) {
            accum += weight * (2 * get_smooth_grey_noise_color(temp_p) - Color(1.0, 1.0, 1.0));
            weight *= 0.5;
            temp_p *= 2;
        }
        return accum.x() > 0.0 ? accum : -accum;
    }
};

#endif