@echo off
setlocal
cd /d "%~dp0"
echo [1/4] Configure project...
cmake -S . -B build -DBUILD_TESTING=ON -DENABLE_COVERAGE=ON
if errorlevel 1 exit /b 1
echo [2/4] Build project...
cmake --build build --config Debug
if errorlevel 1 exit /b 1
echo [3/4] Run Google Test...
ctest --test-dir build -C Debug --output-on-failure
if errorlevel 1 exit /b 1
echo [4/4] Coverage report...
where gcovr >nul 2>nul
if errorlevel 1 (
  echo gcovr is not installed. Install: pip install gcovr
) else (
  gcovr -r . --filter "src" --exclude "src/main.cpp" --exclude "build/_deps" --html-details coverage.html --xml coverage.xml
)
pause
