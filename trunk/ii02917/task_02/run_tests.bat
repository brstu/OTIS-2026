@echo off
chcp 65001 >nul
echo ==========================================
echo COMPILING PROJECT AND RUNNING TESTS:
echo ==========================================

if not exist build mkdir build
cd build

cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .

echo.
echo ==========================================
echo RUNNING GOOGLE TESTS:
echo ==========================================

REM Запускаем скомпилированный файл тестов напрямую для получения подробного лога [ RUN ] / [ OK ]
unit_tests.exe

echo.
echo ==========================================
echo GENERATING CODE COVERAGE REPORT:
echo ==========================================

python -m gcovr -r .. --object-directory=. --print-summary --exclude ".*main\.cpp" --exclude ".*_deps.*"

cd ..
pause