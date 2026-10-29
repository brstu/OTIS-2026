@echo off
echo Сборка проекта...

if not exist build mkdir build
cd build

cmake .. -G "MinGW Makefiles"
cmake --build .

echo Запуск программы...
lab1.exe

cd ..
pause