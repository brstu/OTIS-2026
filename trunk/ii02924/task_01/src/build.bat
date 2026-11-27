@echo off
chcp 65001 > nul
cd /d "%~dp0"

if not exist build (
    mkdir build
)

cd build

echo --- Configuring CMake ---
cmake ..
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] CMake configuration failed!
    cd /d "%~dp0"
    pause
    exit /b 1
)

echo --- Building Project ---
cmake --build . --config Release
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Build failed!
    cd /d "%~dp0"
    pause
    exit /b 1
)

echo --- Build Successful ---
echo --- Starting Simulation ---

if exist Release\task_01.exe (
    cd Release
    task_01.exe
) else if exist task_01.exe (
    task_01.exe
)

cd /d "%~dp0"
pause