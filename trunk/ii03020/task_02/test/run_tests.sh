#!/bin/bash
mkdir -p ../build
cd ../build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON
cmake --build .
ctest --output-on-failure
if command -v gcovr &> /dev/null; then
    mkdir -p ../report
    gcovr -r .. --filter ".*/src/.*" --html --html-details -o ../report/coverage.html
fi
cd ../test