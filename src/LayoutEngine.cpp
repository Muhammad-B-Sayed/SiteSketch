#include "LayoutEngine.h"

#include "Geometry.h"

#include <cmath>
#include <string>

namespace sitesketch {
namespace {

bool conflictsWithObstacle(const Rect& panel, const std::vector<Obstacle>& obstacles) {
    for (const auto& obstacle : obstacles) {
        if (intersects(panel, obstacle.bounds)) {
            return true;
        }
    }
    return false;
}

bool conflictsWithPlacement(const Rect& panel, const std::vector<Placement>& placements) {
    for (const auto& placement : placements) {
        if (intersects(panel, placement.bounds)) {
            return true;
        }
    }
    return false;
}

}  // namespace

LayoutResult LayoutEngine::generate(
    const Site& site,
    const PanelSpec& panel,
    const std::vector<Obstacle>& obstacles) const {
    LayoutResult result;
    result.obstacles = obstacles;

    const Rect usableArea{
        site.setback,
        site.setback,
        site.width - 2.0 * site.setback,
        site.height - 2.0 * site.setback};

    if (!isValid(usableArea)) {
        result.warnings.push_back("WARNING: No panels could be placed because usable area is too small.");
        return result;
    }

    const double xStep = panel.width + panel.colSpacing;
    const double yStep = panel.height + panel.rowSpacing;
    if (xStep <= 0.0 || yStep <= 0.0) {
        result.warnings.push_back("WARNING: No panels could be placed because spacing produced an invalid grid.");
        return result;
    }

    int panelNumber = 0;
    const double epsilon = 1e-9;

    for (double y = usableArea.y; y + panel.height <= usableArea.y + usableArea.height + epsilon; y += yStep) {
        bool rowHasPanel = false;

        for (double x = usableArea.x; x + panel.width <= usableArea.x + usableArea.width + epsilon; x += xStep) {
            const Rect candidate{x, y, panel.width, panel.height};

            if (!contains(usableArea, candidate)) {
                continue;
            }
            if (conflictsWithObstacle(candidate, obstacles)) {
                continue;
            }
            if (conflictsWithPlacement(candidate, result.placements)) {
                continue;
            }

            ++panelNumber;
            result.placements.push_back(
                Placement{"panel_" + std::to_string(panelNumber), candidate, "panel"});
            rowHasPanel = true;
        }

        if (rowHasPanel) {
            ++result.rowsUsed;
        }
    }

    result.estimatedCapacityKw = static_cast<double>(result.placements.size()) * panel.watts / 1000.0;
    if (result.placements.empty()) {
        result.warnings.push_back("WARNING: No panels could be placed because usable area is too small or blocked.");
    }

    return result;
}

}  // namespace sitesketch

