@echo off
cd /d "%~dp0"
if exist build\mingw\bin\task02.exe (
    pushd build\mingw\bin
) else if exist build\vs\bin\task02.exe (
    pushd build\vs\bin
) else (
    echo [ERROR] task02.exe not found. Run run_tests.bat first - it builds the program.
    pause
    exit /b 1
)
task02.exe
popd
pause
