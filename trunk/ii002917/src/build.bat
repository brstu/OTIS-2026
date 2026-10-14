@echo off
chcp 65001 >nul
echo ============================================
echo  Скомпилировано для: Вариант-14 (OTIS)
echo  Модели по вариант: 1.4, 2.8, 3.2
echo ============================================

cd /d "%~dp0"

if not exist out_build mkdir out_build
cd out_build

echo.
echo STEP 1/3 Подготовка файдов проекта...
cmake .. -G "MinGW Makefiles"

echo.
echo STEP 2/3 Компиляция...
cmake --build .

echo.
echo STEP 3/3 Старт исполняемого файла...
otis_task_01.exe

echo.
echo ============================================
echo  Процесс завершен!
echo ============================================
pause
