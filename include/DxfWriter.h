#pragma once

#include "Site.h"

#include <string>

namespace sitesketch {

class DxfWriter {
public:
    static void write(const std::string& path, const Site& site, const LayoutResult& result);
};

}  // namespace sitesketch

