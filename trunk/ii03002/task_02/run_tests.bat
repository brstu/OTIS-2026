@echo off
chcp 65001 >nul
setlocal

cd /d "%~dp0"

echo ========================================
echo   ii03002 task_02: build and run tests
echo ========================================

REM --- Find GCC: prefer %USERPROFILE%\gcc\bin, then C:\mingw64\bin, then PATH
set "GCC_BIN="
if exist "%USERPROFILE%\gcc\bin\g++.exe" set "GCC_BIN=%USERPROFILE%\gcc\bin"
if not defined GCC_BIN if exist "C:\mingw64\bin\g++.exe" set "GCC_BIN=C:\mingw64\bin"
if defined GCC_BIN set "PATH=%GCC_BIN%;%PATH%"

REM --- Check compiler
set "GXX_EXE="
for /f "delims=" %%i in ('where g++.exe 2^>nul') do if not defined GXX_EXE set "GXX_EXE=%%i"
if not defined GXX_EXE (
    echo ERROR: g++ not found!
    goto error
)
echo Using GCC compiler:
echo   "%GXX_EXE%"
g++ --version

REM --- Find make: make.exe first, then mingw32-make.exe
set "MAKE_EXE="
for /f "delims=" %%i in ('where make.exe 2^>nul') do if not defined MAKE_EXE set "MAKE_EXE=%%i"
if not defined MAKE_EXE (
    for /f "delims=" %%i in ('where mingw32-make.exe 2^>nul') do if not defined MAKE_EXE set "MAKE_EXE=%%i"
)
if not defined MAKE_EXE (
    echo ERROR: make / mingw32-make not found!
    goto error
)
echo Build tool: "%MAKE_EXE%"

REM --- Check CMake
where cmake >nul 2>&1
if errorlevel 1 (
    echo ERROR: cmake not found!
    goto error
)

REM --- Remove old CMake configuration
if exist build (
    echo Removing old build directory...
    rmdir /s /q build
)

echo.
echo [1/3] Configuring CMake...

cmake -S . -B build -G "MinGW Makefiles" ^
    -DCMAKE_MAKE_PROGRAM="%MAKE_EXE%" ^
    -DCMAKE_C_COMPILER="%GXX_EXE:g++.exe=gcc.exe%" ^
    -DCMAKE_CXX_COMPILER="%GXX_EXE%" ^
    -DENABLE_COVERAGE=ON

if errorlevel 1 goto error

echo.
echo [2/3] Building project...

cmake --build build --parallel

if errorlevel 1 goto error

echo.
echo [3/3] Running Google Tests...

ctest --test-dir build --output-on-failure > test_report.txt 2>&1
set "CTEST_RESULT=%errorlevel%"

type test_report.txt

if not "%CTEST_RESULT%"=="0" goto error

echo.
echo [Coverage] Generating line and function coverage...

REM Launch gcovr through Python so its Scripts folder does not need to be in PATH.
py -m gcovr --version >nul 2>&1
if not errorlevel 1 (
    py -m gcovr --root "%CD%\.." ^
        --filter ".*/task_01/src/.*" ^
        --filter ".*/task_02/src/.*" ^
        --exclude ".*/task_02/test/.*" ^
        --txt coverage_report.txt --print-summary
) else (
    python -m gcovr --version >nul 2>&1
    if errorlevel 1 (
        echo WARNING: gcovr Python module was not found.
        echo Try: py -m pip install gcovr
        echo Tests passed, but coverage was not generated.
        goto coverage_done
    )
    python -m gcovr --root "%CD%\.." ^
        --filter ".*/task_01/src/.*" ^
        --filter ".*/task_02/src/.*" ^
        --exclude ".*/task_02/test/.*" ^
        --txt coverage_report.txt --print-summary
)
if errorlevel 1 (
    echo WARNING: gcovr could not read coverage data.
) else (
    echo.
    echo Detailed coverage report saved to coverage_report.txt
    type coverage_report.txt
)
:coverage_done

echo.
echo ================
echo   ALL TESTS PASSED!
echo ========================================

pause
exit /b 0

:error
echo.
echo ========================================
echo   BUILD OR TESTS FAILED!
echo ========================================
pause
exit /b 1
