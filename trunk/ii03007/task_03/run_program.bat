@echo off
chcp 65001 >nul
title ЛР3 ii03007: запуск программы
setlocal

set "EXE=build\src\Release\task3_ii03007.exe"
if not exist "%EXE%" set "EXE=build\src\task3_ii03007.exe"
if exist "%EXE%" goto run_program

echo  EXE не найден - сначала собираем проект...
where cmake >nul 2>nul
if errorlevel 1 (
    echo  *** CMake не найден в PATH! ***
    echo  Установка: winget install -e --id Kitware.CMake
    goto fail
)

set "GEN="
where g++ >nul 2>nul || goto build
where mingw32-make >nul 2>nul || goto build
set GEN=-G "MinGW Makefiles"

:build
cmake -S . -B build %GEN%
if errorlevel 1 goto fail
cmake --build build --config Release
if errorlevel 1 goto fail

set "EXE=build\src\Release\task3_ii03007.exe"
if not exist "%EXE%" set "EXE=build\src\task3_ii03007.exe"
if not exist "%EXE%" (
    echo  *** task3_ii03007.exe не найден после сборки! ***
    goto fail
)

:run_program
echo.
echo  Ответьте на вопросы: объект, коэффициенты ПИД, задание w, режим А/Б.
echo.
"%EXE%"
if errorlevel 1 goto end

if exist pid_result.csv (
    where python >nul 2>nul
    if not errorlevel 1 python plot.py
)
if exist graph.png start "" graph.png

:end
echo.
pause
exit /b 0

:fail
echo.
echo *** ОШИБКА. Смотрите вывод выше. ***
pause
exit /b 1