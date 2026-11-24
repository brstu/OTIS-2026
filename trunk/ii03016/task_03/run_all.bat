@echo off
chcp 65001 > nul
cd /d "%~dp0"

echo ==========================================
echo 1. Очистка старого кэша и сборка в build
echo ==========================================
if exist CMakeCache.txt del /q CMakeCache.txt
if exist CMakeFiles rmdir /s /q CMakeFiles
if exist build rmdir /s /q build

mkdir build
cd build
cmake .. -G "MinGW Makefiles" -DCMAKE_POLICY_VERSION_MINIMUM=3.5
if %errorlevel% neq 0 goto :error

cmake --build .
if %errorlevel% neq 0 goto :error

echo ==========================================
echo 2. Запуск модульных тестов (Google Test)
echo ==========================================
.\run_tests.exe
if %errorlevel% neq 0 goto :error

cd ..

echo ==========================================
echo 3. Генерация документации (Doxygen)
echo ==========================================
doxygen Doxyfile
if %errorlevel% neq 0 goto :error

echo ==========================================
echo 4. Запуск основной программы (ПИД-регулятор)
echo ==========================================
.\build\task_03_pid.exe

echo ==========================================
echo 5. Очистка временных файлов сборки
echo ==========================================
if exist build rmdir /s /q build
if exist simulation_results.csv del /q simulation_results.csv

echo ==========================================
echo Готово! Все этапы выполнены.
echo ==========================================
pause
exit /b 0

:error
cd /d "%~dp0"
echo.
echo [ОШИБКА] Выполнение прервано из-за ошибки на этапе выше!
pause
exit /b 1
