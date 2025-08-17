@echo off
echo ========================================
echo Universal Engine - Clean Build Script
echo ========================================

echo Cleaning build directory...

if exist "build" (
    echo Removing existing build directory...
    rmdir /s /q build
    echo Build directory removed.
) else (
    echo Build directory doesn't exist - nothing to clean.
)

echo.
echo Clean completed!
echo Run 'run.bat' to build and run the project.
echo ========================================
pause
