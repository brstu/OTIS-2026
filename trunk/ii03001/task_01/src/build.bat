@echo off
setlocal
cd /d "%~dp0"

echo ========================================
echo Checking CMake...
echo ========================================

where cmake >nul 2>nul
if not errorlevel 1 goto cmake_found

if exist "%ProgramFiles%\CMake\bin\cmake.exe" (
    set "PATH=%ProgramFiles%\CMake\bin;%PATH%"
    goto cmake_found
)
if exist "%ProgramFiles(x86)%\CMake\bin\cmake.exe" (
    set "PATH=%ProgramFiles(x86)%\CMake\bin;%PATH%"
    goto cmake_found
)
if exist "%LocalAppData%\Programs\CMake\bin\cmake.exe" (
    set "PATH=%LocalAppData%\Programs\CMake\bin;%PATH%"
    goto cmake_found
)

echo ERROR: CMake was not found.
echo Install CMake from https://cmake.org/download/
echo During installation, add CMake to the system PATH.
pause
exit /b 1

:cmake_found
echo CMake found:
cmake --version
echo.
echo ========================================
echo Configuring project with CMake...
echo ========================================
cmake -S . -B build
if errorlevel 1 (
    echo CMake configuration failed.
    pause
    exit /b 1
)

echo.
echo ========================================
echo Building project...
echo ========================================
cmake --build build --config Release
if errorlevel 1 (
    echo Build failed.
    echo Install Visual Studio Build Tools with "Desktop development with C++".
    pause
    exit /b 1
)

echo.
echo ========================================
echo Build completed successfully.
echo Starting program...
echo ========================================
if exist "build\Release\task_01.exe" (
    "build\Release\task_01.exe"
) else if exist "build\task_01.exe" (
    "build\task_01.exe"
) else (
    echo Executable was not found.
    pause
    exit /b 1
)

echo.
echo ========================================
echo Program finished. Creating graphs...
echo ========================================

where python >nul 2>nul
if not errorlevel 1 (
    python -c "import matplotlib" >nul 2>nul
    if errorlevel 1 (
        echo Matplotlib is not installed. Installing it now...
        python -m pip install matplotlib
    )
    python plot_results.py
    if errorlevel 1 (
        echo Graph generation failed. Check Python and Matplotlib installation.
    )
    goto finished
)

where py >nul 2>nul
if not errorlevel 1 (
    py -3 -c "import matplotlib" >nul 2>nul
    if errorlevel 1 (
        echo Matplotlib is not installed. Installing it now...
        py -3 -m pip install matplotlib
    )
    py -3 plot_results.py
    if errorlevel 1 (
        echo Graph generation failed. Check Python and Matplotlib installation.
    )
    goto finished
)

echo Python was not found, so graphs could not be generated.
echo Install Python from https://www.python.org/downloads/
echo During installation, select "Add Python to PATH".

:finished
echo.
echo ========================================
echo All tasks finished.
echo CSV results and graph PNG files are in:
echo %CD%
echo ========================================
pause
