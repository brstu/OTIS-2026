@echo off
echo =========================================
echo  Compiling project and running tests...
echo =========================================

if not exist build mkdir build
cd build

cmake .. -G "MinGW Makefiles"
cmake --build .

echo =========================================
echo  Running Google Test...
echo =========================================
.\run_tests.exe

cd ..
pause