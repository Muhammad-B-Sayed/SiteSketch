#pragma once

#include "Site.h"

#include <vector>

namespace sitesketch {

class LayoutEngine {
public:
    LayoutResult generate(
        const Site& site,
        const PanelSpec& panel,
        const std::vector<Obstacle>& obstacles) const;
};

}  // namespace sitesketch

