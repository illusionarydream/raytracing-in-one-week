#ifndef COLOR_H
#define COLOR_H
#include "vec3.h"
#define color vec3
void write_color(std::ostream &out, color pixel_color) {
    out << int(255.999 * pixel_color.x()) << ' '
        << int(255.999 * pixel_color.y()) << ' '
        << int(255.999 * pixel_color.z()) << '\n';
    return;
}

#endif