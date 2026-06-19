#pragma once

#include "Site.h"

#include <string>
#include <vector>

namespace sitesketch {

class CsvParser {
public:
    static SiteConfig parseSiteConfig(const std::string& path, std::vector<std::string>& warnings);
    static std::vector<Obstacle> parseObstacles(
        const std::string& path,
        const Site& site,
        std::vector<std::string>& warnings);
};

}  // namespace sitesketch

