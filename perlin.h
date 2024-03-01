#ifndef PERLIN_H
#define PERLIN_H
#include "vec3.h"
#include "math_materials.h"
#include "color.h"
#include "algorithm"
class Perlin {
   private:
    static const int point_count = 256;
    double *ranfloat;
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
        ranfloat = new double[point_count];
        for (int i = 0; i < 256; i++)
            ranfloat[i] = random_double();

        perm_x = generate_permute_list();
        perm_y = generate_permute_list();
        perm_z = generate_permute_list();
    }
    color get_grey_noise_color(const point3 &p) const {
        auto i = int(p.x() * 4) & 255;
        auto j = int(p.y() * 4) & 255;
        auto k = int(p.z() * 4) & 255;
        auto grey = ranfloat[i ^ j ^ k];
        return color(grey, grey, grey);
    }
    color get_smooth_grey_noise_color(const point3 &p) const {
        int x_floor = int(std::floor(p.x()));
        int y_floor = int(std::floor(p.y()));
        int z_floor = int(std::floor(p.z()));

        double u = p.x() - x_floor;
        double v = p.y() - y_floor;
        double w = p.z() - z_floor;

        u = u * u * (3 - 2 * u);
        v = v * v * (3 - 2 * v);
        w = w * w * (3 - 2 * w);

        double c[2][2][2];
        for (int i = 0; i < 2; i++)
            for (int j = 0; j < 2; j++)
                for (int k = 0; k < 2; k++)
                    c[i][j][k] = ranfloat[perm_x[(x_floor + i) & 255] ^
                                          perm_y[(y_floor + j) & 255] ^
                                          perm_z[(z_floor + k) & 255]];

        double grey_sum = 0;
        for (int i = 0; i < 2; i++)
            for (int j = 0; j < 2; j++)
                for (int k = 0; k < 2; k++)
                    grey_sum += (i * u + (1 - i) * (1 - u)) *
                                (j * v + (1 - j) * (1 - v)) *
                                (k * w + (1 - k) * (1 - w)) * c[i][j][k];
        return color(grey_sum, grey_sum, grey_sum);
    }
};

#endif