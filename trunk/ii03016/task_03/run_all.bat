@echo off
chcp 65001 > nul

echo ==========================================
echo 1. Конфигурация и сборка проекта (CMake)
echo ==========================================
cmake . -G "MinGW Makefiles"
if %errorlevel% neq 0 exit /b %errorlevel%

cmake --build .
if %errorlevel% neq 0 exit /b %errorlevel%

echo ==========================================
echo 2. Запуск модульных тестов (Google Test)
echo ==========================================
.\run_tests.exe
if %errorlevel% neq 0 exit /b %errorlevel%

echo ==========================================
echo 3. Генерация документации (Doxygen)
echo ==========================================
doxygen Doxyfile
if %errorlevel% neq 0 exit /b %errorlevel%

echo ==========================================
echo Готово! Сборка, тесты и документация выполнены.
echo ==========================================
pause
