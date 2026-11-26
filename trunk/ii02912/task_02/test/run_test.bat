
@echo off
chcp 65001 >nul
setlocal EnableExtensions

rem ========================================
rem PATHS
rem ========================================

set "ROOT_DIR=%~dp0.."
set "SRC_DIR=%ROOT_DIR%\src"
set "TEST_DIR=%ROOT_DIR%\test"

set "MSYS2_BIN=C:\msys64\ucrt64\bin"
set "MSYS2_USR_BIN=C:\msys64\usr\bin"
set "PATH=%MSYS2_BIN%;%MSYS2_USR_BIN%;%PATH%"

set "BUILD_DIR=%SRC_DIR%\build-gcc\tests"
set "RESULTS_DIR=%SRC_DIR%\results"
set "TEST_RESULTS=%RESULTS_DIR%\tests"
set "COVERAGE_DIR=%RESULTS_DIR%\coverage"

rem ========================================
rem CHECK TOOLS
rem ========================================

echo.
echo ========== CHECKING TOOLS ==========

where g++.exe >nul 2>&1
if errorlevel 1 goto :error_gcc

where cmake.exe >nul 2>&1
if errorlevel 1 goto :error_cmake

where ninja.exe >nul 2>&1
if errorlevel 1 goto :error_ninja

where gcov.exe >nul 2>&1
if errorlevel 1 goto :error_gcov

where gcovr.exe >nul 2>&1
if errorlevel 1 goto :error_gcovr

echo [OK] GCC
echo [OK] CMake
echo [OK] Ninja
echo [OK] gcov
echo [OK] gcovr

rem ========================================
rem PREPARE DIRECTORIES
rem ========================================

if not exist "%TEST_RESULTS%" mkdir "%TEST_RESULTS%"
if not exist "%COVERAGE_DIR%" mkdir "%COVERAGE_DIR%"

rem ========================================
rem BUILD TESTS
rem ========================================

echo.
echo ========== BUILD TESTS ==========

cmake -S "%TEST_DIR%" -B "%BUILD_DIR%" ^
    -G Ninja ^
    -DCMAKE_CXX_COMPILER=g++.exe ^
    -DCMAKE_MAKE_PROGRAM=ninja.exe ^
    -DCMAKE_BUILD_TYPE=Debug

if errorlevel 1 goto :error

cmake --build "%BUILD_DIR%"

if errorlevel 1 goto :error

echo [OK] Test executable built

rem ========================================
rem RUN GOOGLE TEST
rem ========================================

echo.
echo ========== GOOGLE TEST ==========

if not exist "%BUILD_DIR%\stabltest.exe" (
    echo [ERROR] stabltest.exe not found.
    goto :error
)

"%BUILD_DIR%\stabltest.exe" ^
    --gtest_output="xml:%TEST_RESULTS%\test_results.xml"

if errorlevel 1 goto :error

echo.
echo [OK] ALL TESTS PASSED

rem ========================================
rem CODE COVERAGE
rem ========================================

echo.
echo ========== CODE COVERAGE ==========

gcovr ^
    --root "%SRC_DIR%" ^
    --object-directory "%BUILD_DIR%" ^
    --filter ".*[/\\](Model|Input|stabl)\.hpp$" ^
    --gcov-executable gcov.exe ^
    --txt ^
    --print-summary

if errorlevel 1 goto :error

rem ========================================
rem HTML REPORT
rem ========================================

gcovr ^
    --root "%SRC_DIR%" ^
    --object-directory "%BUILD_DIR%" ^
    --filter ".*[/\\](Model|Input|stabl)\.hpp$" ^
    --gcov-executable gcov.exe ^
    --html-details "%COVERAGE_DIR%\index.html"

if errorlevel 1 goto :error

rem ========================================
rem XML REPORT
rem ========================================

gcovr ^
    --root "%SRC_DIR%" ^
    --object-directory "%BUILD_DIR%" ^
    --filter ".*[/\\](Model|Input|stabl)\.hpp$" ^
    --gcov-executable gcov.exe ^
    --xml-pretty ^
    --output "%COVERAGE_DIR%\coverage.xml"

if errorlevel 1 goto :error

rem ========================================
rem RESULT
rem ========================================

echo.
echo ========== RESULT ==========

echo [OK] TESTS PASSED
echo [OK] COVERAGE GENERATED

echo.
echo Test report: %TEST_RESULTS%\test_results.xml
echo Coverage report: %COVERAGE_DIR%\index.html

echo.
pause
exit /b 0

rem ========================================
rem ERRORS
rem ========================================

:error_gcc
echo [ERROR] GCC not found.
goto :error

:error_cmake
echo [ERROR] CMake not found.
goto :error

:error_ninja
echo [ERROR] Ninja not found.
goto :error

:error_gcov
echo [ERROR] gcov not found.
goto :error

:error_gcovr
echo [ERROR] gcovr not found.
goto :error

:error
echo.
echo ========== FAILED ==========
echo Build, tests or coverage generation failed.
echo.
pause
exit /b 1
