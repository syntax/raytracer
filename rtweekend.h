#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include <cmath>
#include <iostream>
#include <random>
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

inline double random_double() {
    // Returns a random real in [0,1)
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);
}

inline double random_double(double min, double max) {
    // Returns a random real within [min,max)
    return min + (max - min) * random_double();
}

// common Headers

#include "colour.h"
#include "ray.h"
#include "interval.h"
#include "vec3.h"

#endif