@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

echo [1/5] Создание директории сборки...
if not exist build mkdir build
if %ERRORLEVEL% neq 0 (
    echo [ОШИБКА] Не удалось создать директорию build
    pause
    exit /b 1
)
cd build
echo OK

echo.
echo [2/5] Конфигурация проекта CMake...
cmake ../src -G "MinGW Makefiles" > cmake_config.log 2>&1
if %ERRORLEVEL% neq 0 (
    echo [ОШИБКА] Ошибка конфигурации CMake!
    echo Подробности в файле: build\cmake_config.log
    echo.
    type cmake_config.log
    cd ..
    pause
    exit /b 1
)
echo OK

echo.
echo [3/5] Компиляция проекта...
cmake --build . > build.log 2>&1
if %ERRORLEVEL% neq 0 (
    echo [ОШИБКА] Ошибка компиляции!
    echo Подробности в файле: build\build.log
    echo.
    type build.log
    cd ..
    pause
    exit /b 1
)
echo OK

echo.
echo [4/5] Запуск модульных тестов...
echo ========================================

for /f "tokens=2 delims==" %%a in ('wmic OS Get localdatetime /value') do set "dt=%%a"
set "YEAR=%dt:~0,4%"
set "MONTH=%dt:~4,2%"
set "DAY=%dt:~6,2%"
set "HOUR=%dt:~8,2%"
set "MINUTE=%dt:~10,2%"
set "SECOND=%dt:~12,2%"
set "REPORT_NAME=test_report_%YEAR%-%MONTH%-%DAY%_%HOUR%-%MINUTE%-%SECOND%.txt"

if exist run_tests.exe (
    run_tests.exe > "%REPORT_NAME%" 2>&1
    set TEST_EXIT_CODE=%ERRORLEVEL%
    set "TEST_PATH=%REPORT_NAME%"
) else if exist Debug\run_tests.exe (
    Debug\run_tests.exe > "%REPORT_NAME%" 2>&1
    set TEST_EXIT_CODE=%ERRORLEVEL%
    set "TEST_PATH=%REPORT_NAME%"
) else if exist Release\run_tests.exe (
    Release\run_tests.exe > "%REPORT_NAME%" 2>&1
    set TEST_EXIT_CODE=%ERRORLEVEL%
    set "TEST_PATH=%REPORT_NAME%"
) else (
    echo [ОШИБКА] Исполняемый файл тестов не найден!
    cd ..
    pause
    exit /b 1
)

echo ========================================
echo.

echo [5/5] Генерация отчёта...
echo.

(
    echo ========================================
    echo   ОТЧЁТ О ПРОХОЖДЕНИИ ТЕСТОВ
    echo ========================================
    echo Дата выполнения: %DAY%.%MONTH%.%YEAR% %HOUR%:%MINUTE%:%SECOND%
    echo ========================================
    echo.
    type "%REPORT_NAME%"
    echo.
    echo ========================================
    if %TEST_EXIT_CODE% equ 0 (
        echo ИТОГ: ВСЕ ТЕСТЫ ПРОЙДЕНЫ УСПЕШНО
    ) else (
        echo ИТОГ: НЕКОТОРЫЕ ТЕСТЫ НЕ ПРОЙДЕНЫ ^(код: %TEST_EXIT_CODE%^)
    )
    echo ========================================
) > "temp_report.txt"

move /y "temp_report.txt" "%REPORT_NAME%" >nul

type "%REPORT_NAME%"

echo.
echo ========================================
echo   ИНФОРМАЦИЯ ОБ ОТЧЁТЕ
echo ========================================
echo Файл отчёта сохранён:
echo   %CD%\%REPORT_NAME%
echo.
echo ========================================

cd ..

echo.
if %TEST_EXIT_CODE% equ 0 (
    echo [УСПЕХ] Все тесты пройдены!
) else (
    echo [ОШИБКА] Некоторые тесты не пройдены!
    echo Код завершения: %TEST_EXIT_CODE%
)
echo ========================================

echo.
pause
exit /b %TEST_EXIT_CODE%