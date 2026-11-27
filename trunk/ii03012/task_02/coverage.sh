#!/usr/bin/env bash
# Measure production code exercised by both Google Test and the real console application.
set -euo pipefail
cd "$(dirname "$0")"
root="$PWD"
build="$root/build/coverage"
report="$build/report"
mode=ci
case "${1:-}" in
    --demo|--tests) mode=$1; shift ;;
esac
mkdir -p "$build"
profiles=$(mktemp -d "$build/profiles.XXXXXX")
trap 'rm -rf "$profiles"' EXIT
if [[ $(uname -s) == Darwin ]]; then
    llvm=(xcrun)
else
    llvm=()
fi
export LLVM_PROFILE_FILE="$profiles/setup-%m-%p.profraw"
if [[ -n ${OTIS_MAC_BUNDLE:-} ]]; then
    app="$OTIS_MAC_BUNDLE/mac/ii03012_task_02"
    tests="$OTIS_MAC_BUNDLE/mac/ii03012_tests"
    report="$OTIS_MAC_BUNDLE/Покрытие"
else
    cmake -S . -B "$build" -DBUILD_TESTING=ON -DCMAKE_BUILD_TYPE=Debug \
        -DCMAKE_CXX_COMPILER=clang++ '-DCMAKE_CXX_FLAGS=-fprofile-instr-generate -fcoverage-mapping' "$@"
    cmake --build "$build" --parallel
    app="$build/src/ii03012_task_02"
    tests="$build/ii03012_tests"
fi
if [[ "$mode" == --demo ]]; then
    results="${OTIS_MAC_BUNDLE:-$build}/Результаты"
    mkdir -p "$results"
    cp "$(dirname "$app")/plot.py" "$results/plot.py" 2>/dev/null ||
        cp "$root/../task_01/src/plot.py" "$results/plot.py"
    (cd "$results" && MPLBACKEND=Agg "$app")
fi
mkdir "$profiles/tests"
export LLVM_PROFILE_FILE="$profiles/tests/%m-%p.profraw"
if [[ "$mode" == ci ]]; then
    ctest --test-dir "$build" --output-on-failure --output-junit "$build/test-results.xml"
else
    printf '\nРезультаты 27 модульных тестов\n'
    "$tests"
    (cd "$profiles" && cmake "-DPROGRAM=$app" -P "$root/test/console.cmake")
    printf 'Сквозная проверка программы пройдена.\n'
fi
"${llvm[@]}" llvm-profdata merge -sparse "$profiles/tests/"*.profraw -o "$profiles/tests.profdata"

lab1="$root/../task_01/src"
if [[ ! -f "$lab1/Model.cpp" ]]; then
    lab1="$root/../../ii03004/task_01/src"
fi
args=("$tests" -object "$app"
      "-instr-profile=$profiles/tests.profdata")
if [[ -n ${OTIS_MAC_BUNDLE:-} ]]; then
    args+=("-path-equivalence=/otis-demo,$OTIS_MAC_BUNDLE")
fi
sources=("$root"/src/*.cpp "$root"/src/*.h "$lab1"/{Model,InputSignal,NonLinearModels}.{cpp,h})
mkdir -p "$report"
"${llvm[@]}" llvm-cov report "${args[@]}" "${sources[@]}" | tee "$report/summary.txt"
"${llvm[@]}" llvm-cov show "${args[@]}" -format=html "-output-dir=$report" "${sources[@]}"
echo "HTML: $report/index.html"
awk '$1 == "TOTAL" {print "Покрытие тестами: строки " $10 " (" $8-$9 " из " $8 "), функции " $7 " (" $5-$6 " из " $5 ")."}' "$report/summary.txt"
