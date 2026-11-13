@echo off
chcp 65001 >nul
cd /d "%~dp0"
echo ==== Сборка ЛР1 (вариант 15) ====
echo [1/2] Конфигурация CMake...
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
if errorlevel 1 (
    echo [ERROR] Ошибка конфигурации. Установлены ли CMake и компилятор?
    pause
    exit /b 1
)
echo [2/2] Компиляция...
cmake --build build --config Release
if errorlevel 1 (
    echo [ERROR] Ошибка компиляции.
    pause
    exit /b 1
)
echo [OK] Сборка завершена: build\bin\lab01.exe
pause
