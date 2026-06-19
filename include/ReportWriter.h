#pragma once

#include "Site.h"

#include <string>
#include <vector>

namespace sitesketch {

class ReportWriter {
public:
    static void writeSummary(const std::string& path, const Site& site, const LayoutResult& result);
    static void writeWarnings(const std::string& path, const std::vector<std::string>& warnings);
};

}  // namespace sitesketch

