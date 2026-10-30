@echo off
chcp 65001 >nul
cd /d "%~dp0"
if not exist "build\bin\lab01.exe" (
    echo [ERROR] Сначала запустите build.bat
    pause
    exit /b 1
)
copy /Y plot.py build\bin\plot.py >nul
pushd build\bin
lab01.exe
popd
pause
