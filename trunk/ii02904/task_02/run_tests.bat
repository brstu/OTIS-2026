@echo off
setlocal
cd /d "%~dp0"
chcp 65001 >nul

echo =======================================
echo  Сборка проекта и запуск тестов
echo =======================================

where cmake >nul 2>nul
if errorlevel 1 (
    echo Ошибка: CMake не найден в PATH.
    exit /b 1
)

where g++ >nul 2>nul
if errorlevel 1 (
    set "BUILD_DIR=build-msvc"
    cmake -S . -B build-msvc
) else (
    set "BUILD_DIR=build-mingw"
    cmake -S . -B build-mingw -G "MinGW Makefiles"
)
if errorlevel 1 exit /b 1

cmake --build %BUILD_DIR% --config Release
if errorlevel 1 exit /b 1

echo.
echo =======================================
echo  Google Test
echo =======================================
ctest --test-dir %BUILD_DIR% --output-on-failure --output-junit test-results.xml
if errorlevel 1 exit /b 1

where gcovr >nul 2>nul
if errorlevel 1 (
    echo gcovr не найден; отчет о прохождении тестов сохранен в test-results.xml.
    exit /b 0
)

gcovr --root . --filter "src/.*\.cpp$" --exclude "src/main.cpp" --xml-pretty --output %BUILD_DIR%\coverage.xml --html-details %BUILD_DIR%\coverage.html --print-summary
if errorlevel 1 exit /b 1

echo Отчет покрытия: %BUILD_DIR%\coverage.xml
exit /b 0
