@echo off
chcp 65001 >nul
echo ========================================
echo  Compiling project and running tests...
echo ========================================

:: Переход в папку, где лежит этот .bat файл (ii03019/task_02/src)
cd /d "%~dp0"

if not exist build mkdir build
cd build

:: Конфигурация CMake
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="--coverage"
if %errorlevel% neq 0 exit /b %errorlevel%

:: Сборка
cmake --build .
if %errorlevel% neq 0 exit /b %errorlevel%

echo ========================================
echo  Running Google Test...
echo ========================================
.\test_runner.exe

echo ========================================
echo  Generating Coverage Reports...
echo ========================================
:: Генерация покрытия для ЛР1 и текущей папки (ЛР2)
python -m pip install gcovr
python -m gcovr -r ../../.. --html-details coverage.html --exclude ".*main\.cpp" --exclude ".*test_.*" --exclude ".*CompilerId.*"

start "" coverage.html

cd ..
pause