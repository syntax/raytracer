#ifndef INTERVAL_H
#define INTERVAL_H

#include <limits>

class interval {
    public:
        double _min;
        double _max;

        interval()
            : _min(std::numeric_limits<double>::infinity()),
            _max(-std::numeric_limits<double>::infinity())
        {}

        interval(double minVal, double maxVal)
            : _min(minVal),
            _max(maxVal)
        {}

        double size() const {
            return _max - _min;
        }

        bool contains(double x) const {
            return _min <= x && x <= _max;
        }

        double min() const { return _min; }
        double max() const { return _max; }

        bool surrounds(double x) const {
            return _min < x && x < _max;
        }

        double clamp(double x) const {
            if (x < _min) return _min;
            if (x > _max) return _max;
            return x;
        }

        static const interval empty, universe;
};

const interval interval::empty = interval(
    std::numeric_limits<double>::infinity(),
    -std::numeric_limits<double>::infinity()
);
const interval interval::universe = interval(
    -std::numeric_limits<double>::infinity(),
     std::numeric_limits<double>::infinity()
);

#endif // INTERVAL_H