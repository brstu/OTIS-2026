@echo off
setlocal
cd /d "%~dp0" || exit /b 1
cmake -S . -B build -DBUILD_TESTING=ON || exit /b 1
cmake --build build --config Debug || exit /b 1
ctest --test-dir build -C Debug --output-on-failure --output-junit "%CD%\build\test-results.xml"
exit /b %errorlevel%
