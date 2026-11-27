@echo off
echo ========================================
echo [1/3] Compiling project and running tests...
echo ========================================

if not exist build mkdir build
cd build

cmake .. -G "MinGW Makefiles"
if %errorlevel% neq 0 goto error

cmake --build . --target unit_tests
if %errorlevel% neq 0 goto error

echo ========================================
echo [2/3] Running Google Test...
echo ========================================
ctest --output-on-failure
if %errorlevel% neq 0 goto error

echo ========================================
echo [3/3] Coverage report (gcovr)...
echo ========================================
cd ..
py -m gcovr --root . --exclude ".*/_deps/.*" --print-summary | findstr /v /b /c:"branches:" > test_report.txt
type test_report.txt

goto end

:error
echo.
echo [ERROR] Build or test execution failed!
cd ..
pause

:end
echo.
echo All tests passed! Report saved to test_report.txt
pause