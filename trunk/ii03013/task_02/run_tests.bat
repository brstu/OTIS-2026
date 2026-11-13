@echo off
chcp 65001 > nul
setlocal

cd /d "%~dp0"

set BUILD_DIR=build_tests
set TEST_EXE=%BUILD_DIR%\test\task_02_tests.exe
set COVERAGE_DIR=coverage

echo =======================================
echo  Task 02: build, unit tests, coverage
echo =======================================
echo.

rem ---- 0. Check required tools -------------------------------------------
where cmake >nul 2>nul
if errorlevel 1 (
    echo Error: cmake was not found in PATH.
    goto error
)

where g++ >nul 2>nul
if errorlevel 1 (
    echo Error: g++ ^(MinGW-w64^) was not found in PATH.
    goto error
)

where mingw32-make >nul 2>nul
if errorlevel 1 (
    echo Error: mingw32-make was not found in PATH.
    goto error
)

set PYTHON_AVAILABLE=1
where python >nul 2>nul
if errorlevel 1 (
    set PYTHON_AVAILABLE=0
    echo Warning: python was not found in PATH.
    echo.
)

set GCOVR_AVAILABLE=1
python -m gcovr --version >nul 2>nul
if errorlevel 1 (
    set GCOVR_AVAILABLE=0
    echo Warning: gcovr was not found. Install it with: python -m pip install gcovr
    echo.
)

rem ---- 1. Configure and build --------------------------------------------
echo [1/4] Configuring project ^(coverage instrumentation ON^)...
cmake -S . -B %BUILD_DIR% -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON
if errorlevel 1 goto error

echo.
echo [2/4] Building project and tests...
cmake --build %BUILD_DIR%
if errorlevel 1 goto error

rem ---- 2. Run unit tests --------------------------------------------------
echo.
echo [3/4] Running Google Test...

del /s /q "%BUILD_DIR%\*.gcda" >nul 2>nul

"%TEST_EXE%" --gtest_output=xml:"%BUILD_DIR%\test_report.xml"
set TEST_RESULT=%errorlevel%

echo.
echo ---- CTest summary ----
ctest --test-dir %BUILD_DIR% --output-on-failure
echo.

if not "%TEST_RESULT%"=="0" (
    echo TESTS FAILED. See the output above.
) else (
    echo All tests passed.
)

if not exist "%COVERAGE_DIR%" mkdir "%COVERAGE_DIR%"

rem ---- 3. Coverage report (gcovr) ----------------------------------------
if "%GCOVR_AVAILABLE%"=="0" goto skip_coverage

echo.
echo [4/4] Generating coverage report ^(gcovr^)...

python -m gcovr -r . --object-directory "%BUILD_DIR%" ^
    --filter "src/" --exclude "src/main\.cpp" ^
    --print-summary ^
    --txt "%COVERAGE_DIR%\coverage.txt" ^
    --html-details "%COVERAGE_DIR%\index.html" ^
    --json-summary "%COVERAGE_DIR%\summary.json"
if errorlevel 1 (
    echo Warning: gcovr encountered an error.
)

:skip_coverage

rem ---- 4. Generating Google Test HTML Report ------------------------------
if "%PYTHON_AVAILABLE%"=="1" (
    echo.
    echo Generating Google Test HTML report with coverage...
    python gtest_to_html.py "%BUILD_DIR%\test_report.xml" "%COVERAGE_DIR%\gtest_report.html" "%COVERAGE_DIR%\summary.json"
)

:finish
echo.
echo Открытие отчета в браузере...
if exist "%COVERAGE_DIR%\gtest_report.html" start "" "%COVERAGE_DIR%\gtest_report.html"

pause
exit /b %TEST_RESULT%

:error
echo.
echo Build or report generation failed. Check the messages above.
pause
exit /b 1