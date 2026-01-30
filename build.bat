@echo off
setlocal enabledelayedexpansion

echo ========================================
echo         MCPVET Build and Run Script
echo ========================================

:: Check if running in project root directory
if not exist "CMakeLists.txt" (
    echo Error: Please run this script in the project root directory.
    pause
    exit /b 1
)

:: Check if build directory exists
if not exist "build\" (
    echo Build directory does not exist, running CMake configuration...
    cmake -S . -B build -G "MinGW Makefiles"
    if errorlevel 1 (
        echo CMake configuration failed.
        pause
        exit /b 1
    )
    echo CMake configuration succeeded.
) else (
    echo Build directory detected.
)

:: Use PowerShell to get user input for mingw32-make
echo.
echo Execute mingw32-make for build? (y/n)
for /f "delims=" %%a in ('powershell -Command "$response = (Read-Host 'Please enter y or n').Trim().ToLower(); Write-Output $response"') do set "make_choice=%%a"
if /i "!make_choice!" neq "y" (
    echo Skipping build.
    goto :ask_run
)

echo Building...
cd build
mingw32-make
if errorlevel 1 (
    echo Build failed.
    cd ..
    pause
    exit /b 1
)
cd ..
echo Build succeeded.

:ask_run
:: Use PowerShell to get user input for running program
echo.
echo Run program MCPVET? (y/n)
for /f "delims=" %%a in ('powershell -Command "$response = (Read-Host 'Please enter y or n').Trim().ToLower(); Write-Output $response"') do set "run_choice=%%a"
if /i "!run_choice!" neq "y" (
    echo Skipping execution.
    pause
    exit /b 0
)

:: Check if executable exists
if not exist "build\bin\MCPVET.exe" (
    echo Error: Executable not found. Please build first.
    pause
    exit /b 1
)

echo Running program...
cd build\bin
start "" "MCPVET.exe"
cd ..\..

echo Program started.
pause
exit /b 0