@echo off
setlocal
cd /d "%~dp0"

echo =======================================
echo  Compiling project and running tests...
echo =======================================

if not exist build mkdir build
cd build

REM Используем Ninja (он у нас настроен и работает)
cmake .. -G "Ninja" > nul
if errorlevel 1 (
    echo [ERROR] CMake configuration failed.
    pause
    exit /b 1
)

cmake --build .
if errorlevel 1 (
    echo [ERROR] Build failed.
    pause
    exit /b 1
)

echo.
echo =======================================
echo  Running Google Test...
echo =======================================
REM Запускаем exe напрямую, чтобы увидеть детали
.\run_tests.exe

echo.
echo =======================================
echo  Generating Code Coverage...
echo =======================================
REM Выводим таблицу в консоль (дефис означает вывод в поток)
gcovr -r .. --exclude ".*_deps.*" --exclude ".*main\.cpp.*" --txt=-

REM Генерируем HTML-отчет
gcovr -r .. --exclude ".*_deps.*" --exclude ".*main\.cpp.*" --html-details -o coverage.html
echo [OK] Coverage report saved to: build\coverage.html

start chrome "file:///%CD%/coverage.html"

cd ..
pause
endlocal