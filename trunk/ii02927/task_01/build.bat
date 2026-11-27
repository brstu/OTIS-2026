@echo off 
setlocal 
cd /d "%%~dp0" 
if not exist "src\build" mkdir "src\build" 
cd src\build 
cmake .. 
cmake --build . --config Release 
pause 
