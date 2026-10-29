@echo off
echo === Configuring and building task_01_ii02913 ===
if not exist build mkdir build
cd build
cmake ..
cmake --build .
if exist Debug\task_01_ii02913.exe (
    cd Debug
    task_01_ii02913.exe
    python plot.py
    cd ..
) else (
    task_01_ii02913.exe
    python plot.py
)
cd ..
echo === Done! ===
pause