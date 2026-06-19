#pragma once

namespace sitesketch {

struct Point {
    double x = 0.0;
    double y = 0.0;
};

struct Rect {
    double x = 0.0;
    double y = 0.0;
    double width = 0.0;
    double height = 0.0;
};

bool isValid(const Rect& r);
bool intersects(const Rect& a, const Rect& b);
bool contains(const Rect& outer, const Rect& inner);
double area(const Rect& r);
Point center(const Rect& r);

}  // namespace sitesketch

