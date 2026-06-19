#include "ReportWriter.h"

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <stdexcept>

namespace sitesketch {
namespace {

void ensureParentDirectory(const std::string& path) {
    const auto parent = std::filesystem::path(path).parent_path();
    if (!parent.empty()) {
        std::filesystem::create_directories(parent);
    }
}

}  // namespace

void ReportWriter::writeSummary(const std::string& path, const Site& site, const LayoutResult& result) {
    ensureParentDirectory(path);

    std::ofstream out(path);
    if (!out) {
        throw std::runtime_error("Unable to write CSV report: " + path);
    }

    out << std::fixed << std::setprecision(3);
    out << "metric,value\n";
    out << "site_width_m," << site.width << "\n";
    out << "site_height_m," << site.height << "\n";
    out << "setback_m," << site.setback << "\n";
    out << "panels_placed," << result.placements.size() << "\n";
    out << "estimated_capacity_kw," << result.estimatedCapacityKw << "\n";
    out << "rows_used," << result.rowsUsed << "\n";
    out << "obstacles_count," << result.obstacles.size() << "\n";
    out << "warnings_count," << result.warnings.size() << "\n";
}

void ReportWriter::writeWarnings(const std::string& path, const std::vector<std::string>& warnings) {
    ensureParentDirectory(path);

    std::ofstream out(path);
    if (!out) {
        throw std::runtime_error("Unable to write warnings file: " + path);
    }

    if (warnings.empty()) {
        out << "No warnings.\n";
        return;
    }

    for (const auto& warning : warnings) {
        out << warning << "\n";
    }
}

}  // namespace sitesketch

