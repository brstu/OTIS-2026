@echo off
chcp 65001 > nul
echo [1/3] Компиляция C++ проекта...

REM Компилируем все файлы модуля вместе в один исполняемый файл simulation.exe
g++ main.cpp Models.cpp Utils.cpp -o simulation.exe

if %errorlevel% neq 0 (
    echo [ОШИБКА] Ошибка компиляции C++ кода!
    pause
    exit /b %errorlevel%
)

echo [2/3] Запуск C++ симуляции (генерация CSV)...
simulation.exe

echo [3/3] Генерация графиков на Python...
python plot.py

echo.
echo Все готово! Графики и CSV файлы успешно созданы.
pause