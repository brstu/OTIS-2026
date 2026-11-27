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

REM Покрытие включается только если есть и g++, и mingw32-make (MinGW)
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

REM Старые данные покрытия и старые отчёты удаляем, чтобы в отчёт не попало ничего лишнего
if exist build del /s /q build\*.gcda >nul 2>nul
if exist coverage_report.txt del /q coverage_report.txt
if exist coverage_summary.txt del /q coverage_summary.txt
if exist test_report.txt del /q test_report.txt

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
REM Правила отбора файлов (пути в --filter считаются ОТНОСИТЕЛЬНО текущей папки task_02):
REM   src/           - код ЛР №2 (task_02\src)
REM   ../task_01/    - модели ЛР №1 (соседняя папка task_01)
REM Всё остальное (тесты, googletest, build, чужие копии проекта) под фильтры не попадает.
REM main.cpp исключён. Ветки от исключений и недостижимые ветки не считаются.
REM Данные берутся только из build\src (объектные файлы библиотеки core_ii03024).
gcovr -r .. --object-directory build\src ^
      --filter "src/" --filter "[.][.]/task_01/" ^
      --exclude ".*main[.]cpp" ^
      --exclude-unreachable-branches --exclude-throw-branches ^
      --print-summary --txt coverage_report.txt > coverage_summary.txt
echo --- Coverage per file ---
type coverage_report.txt
echo --- Summary (lines / functions / branches) ---
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
