@echo off
setlocal
cd /d "%~dp0"

echo [1/3] Configuring CMake...
cmake -S . -B build
if errorlevel 1 goto error

echo [2/3] Building...
cmake --build build --config Release
if errorlevel 1 goto error

echo [3/3] Starting the program...
if exist "build\src\Release\lab1.exe" (
    "build\src\Release\lab1.exe"
) else if exist "build\src\lab1.exe" (
    "build\src\lab1.exe"
) else (
    echo Executable file was not found.
    goto error
)

pause
exit /b 0

:error
echo Build or execution failed.
pause
exit /b 1
