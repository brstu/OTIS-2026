@echo off
REM ============================================================
REM  Автоматическая сборка проекта ii02914 / task_01
REM  Использование: build.bat
REM  Результат: собранный exe внутри build/
REM ============================================================

setlocal
cd /d "%~dp0"

echo ============================================
echo  Building ii02914 / task_01
echo ============================================
echo.

REM --- Проверка CMake в PATH ---
where cmake >nul 2>nul
if errorlevel 1 (
    if exist "%ProgramFiles%\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" (
        call "%ProgramFiles%\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"
    ) else if exist "%ProgramFiles%\Microsoft Visual Studio\2022\Professional\Common7\Tools\VsDevCmd.bat" (
        call "%ProgramFiles%\Microsoft Visual Studio\2022\Professional\Common7\Tools\VsDevCmd.bat"
    ) else if exist "%ProgramFiles%\Microsoft Visual Studio\2022\Enterprise\Common7\Tools\VsDevCmd.bat" (
        call "%ProgramFiles%\Microsoft Visual Studio\2022\Enterprise\Common7\Tools\VsDevCmd.bat"
    )
)

where cmake >nul 2>nul
if errorlevel 1 (
    echo [ERROR] CMake not found. Please install CMake or configure PATH.
    pause
    exit /b 1
)

if not exist "build" mkdir build
cd build

echo [1/2] Configuring with CMake...
cmake ..
if errorlevel 1 (
    echo [ERROR] CMake configuration failed.
    pause
    exit /b 1
)

echo.
echo [2/2] Building...
cmake --build . --config Release
if errorlevel 1 (
    echo [ERROR] Build failed.
    pause
    exit /b 1
)

echo.
echo ============================================
echo  Build succeeded!
echo ============================================
pause

endlocal