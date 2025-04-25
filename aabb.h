// aabb.h
#ifndef AABB_H
#define AABB_H


// axis aligned bounding box for using within kd treee

#include "ray.h"
#include <algorithm>

class aabb {
public:
    point3 _min, _max;
    aabb() {}
    aabb(const point3& a, const point3& b) : _min(a), _max(b) {}

    point3 min() const { return _min; }
    point3 max() const { return _max; }

    // ray-box intersection (slab method)
    bool hit(const ray& r, const interval& t_range) const {
        double tmin = t_range.min();
        double tmax = t_range.max();
        for (int a = 0; a < 3; ++a) {
            double invD = 1.0 / r.direction()[a];
            double t0   = (_min[a] - r.origin()[a]) * invD;
            double t1   = (_max[a] - r.origin()[a]) * invD;
            if (invD < 0.0) std::swap(t0, t1);
            tmin = t0 > tmin ? t0 : tmin;
            tmax = t1 < tmax ? t1 : tmax;
            if (tmax <= tmin)
                return false;
        }
        return true;
    }
};

inline aabb surrounding_box(const aabb& b0, const aabb& b1) {
    point3 small(
      std::min(b0._min.x(), b1._min.x()),
      std::min(b0._min.y(), b1._min.y()),
      std::min(b0._min.z(), b1._min.z())
    );
    point3 big(
      std::max(b0._max.x(), b1._max.x()),
      std::max(b0._max.y(), b1._max.y()),
      std::max(b0._max.z(), b1._max.z())
    );
    return aabb(small, big);
}

#endif
