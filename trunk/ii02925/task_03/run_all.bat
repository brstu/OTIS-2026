@echo off
setlocal
cd /d "%~dp0"
echo [1/3] Configuring and building...
cmake -S . -B build -DBUILD_TESTING=ON
if errorlevel 1 exit /b 1
cmake --build build --config Debug
if errorlevel 1 exit /b 1
echo [2/3] Running tests...
ctest --test-dir build -C Debug --output-on-failure
if errorlevel 1 exit /b 1
echo [3/3] Generating Doxygen HTML...
doxygen Doxyfile
if errorlevel 1 exit /b 1
echo Completed successfully.
echo Run the program from build folder to perform an interactive experiment.
pause
