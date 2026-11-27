@echo off
setlocal
cd /d "%~dp0"
echo ==========================================
echo  Building Task 02 and running Google Tests
echo ==========================================
if not exist build mkdir build
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug
if errorlevel 1 goto failed
cmake --build build --parallel
if errorlevel 1 goto failed
ctest --test-dir build --output-on-failure
if errorlevel 1 goto failed
> test_report.txt echo Google Test run completed successfully.
echo Test report saved to test_report.txt
goto done
:failed
echo Build or tests failed. See output above.
> test_report.txt echo Build or tests failed. See console output.
exit /b 1
:done
pause
endlocal
