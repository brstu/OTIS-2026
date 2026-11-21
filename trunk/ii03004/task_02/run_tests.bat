@echo off
chcp 65001 > nul
cd /d "%~dp0"

echo ========================================
echo Compiling project and running tests...
echo ========================================

if not exist build mkdir build
cd build

cmake .. -G "MinGW Makefiles"
cmake --build .

echo ========================================
echo Running Google Test...
echo ========================================
ctest --output-on-failure

cd ..
pause