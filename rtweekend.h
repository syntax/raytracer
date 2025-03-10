#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include <cmath>
#include <iostream>
#include <limits>
#include <memory>


// c++ Std Usings

using std::make_shared;
using std::shared_ptr;

// consts

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// util Functions

inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}

// common Headers

#include "colour.h"
#include "ray.h"
#include "vec3.h"

#endif