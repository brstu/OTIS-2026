@echo off
chcp 65001 >nul
cd /d "%~dp0"


set "VENV_DIR="

set "PYTHON=python"
if defined VENV_DIR (
    if exist "%VENV_DIR%\Scripts\python.exe" set "PYTHON=%VENV_DIR%\Scripts\python.exe"
) else (

    for %%D in (".venv" "venv" "env") do (
        for %%P in ("%~dp0%%~D" "%~dp0..\%%~D" "%~dp0..\..\%%~D" "%~dp0..\..\..\%%~D") do (
            if exist "%%~P\Scripts\python.exe" set "PYTHON=%%~P\Scripts\python.exe"
        )
    )
)
echo Python: %PYTHON%

set "OUT_DIR=%~dp0doc"
if not exist "%OUT_DIR%" mkdir "%OUT_DIR%"
echo Output folder: %OUT_DIR%
echo.

echo ===== 1. CMake configure =====
cmake -S src -B build
if errorlevel 1 goto error

echo.
echo ===== 2. Build =====
cmake --build build --config Release
if errorlevel 1 goto error

echo.
echo ===== 3. Run program =====
build\bin\ii02904_task_1.exe "%OUT_DIR%"
if errorlevel 1 goto error

echo.
echo ===== 4. Plot graphs =====
"%PYTHON%" src\plot.py "%OUT_DIR%"
if errorlevel 1 goto error

echo.
echo Done!
pause
exit /b 0

:error
echo.
echo ERROR: step failed.
pause
exit /b 1
