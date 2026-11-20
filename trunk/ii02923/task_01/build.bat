@echo off
chcp 65001 >nul
setlocal EnableExtensions
cd /d "%~dp0"

echo ============================================================
echo  Laboratory work No.1, variant 11
echo  Savko Nikita, group II-29
echo ============================================================
echo.

where cmake >nul 2>nul
if errorlevel 1 (
    echo [ERROR] CMake was not found in PATH.
    echo Install CMake: https://cmake.org/download/
    echo During installation enable "Add CMake to the system PATH".
    goto :fail
)

set "GENERATOR="
where g++ >nul 2>nul
if not errorlevel 1 (
    set "GENERATOR=-G MinGW Makefiles"
    echo [INFO] MinGW g++ found - using "MinGW Makefiles" generator.
) else (
    echo [INFO] MinGW not found - using the default CMake generator.
)

echo.
echo [1/4] Configuring the project...
cmake -S . -B build %GENERATOR%
if errorlevel 1 goto :fail

echo.
echo [2/4] Building...
cmake --build build --config Release
if errorlevel 1 goto :fail

set "EXE=build\bin\otis_lab1_ii02911.exe"
if not exist "%EXE%" (
    echo [ERROR] Executable not found: %EXE%
    goto :fail
)

echo.
echo [3/4] Demo run: model 1.1, step signal, 20 steps...
echo.
"%EXE%" --model 1.1 --signal step --steps 20

echo.
echo [4/4] Building plots...
where python >nul 2>nul
if errorlevel 1 (
    echo [WARNING] Python not found - plots were not generated.
    echo Install Python and run: pip install matplotlib
) else (
    python plot_results.py
    if errorlevel 1 echo [WARNING] Failed to build plots.
)

echo.
echo ============================================================
echo  DONE. The table is printed above; plots are in doc\screenshots.
echo ============================================================
pause
exit /b 0

:fail
echo.
echo ============================================================
echo  BUILD FAILED. Copy the log above and send it to the teacher.
echo ============================================================
pause
exit /b 1
