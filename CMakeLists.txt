cmake_minimum_required(VERSION 3.16)

project(SiteSketch VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(sitesketch_core
    src/CsvParser.cpp
    src/DxfWriter.cpp
    src/Geometry.cpp
    src/LayoutEngine.cpp
    src/ReportWriter.cpp
    src/Site.cpp
)

target_include_directories(sitesketch_core PUBLIC include)

add_executable(sitesketch src/main.cpp)
target_link_libraries(sitesketch PRIVATE sitesketch_core)

enable_testing()
add_subdirectory(tests)

