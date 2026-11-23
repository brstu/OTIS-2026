#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
if command -v gcovr >/dev/null 2>&1; then
  gcovr -r . --filter 'src/' --exclude 'src/main.cpp' --txt --html-details coverage.html
  echo "Coverage report: coverage.html"
else
  echo "gcovr is not installed; install with: python3 -m pip install gcovr"
fi
