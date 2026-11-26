@echo off
setlocal

set "TASK02_DIR=%~dp0"
set "BUILD_DIR=%TASK02_DIR%build_local"

echo ==========================================
echo CONFIGURING AND COMPILING TESTS:
echo ==========================================
cmake -S "%TASK02_DIR%src" -B "%BUILD_DIR%"
if errorlevel 1 exit /b 1

cmake --build "%BUILD_DIR%" --target ii02917_task02_tests --config Release
if errorlevel 1 exit /b 1

echo ==========================================
echo RUNNING GOOGLE TESTS:
echo ==========================================
if exist "%BUILD_DIR%\Release\ii02917_task02_tests.exe" (
  "%BUILD_DIR%\Release\ii02917_task02_tests.exe"
) else if exist "%BUILD_DIR%\ii02917_task02_tests.exe" (
  "%BUILD_DIR%\ii02917_task02_tests.exe"
) else (
  ctest --test-dir "%BUILD_DIR%" -C Release --output-on-failure
)
if errorlevel 1 exit /b 1

echo ==========================================
echo DONE
echo ==========================================
endlocal