@echo off
chcp 65001 > nul
setlocal enabledelayedexpansion

echo =======================================
echo  Compiling project and running tests...
echo =======================================

REM Проверяем наличие CMake
where cmake > nul 2>&1
if errorlevel 1 (
    echo [ОШИБКА] CMake не найден в PATH!
    pause
    exit /b 1
)

REM Проверяем наличие компилятора g++
where g++ > nul 2>&1
if errorlevel 1 (
    echo [ОШИБКА] g++ не найден в PATH!
    pause
    exit /b 1
)

REM Переходим в каталог task_02
cd /d "%~dp0task_02"

REM Создаём каталог сборки
if not exist "build" mkdir "build"
cd "build"

echo Запуск CMake конфигурации...
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug
if %errorlevel% neq 0 (
    echo [ОШИБКА] Ошибка конфигурации CMake!
    cd ..\..
    pause
    exit /b %errorlevel%
)

echo Сборка проекта и тестов...
cmake --build . --config Debug
if %errorlevel% neq 0 (
    echo [ОШИБКА] Ошибка сборки проекта!
    cd ..\..
    pause
    exit /b %errorlevel%
)

echo =======================================
echo  Running Google Test...
echo =======================================

ctest --output-on-failure
set test_result=%errorlevel%

echo =======================================
echo  Generating coverage report (gcovr)...
echo =======================================

REM Поиск gcovr
where gcovr > nul 2>&1
if errorlevel 1 (
    echo [ПРЕДУПРЕЖДЕНИЕ] gcovr не найден, отчёт о покрытии не будет сгенерирован.
    echo Установите gcovr: pip install gcovr
) else (
    gcovr --html --html-details -o coverage.html ..
    if %errorlevel% neq 0 (
        echo [ПРЕДУПРЕЖДЕНИЕ] Ошибка при генерации отчёта о покрытии.
    ) else (
        echo Отчёт о покрытии сохранён: build\coverage.html
    )
)

cd ..\..

echo.
echo =======================================
if %test_result% equ 0 (
    echo Все тесты успешно пройдены!
) else (
    echo [ОШИБКА] Некоторые тесты не прошли. Код возврата: %test_result%
)
echo =======================================

pause
exit /b %test_result%