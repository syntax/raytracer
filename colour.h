#ifndef COLOUR_H
#define COLOUR_H

#include "vec3.h"

using colour = vec3;


void write_colour(std::ostream& out, const colour& pixel_colour) {
    // x corresponds to red, y to green, z to blue
    out << static_cast<int>(255.999 * pixel_colour.x()) << ' '
        << static_cast<int>(255.999 * pixel_colour.y()) << ' '
        << static_cast<int>(255.999 * pixel_colour.z()) << '\n';
}

#endif