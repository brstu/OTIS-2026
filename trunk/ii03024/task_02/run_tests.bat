@echo off
chcp 65001 >nul
title task_02 - build and run unit tests
setlocal
cd /d "%~dp0"

where cmake >nul 2>nul
if errorlevel 1 (
    echo.
    echo  *** CMake not found in PATH! ***
    echo  Install: winget install -e --id Kitware.CMake
    echo.
    pause
    exit /b 1
)

REM MinGW берём только если есть и g++, и mingw32-make; тогда включается покрытие (gcov)
set "GENERATOR="
set "COV_FLAGS="
where g++ >nul 2>nul
if errorlevel 1 goto no_mingw
where mingw32-make >nul 2>nul
if errorlevel 1 goto no_mingw
set GENERATOR=-G "MinGW Makefiles"
set COV_FLAGS=-DCMAKE_CXX_FLAGS="-g -O0 --coverage"
echo Toolchain: MinGW GCC - coverage enabled
goto toolchain_ok
:no_mingw
echo Toolchain: default generator - coverage will be skipped
:toolchain_ok

echo =======================================
echo  [1/3] Configure and build (Google Test)
echo =======================================
cmake -S . -B build %GENERATOR% %COV_FLAGS%
if errorlevel 1 goto error
cmake --build build --config Release
if errorlevel 1 goto error

echo.
echo =======================================
echo  [2/3] Run tests (ctest)
echo =======================================
pushd build
ctest --output-on-failure -C Release > "..\test_report.txt" 2>&1
set RESULT=%errorlevel%
popd
type test_report.txt

echo.
echo =======================================
echo  [3/3] Coverage report (gcovr)
echo =======================================
if not defined COV_FLAGS goto cov_skip
where gcovr >nul 2>nul
if errorlevel 1 (
    echo gcovr not found. Install: pip install gcovr
    goto cov_done
)
REM покрытие считается для кода ЛР №2 (src) и моделей ЛР №1 (task_01), тесты и main исключены
gcovr -r ..\.. --filter ".*task_02/src/.*" --filter ".*task_01/.*" ^
      --exclude ".*main\.cpp" --exclude ".*build.*" --object-directory build ^
      --print-summary --txt coverage_report.txt > coverage_summary.txt
echo --- Coverage per file ---
type coverage_report.txt
echo --- Summary (lines / functions) ---
type coverage_summary.txt
goto cov_done
:cov_skip
echo Skipped: coverage requires MinGW GCC.
:cov_done

if %RESULT% neq 0 goto error
echo.
echo  All tests passed. Reports: test_report.txt, coverage_report.txt, coverage_summary.txt
pause
exit /b 0

:error
echo.
echo  *** FAILED! See output above. ***
pause
exit /b 1
