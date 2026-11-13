@echo off
setlocal EnableExtensions
cd /d "%~dp0"

echo ==========================================================
echo  ii02914 / task_02 : build, unit tests, coverage
echo ==========================================================
echo.

REM NOTE: this file is intentionally ASCII-only and uses no parentheses
REM inside echo lines or blocks - cmd.exe breaks on them.

REM ---------- 1. Tools ----------
where cmake >nul 2>&1
if errorlevel 1 (
    echo [ERROR] cmake not found in PATH.
    goto fail
)

where g++ >nul 2>&1
if errorlevel 1 (
    echo [ERROR] g++ not found in PATH. Install MinGW-w64 GCC and add its bin folder to PATH.
    goto fail
)

set "GXX="
for /f "delims=" %%I in ('where g++ 2^>nul') do if not defined GXX set "GXX=%%I"
for %%I in ("%GXX%") do set "GCC_BIN=%%~dpI"
if "%GCC_BIN:~-1%"=="\" set "GCC_BIN=%GCC_BIN:~0,-1%"

REM make the selected GCC the first one in PATH, so gcov matches the compiler
set "PATH=%GCC_BIN%;%PATH%"

set "GCC_C=%GCC_BIN%\gcc.exe"

REM gcovr: python -m gcovr, then py -m gcovr, then gcovr
set "GCOVR="
python -m gcovr --version >nul 2>&1
if not errorlevel 1 set "GCOVR=python -m gcovr"
if defined GCOVR goto gcovr_ok
py -m gcovr --version >nul 2>&1
if not errorlevel 1 set "GCOVR=py -m gcovr"
if defined GCOVR goto gcovr_ok
gcovr --version >nul 2>&1
if not errorlevel 1 set "GCOVR=gcovr"
if defined GCOVR goto gcovr_ok
echo [ERROR] gcovr not found. Install it with: python -m pip install gcovr
goto fail
:gcovr_ok

REM build tool: Ninja, otherwise mingw32-make
set "GEN="
where ninja >nul 2>&1
if not errorlevel 1 set "GEN=Ninja"
if defined GEN goto gen_ok
where mingw32-make >nul 2>&1
if not errorlevel 1 set "GEN=MinGW Makefiles"
if defined GEN goto gen_ok
echo [ERROR] No build tool found. Install Ninja: python -m pip install ninja
goto fail
:gen_ok

echo Compiler : "%GXX%"
echo gcovr    : %GCOVR%
echo Generator: %GEN%
echo.

REM ---------- 2. Configure ----------
REM a cache created by another generator breaks cmake - drop it
if not exist "build_tests\CMakeCache.txt" goto configure
findstr /C:"CMAKE_GENERATOR:INTERNAL=%GEN%" "build_tests\CMakeCache.txt" >nul
if errorlevel 1 rmdir /s /q "build_tests"

:configure
echo [1/4] Configuring CMake...
if exist "%GCC_C%" (
    cmake -S . -B build_tests -G "%GEN%" -DCMAKE_CXX_COMPILER="%GXX%" -DCMAKE_C_COMPILER="%GCC_C%" -DCMAKE_BUILD_TYPE=Debug -DOTIS_BUILD_TESTS=ON -DOTIS_ENABLE_COVERAGE=ON
) else (
    cmake -S . -B build_tests -G "%GEN%" -DCMAKE_CXX_COMPILER="%GXX%" -DCMAKE_BUILD_TYPE=Debug -DOTIS_BUILD_TESTS=ON -DOTIS_ENABLE_COVERAGE=ON
)
if errorlevel 1 goto fail

REM ---------- 3. Build ----------
echo.
echo [2/4] Building application and tests...
del /s /q "build_tests\*.gcda" >nul 2>&1
cmake --build build_tests
if errorlevel 1 goto fail

REM ---------- 4. Tests ----------
echo.
echo [3/4] Running Google Test...
del /s /q "build_tests\*.gcda" >nul 2>&1
"build_tests\test\otis_tests.exe" --gtest_output=xml:build_tests\test-report.xml > "build_tests\test-output.txt" 2>&1
set "TEST_RC=%ERRORLEVEL%"
type "build_tests\test-output.txt"

REM ---------- 5. Coverage ----------
echo.
echo [4/4] Generating coverage report...
if not exist "build_tests\coverage" mkdir "build_tests\coverage"
%GCOVR% -r . --gcov-executable gcov --filter "src/" --exclude "src/main.cpp" --html-details "build_tests\coverage\coverage.html" --txt "build_tests\coverage\coverage.txt" build_tests
if errorlevel 1 goto cov_fail
%GCOVR% -r . --gcov-executable gcov --filter "src/" --exclude "src/main.cpp" --print-summary build_tests > "build_tests\coverage\summary.txt"
type "build_tests\coverage\coverage.txt"
type "build_tests\coverage\summary.txt"

echo.
echo ==========================================================
echo  Test summary:
findstr /C:"test suites ran" /C:"PASSED" /C:"FAILED" "build_tests\test-output.txt"
echo.
echo  Reports:
echo    test log     : build_tests\test-output.txt
echo    test xml     : build_tests\test-report.xml
echo    coverage html: build_tests\coverage\coverage.html
echo    coverage text: build_tests\coverage\coverage.txt
echo ==========================================================

if not "%TEST_RC%"=="0" goto test_fail
echo.
echo [SUCCESS] All tests passed, coverage report generated.
if exist "build_tests\coverage\coverage.html" start "" "build_tests\coverage\coverage.html"
set "RC=0"
goto finish

:test_fail
echo.
echo [FAIL] Some tests failed - see build_tests\test-output.txt
set "RC=1"
goto finish

:cov_fail
echo.
echo [ERROR] gcovr failed. Check that gcov from the same GCC is in PATH.
set "RC=1"
goto finish

:fail
echo.
echo [ERROR] Stopped because of an error, see messages above.
set "RC=1"

:finish
echo.
if /i not "%~1"=="nopause" pause
endlocal & exit /b %RC%
