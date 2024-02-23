#ifndef COLOR_H
#define COLOR_H
#include "vec3.h"
#include "math_materials.h"
#include "interval.h"
#define color vec3
void write_color(std::ostream &out, color pixel_color) {
    interval rgb_range(0.0, 1.0);
    out << int(255.999 * rgb_range.clamp(pixel_color.x())) << ' '
        << int(255.999 * rgb_range.clamp(pixel_color.y())) << ' '
        << int(255.999 * rgb_range.clamp(pixel_color.z())) << '\n';
    return;
}

#endif