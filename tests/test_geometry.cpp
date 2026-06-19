#include "Geometry.h"

#include <cassert>
#include <cmath>

using namespace sitesketch;

int main() {
    const Rect a{0.0, 0.0, 2.0, 2.0};
    const Rect b{1.0, 1.0, 2.0, 2.0};
    const Rect touching{2.0, 0.0, 1.0, 1.0};
    const Rect outer{0.0, 0.0, 10.0, 10.0};
    const Rect inner{2.0, 2.0, 3.0, 4.0};

    assert(intersects(a, b));
    assert(!intersects(a, touching));
    assert(contains(outer, inner));
    assert(!contains(inner, outer));
    assert(area(inner) == 12.0);
    assert(area(Rect{0.0, 0.0, 0.0, 2.0}) == 0.0);

    const Point c = center(inner);
    assert(std::abs(c.x - 3.5) < 1e-9);
    assert(std::abs(c.y - 4.0) < 1e-9);

    return 0;
}

