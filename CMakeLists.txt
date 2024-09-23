cmake_minimum_required(VERSION 3.22.1)
project (jellyGraph)

SET(CMAKE_CXX_COMPILER "clang++")
SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(COVERAGE_REPORT)
    message(STATUS "Coverage report enabled")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-instr-generate -fcoverage-mapping")
endif()

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}\
    -Wall \
    -Wextra \
    -pedantic \
    -Werror \
    -Wshadow \
    -Wconversion \
    -Wunused-value \
    -Wold-style-cast \
    -Wnon-virtual-dtor \
    -O0 \
    -g \
    -flto \
    -march=native"
)

add_library(jellyGraph INTERFACE)

target_include_directories(jellyGraph
    INTERFACE
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/src/Algorithms
    ${PROJECT_SOURCE_DIR}/src/Exceptions
    ${PROJECT_SOURCE_DIR}/src/GraphImplementations
    ${PROJECT_SOURCE_DIR}/src/IO
    ${PROJECT_SOURCE_DIR}/src/Measures
    ${PROJECT_SOURCE_DIR}/src/Primitives
    ${PROJECT_SOURCE_DIR}/src/Utils
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
    tests/testGraphs.cpp
    tests/testGraphIO.cpp
)

find_package(GTest REQUIRED)

target_link_libraries(test PRIVATE
    GTest::GTest
    GTest::Main
    jellyGraph
)

