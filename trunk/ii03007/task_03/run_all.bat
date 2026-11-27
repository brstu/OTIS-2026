@echo off
chcp 65001 >nul
title ЛР3 ii03007: сборка, тесты, Doxygen
setlocal

where cmake >nul 2>nul
if errorlevel 1 (
    echo.
    echo  *** CMake не найден в PATH! ***
    echo  Установка: winget install -e --id Kitware.CMake
    echo.
    pause
    exit /b 1
)

set "GEN="
where g++ >nul 2>nul || goto gen_ready
where mingw32-make >nul 2>nul || goto gen_ready
set GEN=-G "MinGW Makefiles"
:gen_ready

echo =======================================
echo  [ШАГ 1/3] Сборка проекта и тесты
echo =======================================
cmake -S . -B build %GEN%
if errorlevel 1 goto fail

cmake --build build --config Release
if errorlevel 1 goto fail

pushd build
ctest --output-on-failure > ..\test_report.txt 2>&1
set TESTS=%errorlevel%
popd
type test_report.txt
if not "%TESTS%"=="0" goto fail

echo.
echo =======================================
echo  [ШАГ 2/3] Документация Doxygen
echo =======================================
where doxygen >nul 2>nul
if errorlevel 1 (
    echo  Doxygen не найден в PATH, шаг пропущен.
    echo  Установка: winget install -e --id DimitriVanHeesch.Doxygen
    goto docs_done
)
if not exist doc md doc
pushd ..
doxygen task_03\Doxyfile
set DOX=%errorlevel%
popd
if not "%DOX%"=="0" goto fail
echo  Готово: doc\html\index.html
:docs_done

echo.
echo =======================================
echo  [ШАГ 3/3] Всё выполнено
echo =======================================
echo  отчёт тестов : test_report.txt
echo  документация : doc\html\index.html
echo  график       : python plot.py
pause
exit /b 0

:fail
echo.
echo *** ОШИБКА на шаге выше. Смотрите вывод на экране. ***
pause
exit /b 1