@echo off
echo Project build...

if not exist build mkdir build
cd build

cmake .. -G "MinGW Makefiles"
cmake --build .

echo To run a program...
lab1.exe

cd ..
cmd /k