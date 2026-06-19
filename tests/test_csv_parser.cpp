#include "CsvParser.h"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <vector>

using namespace sitesketch;

namespace {

std::filesystem::path tempPath(const std::string& name) {
    return std::filesystem::temp_directory_path() / name;
}

}  // namespace

int main() {
    const auto sitePath = tempPath("sitesketch_valid_site.csv");
    {
        std::ofstream out(sitePath);
        out << "site_width_m,site_height_m,setback_m,panel_width_m,panel_height_m,panel_watts,row_spacing_m,col_spacing_m\n";
        out << "10,8,1,1,2,500,0.5,0.25\n";
    }

    std::vector<std::string> warnings;
    const SiteConfig config = CsvParser::parseSiteConfig(sitePath.string(), warnings);
    assert(config.site.width == 10.0);
    assert(config.panel.watts == 500.0);

    const auto obstaclePath = tempPath("sitesketch_obstacles_malformed.csv");
    {
        std::ofstream out(obstaclePath);
        out << "id,x_m,y_m,width_m,height_m,type\n";
        out << "valid,2,2,1,1,equipment\n";
        out << "bad,not_a_number,2,1,1,equipment\n";
        out << "zero,3,3,0,1,equipment\n";
    }

    const auto obstacles = CsvParser::parseObstacles(obstaclePath.string(), config.site, warnings);
    assert(obstacles.size() == 1);
    assert(warnings.size() == 2);

    std::filesystem::remove(sitePath);
    std::filesystem::remove(obstaclePath);
    return 0;
}

