@echo off
setlocal

set "SCRIPT_DIR=%~dp0"
if "%SCRIPT_DIR:~-1%"=="\" set "SCRIPT_DIR=%SCRIPT_DIR:~0,-1%"

set "TASK_DIR=%SCRIPT_DIR%"
if exist "%SCRIPT_DIR%\trunk\ii03026\task_02\CMakeLists.txt" (
    set "TASK_DIR=%SCRIPT_DIR%\trunk\ii03026\task_02"
)

set "BUILD_DIR=%TASK_DIR%\build_cov"
set "COV_REPORT=%TASK_DIR%\coverage.html"
set "MINGW_BIN=D:\CLion 2025.2.4\bin\mingw\bin"
set "CMAKE_EXE=D:\CLion 2025.2.4\bin\cmake\win\x64\bin\cmake.exe"
set "CTEST_EXE=D:\CLion 2025.2.4\bin\cmake\win\x64\bin\ctest.exe"

set "PATH=%MINGW_BIN%;%PATH%"

cd /d "%TASK_DIR%"
if errorlevel 1 goto :fail

echo [1/4] Configuring CMake...
"%CMAKE_EXE%" -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="--coverage -O0 -g" -DCMAKE_C_FLAGS="--coverage -O0 -g" -DCMAKE_EXE_LINKER_FLAGS="--coverage" -S . -B build_cov
if errorlevel 1 goto :fail

echo [2/4] Building project...
"%CMAKE_EXE%" --build build_cov -j
if errorlevel 1 goto :fail

echo [3/4] Running CTest...
"%CTEST_EXE%" --test-dir build_cov --output-on-failure
if errorlevel 1 goto :fail

echo [4/4] Generating coverage report...
where gcovr >nul 2>&1
if errorlevel 1 (
    python -m gcovr --version >nul 2>&1
    if errorlevel 1 (
        echo [WARN] gcovr not found. Install with: pip install gcovr
        goto :done
    ) else (
        python -m gcovr --root src --filter src --gcov-executable "%MINGW_BIN%\gcov.exe" --html-details -o "%COV_REPORT%" --print-summary build_cov
        if errorlevel 1 goto :fail
    )
) else (
    gcovr --root src --filter src --gcov-executable "%MINGW_BIN%\gcov.exe" --html-details -o "%COV_REPORT%" --print-summary build_cov
    if errorlevel 1 goto :fail
)

:done
echo Done. Report: %COV_REPORT%
pause
exit /b 0

:fail
echo [ERROR] Script failed with code %errorlevel%.
pause
exit /b 1