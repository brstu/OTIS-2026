@echo off
chcp 65001 > nul
echo === 1. Компиляция проекта ===
cmake --build build

if %errorlevel% neq 0 (
    echo [Ошибка] Компиляция не удалась!
    pause
    exit /b %errorlevel%
)

echo.
echo === 2. Запуск симуляции (C++) ===
REM Путь к твоему скомпилированному exe-файлу
build\lab01.exe

echo.
echo === 3. Построение графика (Python) ===
if exist plot.py (
    python plot.py
) else (
    echo [Предупреждение] Файл plot.py не найден, график не построен.
)

echo.
echo === Всё готово! ===
pause