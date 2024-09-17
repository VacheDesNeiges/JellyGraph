#!/bin/bash

COVERAGE_REPORT=false

while [[ $# -gt 0 ]]; do
    case "$1" in
        --coverage-report)
            COVERAGE_REPORT=true
            shift
            ;;
        *)
            echo "Unknow option : $1"
            exit 1
            ;;
    esac
done

mkdir -p build
cd build

cmake_options="-G Ninja -DCMAKE_INSTALL_PREFIX=${HOME}/libs/jellyGraph"

cmake $cmake_options ..
ninja
ninja install
mv test ..

if [ "$COVERAGE_REPORT" = true ]; then
    cd ..
    ./test
    llvm-profdata-18 merge -sparse default.profraw -o coverage.profdata
    llvm-cov-18 show ./test -instr-profile=coverage.profdata -format=html -output-dir=coverage-report
    rm default.profraw
    rm coverage.profdata
fi
