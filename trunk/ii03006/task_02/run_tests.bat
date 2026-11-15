@echo off
setlocal
cd /d "%~dp0"
echo Configuring and building Task 02...
cmake -S src -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
if errorlevel 1 exit /b 1
cmake --build build
if errorlevel 1 exit /b 1
del /s /q build\CMakeFiles\*.gcda >nul 2>nul
echo Running Google Test...
ctest --test-dir build --output-on-failure
if errorlevel 1 exit /b 1
where gcov >nul 2>nul
if errorlevel 1 (
    echo ERROR: gcov is not installed or not available in PATH.
    exit /b 1
)
echo Generating coverage reports...
if not exist build\coverage mkdir build\coverage
del /q build\coverage\*.gcov >nul 2>nul
gcov -r -b -c build\CMakeFiles\models.dir\models.cpp.gcda > build\coverage\models.gcov.txt
if errorlevel 1 exit /b 1
if exist src\models.cpp.gcov move /y src\models.cpp.gcov build\coverage\models.cpp.gcov >nul
if exist src\models.h.gcov move /y src\models.h.gcov build\coverage\models.h.gcov >nul
gcov -r -b -c build\CMakeFiles\models.dir\stability_analyzer.cpp.gcda > build\coverage\stability_analyzer.gcov.txt
if errorlevel 1 exit /b 1
if exist src\stability_analyzer.cpp.gcov move /y src\stability_analyzer.cpp.gcov build\coverage\stability_analyzer.cpp.gcov >nul
if exist src\models.h.gcov move /y src\models.h.gcov build\coverage\models.h.gcov >nul
echo Tests completed successfully.
endlocal
