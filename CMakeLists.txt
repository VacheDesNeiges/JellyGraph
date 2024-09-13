cmake_minimum_required(VERSION 3.22.1)
project (jellyGraph)

SET(CMAKE_CXX_COMPILER "g++")
SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}\
    -Wall \
    -Wextra \
    -pedantic \
    -Wshadow \
    -Wconversion \
    -O3 \
    -march=native"
)

add_library(jellyGraph INTERFACE)

target_include_directories(jellyGraph
    INTERFACE
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/src/GraphImpl
    ${PROJECT_SOURCE_DIR}/src/Utils
    ${PROJECT_SOURCE_DIR}/src/Measures
    ${PROJECT_SOURCE_DIR}/src/Algorithms
    ${PROJECT_SOURCE_DIR}/src/Primitives
)

install(TARGETS jellyGraph)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/src/
    DESTINATION include/jellyGraph
    FILES_MATCHING PATTERN "*.hpp"
)


add_executable(test
    tests/main.cpp
    tests/testGraphPrimitives.cpp
    tests/testGraphMeasures.cpp
    tests/testGraphAlgorithms.cpp
    tests/testGraph.cpp
)

find_package(GTest REQUIRED)

target_link_libraries(test PRIVATE
    gtest
    jellyGraph
)
