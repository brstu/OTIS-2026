@echo off
chcp 65001 > nul
cd /d "%~dp0"

echo ===== [1/4] CMake configure =====
cmake -S . -B build
if errorlevel 1 goto error

echo ===== [2/4] CMake build =====
cmake --build build --config Release
if errorlevel 1 goto error

set "EXE="
if exist "build\Release\ii03023_lab1.exe" set "EXE=build\Release\ii03023_lab1.exe"
if exist "build\Debug\ii03023_lab1.exe" set "EXE=build\Debug\ii03023_lab1.exe"
if exist "build\ii03023_lab1.exe" set "EXE=build\ii03023_lab1.exe"
if not defined EXE goto notfound
echo Program: %EXE%

:run
echo.
echo ===== [3/4] Run program =====
if exist results.csv del results.csv
"%EXE%"
if not exist results.csv goto noresults

echo.
echo ===== [4/4] Plot (Python + matplotlib) =====
python plot.py
goto ask

:noresults
echo results.csv was not created, skipping plot.

:ask
echo.
set "AGAIN="
set /p AGAIN="Run again with another model/input? (y/n): "
if /i "%AGAIN%"=="y" goto run
echo ===== DONE =====
exit /b 0

:notfound
echo Program ii03023_lab1.exe was not found after build.
echo Check that CMakeLists.txt contains: add_executable(ii03023_lab1 main.cpp models.h)
goto error

:error
echo BUILD FAILED
pause
exit /b 1
