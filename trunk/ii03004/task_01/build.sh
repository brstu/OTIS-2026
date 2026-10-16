#!/usr/bin/env bash
# Аналог build.bat для macOS / Linux.
# Запуск: ./build.sh   (если "Permission denied": chmod +x build.sh)

cd "$(dirname "$0")" || exit 1

echo "======================================="
echo " OTIS-2026  Task 01  Variant 4"
echo " Build process started"
echo "======================================="

if ! command -v cmake > /dev/null 2>&1; then
    echo "[ERROR] CMake not found. Install it: brew install cmake"
    exit 1
fi

echo
echo "[1/3] Configuration (cmake)"
echo "---------------------------------------"
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release || { echo "[ERROR] Configuration failed"; exit 1; }

echo
echo "[2/3] Compilation"
echo "---------------------------------------"
cmake --build build --config Release || { echo "[ERROR] Build failed"; exit 1; }

echo
echo "[3/3] Running the program"
echo "---------------------------------------"
echo "SUCCESS: build completed, executable is in build/bin"
echo
cd build/bin && ./ii03004_task_01

echo
echo "======================================="
echo " Done"
echo "======================================="
