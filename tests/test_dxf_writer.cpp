#include "DxfWriter.h"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <sstream>

using namespace sitesketch;

int main() {
    const auto path = std::filesystem::temp_directory_path() / "sitesketch_test_layout.dxf";

    LayoutResult result;
    result.placements.push_back(Placement{"panel_1", Rect{1.0, 1.0, 2.0, 1.0}, "panel"});
    result.obstacles.push_back(Obstacle{"road", Rect{0.0, 3.0, 5.0, 1.0}, "access"});

    DxfWriter::write(path.string(), Site{10.0, 8.0, 1.0}, result);

    std::ifstream in(path);
    std::stringstream buffer;
    buffer << in.rdbuf();
    const std::string contents = buffer.str();

    assert(contents.find("SECTION") != std::string::npos);
    assert(contents.find("ENTITIES") != std::string::npos);
    assert(contents.find("LINE") != std::string::npos);
    assert(contents.find("ENDSEC") != std::string::npos);
    assert(contents.find("EOF") != std::string::npos);

    std::filesystem::remove(path);
    return 0;
}

