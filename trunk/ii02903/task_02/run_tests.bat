@echo off
cd /d "%~dp0"

echo =======================================
echo Task 02: build, unit tests, coverage
echo =======================================

where cmake >nul 2>nul
if errorlevel 1 (
    echo [ERROR] CMake not found. Install CMake and tick "Add CMake to the system PATH".
    pause
    exit /b 1
)

rem If MinGW g++ is installed use it, otherwise let CMake pick Visual Studio.
rem Coverage is measured only with MinGW (gcc) and gcovr; without them the tests still run.
set GEN=
set BDIR=build\vs
set COV=OFF
where g++ >nul 2>nul
if not errorlevel 1 (
    set GEN=-G "MinGW Makefiles"
    set BDIR=build\mingw
    where gcovr >nul 2>nul
    if not errorlevel 1 set COV=ON
)
set REPORT=%BDIR%\test_report.txt

echo.
echo Configuring (the first run downloads Google Test, the internet is needed)
echo ---------------------------------------------------------------------------
cmake -S . -B %BDIR% %GEN% -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=%COV%
if errorlevel 1 (
    echo.
    echo [ERROR] Configuration failed! Is a C++ compiler installed and is the internet on?
    pause
    exit /b 1
)

echo.
echo Building project and tests
echo --------------------------
cmake --build %BDIR% --config Debug
if errorlevel 1 (
    echo.
    echo [ERROR] Build failed!
    pause
    exit /b 1
)

echo.
echo =======================================
echo Running Google Test
echo =======================================
pushd %BDIR%
ctest -C Debug --output-on-failure > test_report.txt 2>&1
set TESTS_RESULT=%errorlevel%
popd
type "%REPORT%"
echo.
echo Test report saved to %REPORT%

if "%COV%"=="ON" (
    echo.
    echo =======================================
    echo Code coverage ^(gcovr^)
    echo =======================================
    gcovr -r . --object-directory %BDIR% --filter "src/" --exclude "src/main.cpp" --exclude-throw-branches --txt %BDIR%\coverage.txt --html-details %BDIR%\coverage.html
    type %BDIR%\coverage.txt
    echo.
    echo Coverage report: %BDIR%\coverage.txt and %BDIR%\coverage.html
) else (
    echo.
    echo [INFO] Coverage skipped: it needs MinGW ^(g++^) and gcovr ^(pip install gcovr^).
)

echo.
if %TESTS_RESULT%==0 (
    echo [OK] All tests passed.
) else (
    echo [FAIL] Some tests failed, see the report above.
)
pause
exit /b %TESTS_RESULT%
