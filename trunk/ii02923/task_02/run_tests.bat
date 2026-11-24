@echo off
setlocal enabledelayedexpansion

echo =======================================
echo  Compiling project and running tests...
echo =======================================

if exist coverage_report rmdir /s /q coverage_report
if not exist build mkdir build
cd build

cmake .. -G "MinGW Makefiles" -DENABLE_COVERAGE=ON -Wno-dev
if %errorlevel% neq 0 (
    echo [ERROR] CMake configuration failed.
    cd ..
    pause
    exit /b %errorlevel%
)

cmake --build .
if %errorlevel% neq 0 (
    echo [ERROR] Build failed.
    cd ..
    pause
    exit /b %errorlevel%
)

echo =======================================
echo  Running Google Test via CTest...
echo =======================================
ctest --output-on-failure --output-junit test_report.xml
if %errorlevel% neq 0 (
    echo [ERROR] Some unit tests failed!
    cd ..
    pause
    exit /b %errorlevel%
)

if exist src\task_02.exe (
    (echo 5& echo 1& echo 1& echo & echo & echo & echo & echo ) > _in1.txt && src\task_02.exe < _in1.txt >nul 2>&1
    (echo 5& echo 2& echo 2& echo & echo & echo & echo & echo & echo ) > _in2.txt && src\task_02.exe < _in2.txt >nul 2>&1
    (echo 5& echo 3& echo 3& echo & echo & echo & echo & echo ) > _in3.txt && src\task_02.exe < _in3.txt >nul 2>&1
    src\task_02.exe 3 < NUL >nul 2>&1
    src\task_02.exe abc < NUL >nul 2>&1
    src\task_02.exe 999999999999999999999999999999 < NUL >nul 2>&1
    src\task_02.exe 2 Z:\nonexistent_dir\results.csv < NUL >nul 2>&1
    del /q _in1.txt _in2.txt _in3.txt >nul 2>&1
)

where gcovr >nul 2>nul
if %errorlevel% equ 0 (
    echo =======================================
    echo  Generating HTML coverage report...
    echo =======================================
    if not exist coverage_report mkdir coverage_report
    gcovr -r .. --filter "../src" --exclude-throw-branches --exclude-unreachable-branches --html-details -o coverage_report/index.html
    echo HTML report saved to: build\coverage_report\index.html
) else (
    echo [WARNING] gcovr not found in PATH! Install via: pip install gcovr
)

cd ..
pause