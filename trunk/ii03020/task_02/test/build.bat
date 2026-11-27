@echo off
cd ..
if not exist build mkdir build
cd build
cmake .. -G "MinGW Makefiles" -DENABLE_COVERAGE=ON
cmake --build .
cd ../test