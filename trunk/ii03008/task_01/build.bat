@echo off
echo Project build...

if not exist build mkdir build
cd build

rem 
cmake ..\src -G "MinGW Makefiles"
cmake --build .

echo To run a program...
.\lab1_ii03008.exe

cd ..
cmd /k




