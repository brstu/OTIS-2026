@echo off
setlocal

echo === [1/5] Creating build directory ===
if not exist build (
    mkdir build
)
cd build

echo === [2/5] Configuring project with CMake (MinGW) ===
cmake -G "MinGW Makefiles" ..
if %errorlevel% neq 0 (
    echo [ERROR] CMake configuration failed!
    pause
    exit /b %errorlevel%
)

echo === [3/5] Compiling the project ===
cmake --build .
if %errorlevel% neq 0 (
    echo [ERROR] Build failed!
    pause
    exit /b %errorlevel%
)

echo.
echo === [4/5] Running simulation ===
task_01.exe
if %errorlevel% neq 0 (
    echo [ERROR] Simulation failed!
    pause
    exit /b %errorlevel%
)

cd ..

echo.
echo === [5/5] Generating and opening plot ===
py src\plot.py
if %errorlevel% neq 0 (
    echo [ERROR] Python script failed!
    pause
    exit /b %errorlevel%
)

echo.
echo ========================================
echo All done! Graph saved to doc/ and opened.
echo ========================================
pause