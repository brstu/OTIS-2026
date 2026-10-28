@echo off
chcp 65001 >nul
setlocal

echo ============================================
echo    Сборка проекта OTIS Lab 1 (Variant 15)
echo ============================================
echo.

:: Переход в папку со скриптом
cd /d "%~dp0"

:: Проверка наличия CMakeLists.txt (исправлено расширение)
if not exist "CMakeLists.txt" (
    echo [ERROR] Файл CMakeLists.txt не найден в папке: %~dp0
    pause
    exit /b 1
)

:: Удаление старой папки build для чистой сборки
if exist "build" (
    echo [INFO] Удаление старой папки build...
    rmdir /s /q build
)

:: Шаг 1: Конфигурация CMake
echo [1/3] Конфигурация проекта...
cmake -S . -B build -G "MinGW Makefiles"
if errorlevel 1 (
    echo [ERROR] Ошибка конфигурации CMake! Убедитесь, что установлены CMake и MinGW (g++ / gcc).
    pause
    exit /b 1
)

:: Шаг 2: Сборка проекта
echo [2/3] Компиляция проекта...
cmake --build build
if errorlevel 1 (
    echo [ERROR] Ошибка компиляции! Проверьте исходный код проекта.
    pause
    exit /b 1
)

:: Шаг 3: Копирование exe в корень проекта
echo [3/3] Копирование исполняемого файла...
if exist "build\lab01.exe" (
    copy /Y "build\lab01.exe" ".\lab01.exe" >nul
    echo [OK] Исполняемый файл успешно создан: lab01.exe
) else if exist "build\Release\lab01.exe" (
    copy /Y "build\Release\lab01.exe" ".\lab01.exe" >nul
    echo [OK] Исполняемый файл успешно создан: lab01.exe
) else (
    echo [WARNING] Исполняемый файл не найден в папке build. Проверьте имя таргета в CMakeLists.txt!
)

echo.
echo ============================================
echo    Сборка завершена успешно!
echo ============================================
echo.
pause
endlocal