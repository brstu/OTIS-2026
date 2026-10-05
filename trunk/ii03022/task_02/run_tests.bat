@echo off
title task_02 - build and run unit tests
setlocal

where cmake >nul 2>nul
if %errorlevel% neq 0 (
    echo.
    echo  *** CMake not found in PATH! ***
    echo  Install: winget install -e --id Kitware.CMake
    echo.
    pause
    exit /b 1
)

REM --- если есть gcc (MinGW), собираем им: покрытие gcov возможно только с GCC ---
set "GENERATOR="
set "COV_FLAGS="
where g++ >nul 2>nul
if %errorlevel% equ 0 (
    set GENERATOR=-G "MinGW Makefiles"
    set COV_FLAGS=-DCMAKE_CXX_FLAGS="-g -O0 --coverage"
)
REM ВНИМАНИЕ: после смены генератора удали вручную папку build и запусти заново!

echo =======================================
echo  [1/3] Configure + build (Google Test)
echo =======================================
cmake -S . -B build %GENERATOR% %COV_FLAGS%
if errorlevel 1 goto error

cmake --build build --config Release
if errorlevel 1 goto error

echo.
echo =======================================
echo  [2/3] Run tests with ctest
echo =======================================
cd build
ctest --output-on-failure > ..\test_report.txt 2>&1
set RESULT=%errorlevel%
type ..\test_report.txt
cd ..

echo.
echo =======================================
echo  [3/3] Coverage report (gcovr, optional)
echo =======================================
where gcovr >nul 2>nul
if %errorlevel% equ 0 (
    gcovr -r src build --print-summary
) else (
    echo  gcovr not found - coverage skipped. Install: pip install gcovr
)

if %RESULT% neq 0 goto error
echo.
echo  All tests passed! Report saved to test_report.txt
pause
exit /b 0

:error
echo.
echo *** FAILED! See output above. ***
pause
exit /b 1