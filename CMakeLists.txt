cmake_minimum_required(VERSION 3.15)
project(SwiftCast
    VERSION 0.1.0
    LANGUAGES CXX
    DESCRIPTION "High-performance productivity launcher"
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(SOURCES
    src/main.cpp
)

# Header directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Executable
add_executable(swiftcast ${SOURCES})

# Optional: Enable testing
enable_testing()

# TODO: Add subdirectories for core, ui, extensions, utils as they are developed
