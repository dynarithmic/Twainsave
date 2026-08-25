@echo off
setlocal

echo Using:
cmake --version
echo.

cmake --preset vs2026-x64
if errorlevel 1 exit /b 1

cmake --build --preset vs2026-x64-release -- /m
if errorlevel 1 exit /b 1

cmake --build --preset vs2026-x64-debug -- /m
if errorlevel 1 exit /b 1

endlocal
