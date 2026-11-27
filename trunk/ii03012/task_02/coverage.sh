#!/usr/bin/env bash
# Measure production code exercised by both Google Test and the real console application.
set -euo pipefail
cd "$(dirname "$0")"
root="$PWD"
build="$root/build/coverage"
report="$build/report"
mkdir -p "$build"
profiles=$(mktemp -d "$build/profiles.XXXXXX")
trap 'rm -rf "$profiles"' EXIT
if [[ $(uname -s) == Darwin ]]; then
    llvm=(xcrun)
else
    llvm=()
fi
export LLVM_PROFILE_FILE="$profiles/setup-%m-%p.profraw"
cmake -S . -B "$build" -DBUILD_TESTING=ON -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_COMPILER=clang++ '-DCMAKE_CXX_FLAGS=-fprofile-instr-generate -fcoverage-mapping' "$@"
cmake --build "$build" --parallel
mkdir "$profiles/tests"
export LLVM_PROFILE_FILE="$profiles/tests/%m-%p.profraw"
ctest --test-dir "$build" --output-on-failure --output-junit "$build/test-results.xml"
"${llvm[@]}" llvm-profdata merge -sparse "$profiles/tests/"*.profraw -o "$profiles/tests.profdata"

lab1="$root/../task_01/src"
if [[ ! -f "$lab1/Model.cpp" ]]; then
    lab1="$root/../../ii03004/task_01/src"
fi
args=("$build/ii03012_tests" -object "$build/src/ii03012_task_02"
      "-instr-profile=$profiles/tests.profdata")
sources=("$root"/src/*.cpp "$root"/src/*.h "$lab1"/{Model,InputSignal,NonLinearModels}.{cpp,h})
mkdir -p "$report"
"${llvm[@]}" llvm-cov report "${args[@]}" "${sources[@]}" | tee "$report/summary.txt"
"${llvm[@]}" llvm-cov show "${args[@]}" -format=html "-output-dir=$report" "${sources[@]}"
echo "HTML: $report/index.html"
