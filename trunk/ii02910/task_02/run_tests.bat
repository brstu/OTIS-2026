@echo off
REM ============================================================
REM  run_tests.bat - Build and run all unit tests
REM ============================================================

setlocal
cd /d "%~dp0"

echo ============================================
echo  Building task_02 with tests...
echo ============================================
echo.

set CMAKE_BIN=C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe

if not exist "%CMAKE_BIN%" (
    echo [ERROR] CMake not found at:
    echo   %CMAKE_BIN%
    pause
    exit /b 1
)

if not exist "build" mkdir build
cd build

echo [1/3] Configuring with CMake...
"%CMAKE_BIN%" .. -DCMAKE_BUILD_TYPE=Release
if errorlevel 1 (
    echo [ERROR] CMake configuration failed.
    pause
    exit /b 1
)

echo.
echo [2/3] Building...
"%CMAKE_BIN%" --build . --config Release
if errorlevel 1 (
    echo [ERROR] Build failed.
    pause
    exit /b 1
)

echo.
echo [3/3] Running tests...
cd ..

REM Try different possible locations
set TEST_EXE=

if exist "out\build\x64-Debug\test\run_tests.exe" set TEST_EXE=out\build\x64-Debug\test\run_tests.exe
if exist "out\build\x64-Release\test\run_tests.exe" set TEST_EXE=out\build\x64-Release\test\run_tests.exe
if exist "build\test\Release\run_tests.exe" set TEST_EXE=build\test\Release\run_tests.exe
if exist "build\test\Debug\run_tests.exe" set TEST_EXE=build\test\Debug\run_tests.exe

if "%TEST_EXE%"=="" (
    echo [ERROR] run_tests.exe not found.
    echo Tried:
    echo   out\build\x64-Debug\test\run_tests.exe
    echo   out\build\x64-Release\test\run_tests.exe
    echo   build\test\Release\run_tests.exe
    echo   build\test\Debug\run_tests.exe
    pause
    exit /b 1
)

echo Running: %TEST_EXE%
echo.
"%TEST_EXE%"

if errorlevel 1 (
    echo.
    echo ============================================
    echo  Some tests FAILED!
    echo ============================================
) else (
    echo.
    echo ============================================
    echo  ALL TESTS PASSED!
    echo ============================================
)

pause
endlocal