#!/bin/bash
set -euo pipefail
cd "$(dirname "$0")"
export PATH="/opt/homebrew/bin:/usr/local/bin:$PATH"
base="$PWD"
build="$base/build-coverage"
cmake -S . -B "$build" -G "Unix Makefiles" -DCMAKE_CXX_COMPILER="$(xcrun --find clang++)" \
    -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON -DOTIS_COVERAGE=ON
cmake --build "$build" --parallel
profiles="$(mktemp -d "$build/profiles.XXXXXX")"
trap 'rm -rf "$profiles"' EXIT
export LLVM_PROFILE_FILE="$profiles/%m-%p.profraw"
ctest --test-dir "$build" -C Debug --output-on-failure --no-tests=error \
    --output-junit "$base/test_report.xml" --output-log "$base/test_report.txt"
xcrun llvm-profdata merge -sparse "$profiles"/*.profraw -o "$build/unit.profdata"
xcrun llvm-cov report "$build/test/ii02920_task02_tests" -object "$build/src/ii02920_task02" \
    -instr-profile="$build/unit.profdata" -sources "$base/src" "$(cd ../task_01/src && pwd)/InputSignals.h" | tee "$base/coverage.txt"
echo "Отчёты: test_report.txt, test_report.xml, coverage.txt"
