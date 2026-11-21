@echo off
REM ============================================================
REM  run_all.bat для ЛР №3
REM  1) Сборка проекта
REM  2) Запуск тестов
REM  3) Генерация Doxygen-документации
REM ============================================================

setlocal
cd /d "%~dp0"

echo ============================================
echo  [STEP 1/3] Building project...
echo ============================================
echo.

where cmake >nul 2>nul
if errorlevel 1 (
    echo [ERROR] CMake not found in PATH.
    pause
    exit /b 1
)

if not exist "build" mkdir build
cd build

cmake ..
if errorlevel 1 (
    echo [ERROR] CMake configuration failed.
    pause
    exit /b 1
)

cmake --build . --config Debug
if errorlevel 1 (
    echo [ERROR] Build failed.
    pause
    exit /b 1
)

cd ..

echo.
echo ============================================
echo  [STEP 2/3] Running tests...
echo ============================================
echo.

build\test\Debug\run_tests_03.exe
if errorlevel 1 (
    echo.
    echo [FAILED] Some tests failed!
    pause
    exit /b 1
)

echo.
echo ============================================
echo  [STEP 3/3] Generating Doxygen docs...
echo ============================================
echo.

where doxygen >nul 2>nul
if errorlevel 1 (
    echo [ERROR] Doxygen not found in PATH.
    pause
    exit /b 1
)

doxygen Doxyfile
if errorlevel 1 (
    echo [ERROR] Doxygen generation failed.
    pause
    exit /b 1
)

echo.
echo ============================================
echo  Execution finished successfully!
echo ============================================
echo.
echo  HTML documentation: doc\html\index.html
echo.

pause
endlocal