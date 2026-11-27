@echo off
cd /d "%~dp0"

echo ========================================================
echo  OTIS: Building project and executing unit tests...
echo ========================================================

cd ..
if not exist build mkdir build
cd build

cmake .. -G "MinGW Makefiles" -DENABLE_COVERAGE=ON
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] CMake configuration failed!
    pause
    exit /b %ERRORLEVEL%
)

cmake --build .
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Compilation failed!
    pause
    exit /b %ERRORLEVEL%
)

echo ========================================================
echo  Running Google Test...
echo ========================================================
ctest --output-on-failure

cd ..
if not exist report mkdir report

echo ========================================================
echo  Generating interactive HTML Coverage Report...
echo ========================================================
where gcovr >nul 2>nul
if %ERRORLEVEL% EQU 0 (
    gcovr -r . -f "src/" -e "build/" -e "test/" -e ".*main\.cpp" --exclude-branches-by-pattern ".*" --html --html-details -o report/coverage.html
    echo [SUCCESS] Report generated at: report/coverage.html
    start report/coverage.html
) else (
    echo [WARNING] 'gcovr' not detected in system PATH.
)

cd test
echo ========================================================
echo  Done!
echo ========================================================
pause