@echo off
chcp 65001 >nul
setlocal EnableExtensions

rem =====================================================================
rem  Лабораторная работа №2. Вариант 6. Низамутдинов М.А., группа ИИ-30.
rem
rem  Назначение: сборка проекта, запуск модульных тестов Google Test
rem  через ctest и (если доступно) подсчёт покрытия кода.
rem
rem  Использование:
rem      run_tests.bat             - сборка Debug + тесты
rem      run_tests.bat Release     - сборка Release + тесты
rem
rem  Необязательная переменная окружения:
rem      OTIS_GTEST_DIR - путь к распакованным исходникам googletest
rem                       (если нет доступа к GitHub и FetchContent
rem                        не может скачать архив)
rem      OCC            - путь к OpenCppCoverage.exe (если не в PATH)
rem =====================================================================

set "ROOT=%~dp0"
set "BUILD=%ROOT%build"
set "CONFIG=Debug"
if not "%~1"=="" set "CONFIG=%~1"

echo ============================================================
echo  ОТИС-2026 ^| ЛР №2 ^| Вариант 6
echo  Каталог проекта : %ROOT%
echo  Каталог сборки  : %BUILD%
echo  Конфигурация    : %CONFIG%
echo ============================================================
echo.

rem ---------------------------------------------------------------------
rem  Шаг 1. Окружение Visual Studio 2022
rem ---------------------------------------------------------------------
echo [1/4] Поиск окружения Visual Studio 2022...

set "VCVARS="
for %%P in (
    "G:\VISUAL\VC\Auxiliary\Build\vcvars64.bat"
    "%ProgramFiles%\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
    "%ProgramFiles%\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat"
    "%ProgramFiles%\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat"
    "%ProgramFiles%\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
    "%ProgramFiles(x86)%\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
) do (
    if exist %%P if not defined VCVARS set "VCVARS=%%~P"
)

if not defined VCVARS goto :no_msvc
echo       найдено: %VCVARS%

call "%VCVARS%" >nul 2>&1
if errorlevel 1 goto :no_msvc

where cmake >nul 2>&1
if errorlevel 1 goto :no_cmake

echo       cmake  : OK
echo.

rem ---------------------------------------------------------------------
rem  Шаг 2. Конфигурация проекта
rem ---------------------------------------------------------------------
echo [2/4] Конфигурация проекта (CMake)...

set "GTEST_ARG="
if defined OTIS_GTEST_DIR set "GTEST_ARG=-DOTIS_GTEST_SOURCE_DIR=%OTIS_GTEST_DIR%"
if defined OTIS_GTEST_DIR echo       локальный googletest: %OTIS_GTEST_DIR%

cmake -S "%ROOT%." -B "%BUILD%" -G "Visual Studio 17 2022" -A x64 %GTEST_ARG%
if errorlevel 1 goto :cmake_failed
echo.

rem ---------------------------------------------------------------------
rem  Шаг 3. Сборка
rem ---------------------------------------------------------------------
rem  Сборка последовательная: MSBuild с --parallel на этой конфигурации
rem  (MSVC 17.14 + PDB) завершается с кодом 1 без сообщений об ошибках.
echo [3/4] Сборка проекта...
cmake --build "%BUILD%" --config %CONFIG%
if errorlevel 1 goto :build_failed
echo.

rem ---------------------------------------------------------------------
rem  Шаг 4. Запуск тестов
rem ---------------------------------------------------------------------
echo [4/4] Запуск тестов (ctest)...
ctest --test-dir "%BUILD%" -C %CONFIG% --output-on-failure
set "TEST_RESULT=%errorlevel%"
echo.

if not "%TEST_RESULT%"=="0" goto :tests_failed

rem ---------------------------------------------------------------------
rem  Покрытие кода
rem ---------------------------------------------------------------------
echo Подготовка отчёта о покрытии...

where gcovr >nul 2>&1
if errorlevel 1 goto :try_opencppcov

echo       инструмент: gcovr
gcovr --root "%ROOT%." --filter "%ROOT%src" --txt-metric line --print-summary
goto :success

:try_opencppcov
rem  OpenCppCoverage не всегда прописывается в PATH, поэтому кроме "where"
rem  проверяем типовые каталоги установки. Переменная окружения OCC имеет
rem  приоритет: set OCC=<полный путь к OpenCppCoverage.exe>
if defined OCC goto :got_occ

set "OCC="
where OpenCppCoverage >nul 2>&1
if not errorlevel 1 set "OCC=OpenCppCoverage"

if not defined OCC if exist "G:\OpenCppCoverage\OpenCppCoverage.exe" set "OCC=G:\OpenCppCoverage\OpenCppCoverage.exe"
if not defined OCC if exist "%ProgramFiles%\OpenCppCoverage\OpenCppCoverage.exe" set "OCC=%ProgramFiles%\OpenCppCoverage\OpenCppCoverage.exe"
if not defined OCC if exist "%ProgramFiles(x86)%\OpenCppCoverage\OpenCppCoverage.exe" set "OCC=%ProgramFiles(x86)%\OpenCppCoverage\OpenCppCoverage.exe"
if not defined OCC if exist "%LOCALAPPDATA%\Programs\OpenCppCoverage\OpenCppCoverage.exe" set "OCC=%LOCALAPPDATA%\Programs\OpenCppCoverage\OpenCppCoverage.exe"

:got_occ
if not defined OCC goto :no_coverage
echo       инструмент: %OCC%

rem  Бинарники разведены по подкаталогам конфигурации, поэтому берём файл
rem  строго из build\bin\%CONFIG%, иначе легко подхватить чужую сборку.
set "TEST_EXE=%BUILD%\bin\%CONFIG%\otis_tests.exe"
if not exist "%TEST_EXE%" goto :no_coverage

echo       тестовый файл: %TEST_EXE%

set "COV_XML=%BUILD%\coverage.xml"
set "COV_HTML=%BUILD%\coverage"

rem  Запускаем тесты из их каталога: рядом лежит PDB, без которой покрытие
rem  не измеряется.
pushd "%BUILD%\bin\%CONFIG%"
"%OCC%" --quiet --sources "%ROOT%src\*" ^
        --export_type cobertura:"%COV_XML%" ^
        --export_type html:"%COV_HTML%" ^
        -- "%TEST_EXE%"
set "COV_RESULT=%errorlevel%"
popd

if not "%COV_RESULT%"=="0" goto :no_coverage
if not exist "%COV_XML%" goto :no_coverage

rem  Разбор cobertura: текстовая печать процентов.
powershell -NoProfile -Command ^
  "$x=[xml](Get-Content -Encoding UTF8 '%COV_XML%');" ^
  "$c=$x.coverage;" ^
  "$v=[int]$c.'lines-valid';" ^
  "if ($v -eq 0) {" ^
  "  '       ПРИМЕЧАНИЕ: покрытие не измерено - в сборке %CONFIG% нет отладочной';" ^
  "  '                   информации (PDB). Запустите скрипт без аргументов,';" ^
  "  '                   чтобы измерить покрытие на конфигурации Debug.'" ^
  "} else {" ^
  "  '       итого: ' + $c.'lines-covered' + ' из ' + $v + ' строк = ' + [math]::Round(100*[double]$c.'line-rate',1) + '%%';" ^
  "  $x.coverage.packages.package.classes.class | ForEach-Object {" ^
  "    $vv=($_.lines.line | Measure-Object).Count;" ^
  "    $k=($_.lines.line | Where-Object {[int]$_.hits -gt 0} | Measure-Object).Count;" ^
  "    if ($vv -gt 0) { '       ' + (Split-Path $_.filename -Leaf).PadRight(26) + $k + '/' + $vv + ' = ' + [math]::Round(100.0*$k/$vv,1) + '%%' } } }"

echo       отчёт HTML: %COV_HTML%\index.html
goto :success

:no_coverage
echo.
echo ПРИМЕЧАНИЕ: gcovr (работает с GCC/Clang) и OpenCppCoverage
echo             не найдены - шаг покрытия пропущен.
echo             Для MSVC установите OpenCppCoverage:
echo                 winget install OpenCppCoverage.OpenCppCoverage
echo             либо распакуйте портативную версию и повторите запуск,
echo             указав путь через переменную окружения OCC, например:
echo                 set OCC=G:\OpenCppCoverage\OpenCppCoverage.exe

:success
echo.
echo ============================================================
echo  РЕЗУЛЬТАТ: ВСЕ ТЕСТЫ ПРОЙДЕНЫ.
echo  Конфигурация: %CONFIG%
echo  Отчёт о покрытии: см. вывод выше.
echo ============================================================
echo.
echo  Нажмите любую клавишу, чтобы закрыть окно...
pause >nul
endlocal
exit /b 0

rem ---------------------------------------------------------------------
rem  Обработка ошибок
rem ---------------------------------------------------------------------
:no_msvc
echo.
echo ОШИБКА: не найдено окружение Visual Studio 2022 (vcvars64.bat).
echo Установите Visual Studio 2022 с рабочей нагрузкой
echo "Разработка классических приложений на C++".
echo.
echo  Нажмите любую клавишу, чтобы закрыть окно...
pause >nul
endlocal
exit /b 1

:no_cmake
echo.
echo ОШИБКА: cmake не найден в PATH.
echo Установите CMake или добавьте его в PATH.
echo.
echo  Нажмите любую клавишу, чтобы закрыть окно...
pause >nul
endlocal
exit /b 1

:cmake_failed
echo.
echo ОШИБКА: конфигурация CMake завершилась с ошибкой.
echo Если проект ранее собирался другим генератором, удалите каталог:
echo     %BUILD%
echo.
echo  Нажмите любую клавишу, чтобы закрыть окно...
pause >nul
endlocal
exit /b 1

:build_failed
echo.
echo ОШИБКА: сборка завершилась с ошибкой. Смотрите сообщения выше.
echo.
echo  Нажмите любую клавишу, чтобы закрыть окно...
pause >nul
endlocal
exit /b 1

:tests_failed
echo.
echo ============================================================
echo  РЕЗУЛЬТАТ: ЕСТЬ ПРОВАЛЕННЫЕ ТЕСТЫ.
echo  Код возврата ctest: %TEST_RESULT%
echo ============================================================
echo.
echo  Нажмите любую клавишу, чтобы закрыть окно...
pause >nul
endlocal & exit /b %TEST_RESULT%