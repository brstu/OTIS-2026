@echo off
cd /d "%~dp0"
if not exist build\Release\otis.exe (
    echo Executable not found! Run build.bat first.
    pause
    exit /b 1
)
build\Release\otis.exe
pause
