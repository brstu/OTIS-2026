#!/bin/bash
set -euo pipefail
cd "$(dirname "$0")"
export PATH="/opt/homebrew/bin:/usr/local/bin:$PATH"
base="$PWD"
build="$base/build-app"
cmake -S . -B "$build" -G "Unix Makefiles" -DBUILD_TESTING=OFF -DCMAKE_BUILD_TYPE=Release
cmake --build "$build" --parallel
work="$(mktemp -d "$build/demo.XXXXXX")"
trap 'rm -rf "$work"' EXIT
(cd "$work" && "$build/src/ii02920_task02")
if [[ -f "$work/result.csv" ]]; then
    cp "$work/result.csv" "$base/result.csv"
    python3 "$base/src/plot_results.py" < "$work/result.csv" > "$work/graph.svg"
    mv -f "$work/graph.svg" "$base/graph.svg"
    echo "График: $base/graph.svg"
    if [[ "${1:-}" != "--no-open" ]]; then
        open -a Safari "$base/graph.svg"
    fi
fi
