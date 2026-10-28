@echo off
echo Starting configuration...
if not exist build mkdir build
cd build
cmake -G "Visual Studio 17 2022" -A x64 ..
if %errorlevel% neq 0 (
    echo CMake configuration failed!
    pause
    exit /b 1
)
echo.
echo Building project...
cmake --build . --config Release
if %errorlevel% neq 0 (
    echo Build failed!
    pause
    exit /b 1
)
echo.
echo Build completed successfully!
cd ..
pause
