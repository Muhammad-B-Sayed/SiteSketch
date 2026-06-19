#include "ReportWriter.h"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <sstream>

using namespace sitesketch;

int main() {
    const auto summaryPath = std::filesystem::temp_directory_path() / "sitesketch_summary.csv";
    const auto warningsPath = std::filesystem::temp_directory_path() / "sitesketch_warnings.txt";

    LayoutResult result;
    result.placements.push_back(Placement{"panel_1", Rect{1.0, 1.0, 1.0, 1.0}, "panel"});
    result.obstacles.push_back(Obstacle{"pad", Rect{2.0, 2.0, 1.0, 1.0}, "equipment"});
    result.rowsUsed = 1;
    result.estimatedCapacityKw = 0.55;
    result.warnings.push_back("WARNING: sample");

    ReportWriter::writeSummary(summaryPath.string(), Site{10.0, 8.0, 1.0}, result);
    ReportWriter::writeWarnings(warningsPath.string(), result.warnings);

    std::ifstream summaryIn(summaryPath);
    std::stringstream summaryBuffer;
    summaryBuffer << summaryIn.rdbuf();
    assert(summaryBuffer.str().find("panels_placed,1") != std::string::npos);
    assert(summaryBuffer.str().find("estimated_capacity_kw,0.550") != std::string::npos);

    std::ifstream warningsIn(warningsPath);
    std::stringstream warningsBuffer;
    warningsBuffer << warningsIn.rdbuf();
    assert(warningsBuffer.str().find("WARNING: sample") != std::string::npos);

    std::filesystem::remove(summaryPath);
    std::filesystem::remove(warningsPath);
    return 0;
}

