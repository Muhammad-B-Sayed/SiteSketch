#include "LayoutEngine.h"

#include "Geometry.h"

#include <cassert>

using namespace sitesketch;

int main() {
    LayoutEngine engine;

    const Site site{5.0, 5.0, 0.5};
    const PanelSpec panel{1.0, 1.0, 400.0, 0.0, 0.0};
    const auto noObstacleResult = engine.generate(site, panel, {});
    assert(noObstacleResult.placements.size() == 16);
    assert(noObstacleResult.rowsUsed == 4);
    assert(noObstacleResult.estimatedCapacityKw == 6.4);

    const Obstacle obstacle{"block", Rect{1.5, 1.5, 1.0, 1.0}, "restricted"};
    const auto obstacleResult = engine.generate(site, panel, {obstacle});
    assert(obstacleResult.placements.size() < noObstacleResult.placements.size());
    for (const auto& placement : obstacleResult.placements) {
        assert(!intersects(placement.bounds, obstacle.bounds));
    }

    const Site tiny{1.0, 1.0, 0.2};
    const PanelSpec largePanel{2.0, 2.0, 400.0, 0.0, 0.0};
    const auto tinyResult = engine.generate(tiny, largePanel, {});
    assert(tinyResult.placements.empty());
    assert(!tinyResult.warnings.empty());

    return 0;
}

