#pragma once

#include "Geometry.h"

#include <string>
#include <vector>

namespace sitesketch {

struct Site {
    double width = 0.0;
    double height = 0.0;
    double setback = 0.0;
};

struct PanelSpec {
    double width = 0.0;
    double height = 0.0;
    double watts = 0.0;
    double rowSpacing = 0.0;
    double colSpacing = 0.0;
};

struct Obstacle {
    std::string id;
    Rect bounds;
    std::string type;
};

struct Placement {
    std::string id;
    Rect bounds;
    std::string type;
};

struct LayoutResult {
    std::vector<Placement> placements;
    std::vector<Obstacle> obstacles;
    std::vector<std::string> warnings;
    int rowsUsed = 0;
    double estimatedCapacityKw = 0.0;
};

struct SiteConfig {
    Site site;
    PanelSpec panel;
};

}  // namespace sitesketch

