@echo off
echo ===========================
echo Compiling project and running tests
echo ===========================

if not exist build mkdir build
cd build

cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Debug

echo ===========================
echo Running Google Test...
echo ===========================
.\trunk\ii02906\task_02\src\Debug\run_tests.exe

cd ..
pause