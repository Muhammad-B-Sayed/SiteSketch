#include "CsvParser.h"
#include "DxfWriter.h"
#include "LayoutEngine.h"
#include "ReportWriter.h"

#include <exception>
#include <iostream>
#include <string>
#include <vector>

namespace {

struct CliOptions {
    std::string sitePath;
    std::string obstaclesPath;
    std::string dxfPath;
    std::string reportPath;
    std::string warningsPath;
    bool help = false;
};

void printHelp(std::ostream& out) {
    out << "SiteSketch: Engineering Layout Automation Tool\n\n";
    out << "Usage:\n";
    out << "  sitesketch --site <site.csv> --obstacles <obstacles.csv> --out <layout.dxf> --report <summary.csv> --warnings <warnings.txt>\n\n";
    out << "Options:\n";
    out << "  --site        Path to site configuration CSV\n";
    out << "  --obstacles   Path to obstacles CSV\n";
    out << "  --out         Path to output DXF file\n";
    out << "  --report      Path to summary CSV file\n";
    out << "  --warnings    Path to warnings TXT file\n";
    out << "  --help        Show help message\n";
}

CliOptions parseArgs(int argc, char* argv[]) {
    CliOptions options;

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--help") {
            options.help = true;
            return options;
        }

        auto requireValue = [&](const std::string& name) -> std::string {
            if (i + 1 >= argc) {
                throw std::runtime_error("Missing value for " + name);
            }
            return argv[++i];
        };

        if (arg == "--site") {
            options.sitePath = requireValue(arg);
        } else if (arg == "--obstacles") {
            options.obstaclesPath = requireValue(arg);
        } else if (arg == "--out") {
            options.dxfPath = requireValue(arg);
        } else if (arg == "--report") {
            options.reportPath = requireValue(arg);
        } else if (arg == "--warnings") {
            options.warningsPath = requireValue(arg);
        } else {
            throw std::runtime_error("Unknown option: " + arg);
        }
    }

    if (options.sitePath.empty() || options.dxfPath.empty() ||
        options.reportPath.empty() || options.warningsPath.empty()) {
        throw std::runtime_error("Missing required arguments. Use --help for usage.");
    }

    return options;
}

}  // namespace

int main(int argc, char* argv[]) {
    try {
        const CliOptions options = parseArgs(argc, argv);
        if (options.help) {
            printHelp(std::cout);
            return 0;
        }

        std::vector<std::string> warnings;
        const auto config = sitesketch::CsvParser::parseSiteConfig(options.sitePath, warnings);

        std::vector<sitesketch::Obstacle> obstacles;
        if (!options.obstaclesPath.empty()) {
            obstacles = sitesketch::CsvParser::parseObstacles(options.obstaclesPath, config.site, warnings);
        }

        sitesketch::LayoutEngine engine;
        auto result = engine.generate(config.site, config.panel, obstacles);
        result.warnings.insert(result.warnings.begin(), warnings.begin(), warnings.end());

        sitesketch::DxfWriter::write(options.dxfPath, config.site, result);
        sitesketch::ReportWriter::writeSummary(options.reportPath, config.site, result);
        sitesketch::ReportWriter::writeWarnings(options.warningsPath, result.warnings);

        std::cout << "SiteSketch layout generation complete.\n\n";
        std::cout << "Panels placed: " << result.placements.size() << "\n";
        std::cout << "Estimated capacity: " << result.estimatedCapacityKw << " kW\n";
        std::cout << "Rows used: " << result.rowsUsed << "\n";
        std::cout << "Obstacles avoided: " << result.obstacles.size() << "\n";
        std::cout << "Warnings: " << result.warnings.size() << "\n\n";
        std::cout << "DXF written to: " << options.dxfPath << "\n";
        std::cout << "CSV report written to: " << options.reportPath << "\n";
        std::cout << "Warnings written to: " << options.warningsPath << "\n";
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "ERROR: " << ex.what() << "\n";
        return 1;
    }
}

