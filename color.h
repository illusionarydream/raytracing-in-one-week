#ifndef COLOR_H
#define COLOR_H
#include "vec3.h"
#include "math_materials.h"
#include "interval.h"
#define color vec3
double from_linear_to_gamma(double linear_col) {
    return pow(linear_col, 0.4545);
}

void write_color(std::ostream &out, color pixel_color) {
    interval rgb_range(0.0, 1.0);

    auto R = rgb_range.clamp(pixel_color.x());
    auto G = rgb_range.clamp(pixel_color.y());
    auto B = rgb_range.clamp(pixel_color.z());

    // *gamma 矫正
    R = from_linear_to_gamma(R);
    G = from_linear_to_gamma(G);
    B = from_linear_to_gamma(B);

    out << int(255.999 * R) << ' '
        << int(255.999 * G) << ' '
        << int(255.999 * B) << '\n';
    return;
}

#endif