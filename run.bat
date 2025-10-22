@echo off
echo ========================================
echo Universal Engine - Build and Run Script
echo ========================================

:: Check if cmake is available
cmake --version >nul 2>&1
if %errorlevel% neq 0 (
    echo ERROR: CMake is not installed or not in PATH
    echo Please install CMake and add it to your system PATH
    pause
    exit /b 1
)

:: Create build directory if it doesn't exist
if not exist "build" (
    echo Creating build directory...
    mkdir build
)

echo.
echo Configuring project with CMake...
echo Using Visual Studio 2022 generator...

:: Clean build directory for fresh configuration
if exist "build\CMakeCache.txt" (
    echo Cleaning previous CMake cache...
    del /Q build\CMakeCache.txt
)

:: Configure with Visual Studio 2022 and vcpkg toolchain
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_TOOLCHAIN_FILE="%CD%\vcpkg\scripts\buildsystems\vcpkg.cmake"
if %errorlevel% neq 0 (
    echo ERROR: CMake configuration failed
    echo Please ensure Visual Studio 2022 with C++ tools is installed
    pause
    exit /b 1
)

echo.
echo Building project...
cmake --build build --config Release
if %errorlevel% neq 0 (
    echo ERROR: Build failed
    pause
    exit /b 1
)

echo.
echo Build successful! Starting Universal Engine...
echo ========================================

:: Check if executable exists and run it
if exist "build\Release\UniversalEngine.exe" (
    build\Release\UniversalEngine.exe
) else (
    echo ERROR: UniversalEngine.exe not found
    echo Expected location: build\Release\UniversalEngine.exe
    pause
    exit /b 1
)

echo.
echo ========================================
echo Universal Engine execution completed
echo ========================================
pause
