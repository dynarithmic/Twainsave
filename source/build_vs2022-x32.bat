@echo off
setlocal

echo Using:
cmake --version
echo.

cmake --preset vs2022-x32
if errorlevel 1 exit /b 1

cmake --build --preset vs2022-x32-release -- /m
if errorlevel 1 exit /b 1

cmake --build --preset vs2022-x32-debug -- /m
if errorlevel 1 exit /b 1

endlocal
