@echo off
if not exist build mkdir build
cd build
cmake -S .. -B . -G "Visual Studio 17 2022" -DCMAKE_BUILD_TYPE=Debug
if %errorlevel% neq 0 (
    echo CMake configuration failed
    exit /b %errorlevel%
)
echo CMake configuration successful
pause