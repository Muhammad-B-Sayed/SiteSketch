#include "DxfWriter.h"

#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>

namespace sitesketch {
namespace {

std::string layerForObstacle(const Obstacle& obstacle) {
    if (obstacle.type == "access") {
        return "ACCESS";
    }
    if (obstacle.type == "restricted") {
        return "RESTRICTED";
    }
    return "OBSTACLE";
}

void ensureParentDirectory(const std::string& path) {
    const auto parent = std::filesystem::path(path).parent_path();
    if (!parent.empty()) {
        std::filesystem::create_directories(parent);
    }
}

void writeLine(
    std::ostream& out,
    const std::string& layer,
    double x1,
    double y1,
    double x2,
    double y2) {
    out << "0\nLINE\n";
    out << "8\n" << layer << "\n";
    out << "10\n" << x1 << "\n20\n" << y1 << "\n30\n0\n";
    out << "11\n" << x2 << "\n21\n" << y2 << "\n31\n0\n";
}

void writeRect(std::ostream& out, const std::string& layer, const Rect& rect) {
    const double x1 = rect.x;
    const double y1 = rect.y;
    const double x2 = rect.x + rect.width;
    const double y2 = rect.y + rect.height;

    writeLine(out, layer, x1, y1, x2, y1);
    writeLine(out, layer, x2, y1, x2, y2);
    writeLine(out, layer, x2, y2, x1, y2);
    writeLine(out, layer, x1, y2, x1, y1);
}

void writeText(std::ostream& out, const std::string& text, const Point& point, double height) {
    out << "0\nTEXT\n";
    out << "8\nTEXT\n";
    out << "10\n" << point.x << "\n20\n" << point.y << "\n30\n0\n";
    out << "40\n" << height << "\n";
    out << "1\n" << text << "\n";
}

}  // namespace

void DxfWriter::write(const std::string& path, const Site& site, const LayoutResult& result) {
    ensureParentDirectory(path);

    std::ofstream out(path);
    if (!out) {
        throw std::runtime_error("Unable to write DXF file: " + path);
    }

    out << "0\nSECTION\n";
    out << "2\nENTITIES\n";

    writeRect(out, "SITE", Rect{0.0, 0.0, site.width, site.height});

    for (const auto& obstacle : result.obstacles) {
        writeRect(out, layerForObstacle(obstacle), obstacle.bounds);
        writeText(out, obstacle.id, center(obstacle.bounds), 0.8);
    }

    for (const auto& placement : result.placements) {
        writeRect(out, "PANEL", placement.bounds);
    }

    out << "0\nENDSEC\n";
    out << "0\nEOF\n";
}

}  // namespace sitesketch

