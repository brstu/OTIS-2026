
@echo off
chcp 65001 > nul
setlocal

cd /d "%~dp0"

echo ========================================
echo       СБОРКА И ТЕСТИРОВАНИЕ TASK_02
echo ========================================

if not exist build (
    echo Создание проекта...
    cmake -S . -B build -G "MinGW Makefiles"

    if errorlevel 1 (
        echo Ошибка конфигурации CMake!
        pause
        exit /b 1
    )
)

echo.
echo Сборка проекта...

cmake --build build --config Debug

if errorlevel 1 (
    echo Ошибка сборки!
    pause
    exit /b 1
)

set TEST_EXE=

if exist "build\Debug\unit_tests.exe" (
    set TEST_EXE=build\Debug\unit_tests.exe
)

if exist "build\unit_tests.exe" (
    set TEST_EXE=build\unit_tests.exe
)

if "%TEST_EXE%"=="" (
    echo Не найден unit_tests.exe!
    pause
    exit /b 1
)

echo.
echo Запуск тестов...

"%TEST_EXE%" --gtest_output=xml:build\test_results.xml

set TEST_RESULT=%ERRORLEVEL%

echo.
echo Создание HTML-отчета через Python и gcovr...

python generate_html_report.py --xml build\test_results.xml --html build\test_report.html --build-dir build

if errorlevel 1 (
    echo Не удалось создать HTML-отчет!
    pause
    exit /b 1
)

echo.
echo Открытие HTML-отчета...

start "" "%CD%\build\test_report.html"

echo.
echo ========================================
echo                 ГОТОВО
echo ========================================

if not "%TEST_RESULT%"=="0" (
    echo Внимание: некоторые тесты завершились с ошибками.
)

pause
exit /b %TEST_RESULT%
