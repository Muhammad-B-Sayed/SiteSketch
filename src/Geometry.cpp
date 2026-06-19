#include "Geometry.h"

#include <algorithm>

namespace sitesketch {

bool isValid(const Rect& r) {
    return r.width > 0.0 && r.height > 0.0;
}

bool intersects(const Rect& a, const Rect& b) {
    if (!isValid(a) || !isValid(b)) {
        return false;
    }

    const double aRight = a.x + a.width;
    const double bRight = b.x + b.width;
    const double aTop = a.y + a.height;
    const double bTop = b.y + b.height;

    return a.x < bRight && aRight > b.x && a.y < bTop && aTop > b.y;
}

bool contains(const Rect& outer, const Rect& inner) {
    if (!isValid(outer) || !isValid(inner)) {
        return false;
    }

    return inner.x >= outer.x &&
           inner.y >= outer.y &&
           inner.x + inner.width <= outer.x + outer.width &&
           inner.y + inner.height <= outer.y + outer.height;
}

double area(const Rect& r) {
    if (!isValid(r)) {
        return 0.0;
    }
    return r.width * r.height;
}

Point center(const Rect& r) {
    return Point{r.x + r.width / 2.0, r.y + r.height / 2.0};
}

}  // namespace sitesketch

