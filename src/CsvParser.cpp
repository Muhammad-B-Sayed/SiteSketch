#include "CsvParser.h"

#include "Geometry.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace sitesketch {
namespace {

std::string trim(const std::string& value) {
    auto begin = value.begin();
    while (begin != value.end() && std::isspace(static_cast<unsigned char>(*begin))) {
        ++begin;
    }

    auto end = value.end();
    while (end != begin && std::isspace(static_cast<unsigned char>(*(end - 1)))) {
        --end;
    }

    return std::string(begin, end);
}

std::vector<std::string> splitCsvLine(const std::string& line) {
    std::vector<std::string> fields;
    std::stringstream stream(line);
    std::string field;

    while (std::getline(stream, field, ',')) {
        fields.push_back(trim(field));
    }

    if (!line.empty() && line.back() == ',') {
        fields.emplace_back();
    }

    return fields;
}

std::unordered_map<std::string, std::size_t> makeHeaderMap(const std::vector<std::string>& headers) {
    std::unordered_map<std::string, std::size_t> map;
    for (std::size_t i = 0; i < headers.size(); ++i) {
        map[headers[i]] = i;
    }
    return map;
}

void requireColumns(
    const std::unordered_map<std::string, std::size_t>& headers,
    const std::vector<std::string>& required,
    const std::string& context) {
    std::vector<std::string> missing;
    for (const auto& column : required) {
        if (headers.find(column) == headers.end()) {
            missing.push_back(column);
        }
    }

    if (!missing.empty()) {
        std::ostringstream message;
        message << "Missing required " << context << " column(s): ";
        for (std::size_t i = 0; i < missing.size(); ++i) {
            if (i > 0) {
                message << ", ";
            }
            message << missing[i];
        }
        throw std::runtime_error(message.str());
    }
}

double parseDouble(const std::string& value, const std::string& column) {
    try {
        std::size_t parsed = 0;
        const double number = std::stod(value, &parsed);
        if (trim(value.substr(parsed)).empty()) {
            return number;
        }
    } catch (const std::exception&) {
    }

    throw std::runtime_error("Invalid numeric value for '" + column + "': '" + value + "'");
}

std::string getField(
    const std::vector<std::string>& row,
    const std::unordered_map<std::string, std::size_t>& headers,
    const std::string& column) {
    const auto it = headers.find(column);
    if (it == headers.end() || it->second >= row.size()) {
        throw std::runtime_error("Missing value for '" + column + "'");
    }
    return row[it->second];
}

double getDouble(
    const std::vector<std::string>& row,
    const std::unordered_map<std::string, std::size_t>& headers,
    const std::string& column) {
    return parseDouble(getField(row, headers, column), column);
}

bool isBlankLine(const std::string& line) {
    return trim(line).empty();
}

}  // namespace

SiteConfig CsvParser::parseSiteConfig(const std::string& path, std::vector<std::string>& warnings) {
    (void)warnings;

    std::ifstream input(path);
    if (!input) {
        throw std::runtime_error("Missing site file: " + path);
    }

    std::string line;
    if (!std::getline(input, line)) {
        throw std::runtime_error("Site CSV is empty: " + path);
    }

    const auto headers = makeHeaderMap(splitCsvLine(line));
    requireColumns(
        headers,
        {"site_width_m", "site_height_m", "setback_m", "panel_width_m", "panel_height_m",
         "panel_watts", "row_spacing_m", "col_spacing_m"},
        "site");

    int lineNumber = 1;
    while (std::getline(input, line)) {
        ++lineNumber;
        if (isBlankLine(line)) {
            continue;
        }

        const auto row = splitCsvLine(line);
        try {
            SiteConfig config;
            config.site.width = getDouble(row, headers, "site_width_m");
            config.site.height = getDouble(row, headers, "site_height_m");
            config.site.setback = getDouble(row, headers, "setback_m");
            config.panel.width = getDouble(row, headers, "panel_width_m");
            config.panel.height = getDouble(row, headers, "panel_height_m");
            config.panel.watts = getDouble(row, headers, "panel_watts");
            config.panel.rowSpacing = getDouble(row, headers, "row_spacing_m");
            config.panel.colSpacing = getDouble(row, headers, "col_spacing_m");

            if (config.site.width <= 0.0 || config.site.height <= 0.0 || config.site.setback < 0.0) {
                throw std::runtime_error("Invalid site dimensions or setback in site CSV");
            }
            if (config.panel.width <= 0.0 || config.panel.height <= 0.0 || config.panel.watts <= 0.0) {
                throw std::runtime_error("Invalid panel dimensions or wattage in site CSV");
            }
            if (config.panel.rowSpacing < 0.0 || config.panel.colSpacing < 0.0) {
                throw std::runtime_error("Panel spacing values cannot be negative");
            }

            return config;
        } catch (const std::exception& ex) {
            throw std::runtime_error(
                "Unable to parse site row " + std::to_string(lineNumber) + ": " + ex.what());
        }
    }

    throw std::runtime_error("Site CSV has no data rows: " + path);
}

std::vector<Obstacle> CsvParser::parseObstacles(
    const std::string& path,
    const Site& site,
    std::vector<std::string>& warnings) {
    std::ifstream input(path);
    if (!input) {
        warnings.push_back("WARNING: Obstacle file could not be opened; continuing with no obstacles: " + path);
        return {};
    }

    std::string line;
    if (!std::getline(input, line)) {
        return {};
    }

    const auto headers = makeHeaderMap(splitCsvLine(line));
    try {
        requireColumns(headers, {"id", "x_m", "y_m", "width_m", "height_m", "type"}, "obstacle");
    } catch (const std::exception& ex) {
        warnings.push_back(std::string("WARNING: ") + ex.what() + "; continuing with no obstacles.");
        return {};
    }

    const Rect siteBoundary{0.0, 0.0, site.width, site.height};
    std::vector<Obstacle> obstacles;
    int lineNumber = 1;

    while (std::getline(input, line)) {
        ++lineNumber;
        if (isBlankLine(line)) {
            continue;
        }

        const auto row = splitCsvLine(line);
        try {
            Obstacle obstacle;
            obstacle.id = getField(row, headers, "id");
            obstacle.bounds.x = getDouble(row, headers, "x_m");
            obstacle.bounds.y = getDouble(row, headers, "y_m");
            obstacle.bounds.width = getDouble(row, headers, "width_m");
            obstacle.bounds.height = getDouble(row, headers, "height_m");
            obstacle.type = getField(row, headers, "type");

            if (obstacle.id.empty()) {
                throw std::runtime_error("Obstacle id cannot be empty");
            }
            if (obstacle.type.empty()) {
                obstacle.type = "obstacle";
            }
            if (!isValid(obstacle.bounds)) {
                warnings.push_back(
                    "WARNING: Obstacle " + obstacle.id + " has non-positive dimensions and was skipped.");
                continue;
            }
            if (!contains(siteBoundary, obstacle.bounds)) {
                warnings.push_back(
                    "WARNING: Obstacle " + obstacle.id + " extends outside site boundary.");
            }

            obstacles.push_back(obstacle);
        } catch (const std::exception& ex) {
            warnings.push_back(
                "WARNING: Skipped malformed obstacle row " + std::to_string(lineNumber) + ": " + ex.what());
        }
    }

    return obstacles;
}

}  // namespace sitesketch

