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
    -Wconversion"
)

add_library(jellyGraph SHARED
    src/GraphImpl/ListGraph.cpp
    src/GraphImpl/MatrixGraph.cpp
    src/Measures/GraphMeasures.cpp
)

target_include_directories(jellyGraph
    PUBLIC ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/src/GraphImpl
    ${PROJECT_SOURCE_DIR}/src/Utils
    ${PROJECT_SOURCE_DIR}/src/Measures
    ${PROJECT_SOURCE_DIR}/src/Algorithms

)

install(TARGETS jellyGraph
    LIBRARY DESTINATION lib
)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/src/
    DESTINATION include/jellyGraph
    FILES_MATCHING PATTERN "*.hpp"
)


add_executable(test
    tests/main.cpp
    tests/testGraphPrimitives.cpp
    tests/testGraphMeasures.cpp
)

find_package(GTest REQUIRED)

target_link_libraries(test PRIVATE
    gtest
    jellyGraph
)
