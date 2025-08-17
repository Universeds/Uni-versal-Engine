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

:: Navigate to build directory
cd build

echo.
echo Configuring project with CMake...
echo Trying to auto-detect best generator...

:: Try different generators in order of preference
cmake .. -G "Visual Studio 17 2022" >nul 2>&1
if %errorlevel% equ 0 (
    echo Using Visual Studio 17 2022 generator
    set "GENERATOR_FOUND=1"
    set "BUILD_CONFIG=Release"
    set "EXE_PATH=Release\UniversalEngine.exe"
) else (
    cmake .. -G "Visual Studio 16 2019" >nul 2>&1
    if %errorlevel% equ 0 (
        echo Using Visual Studio 16 2019 generator
        set "GENERATOR_FOUND=1"
        set "BUILD_CONFIG=Release"
        set "EXE_PATH=Release\UniversalEngine.exe"
    ) else (
        cmake .. -G "MinGW Makefiles" >nul 2>&1
        if %errorlevel% equ 0 (
            echo Using MinGW Makefiles generator
            set "GENERATOR_FOUND=1"
            set "BUILD_CONFIG="
            set "EXE_PATH=UniversalEngine.exe"
        ) else (
            cmake .. >nul 2>&1
            if %errorlevel% equ 0 (
                echo Using default generator
                set "GENERATOR_FOUND=1"
                set "BUILD_CONFIG="
                set "EXE_PATH=UniversalEngine.exe"
            ) else (
                echo ERROR: No suitable generator found
                echo Please install Visual Studio, MinGW, or another compatible build system
                cd ..
                pause
                exit /b 1
            )
        )
    )
)

if not defined GENERATOR_FOUND (
    echo ERROR: CMake configuration failed
    cd ..
    pause
    exit /b 1
)

echo.
echo Building project...
if defined BUILD_CONFIG (
    cmake --build . --config %BUILD_CONFIG%
) else (
    cmake --build .
)

if %errorlevel% neq 0 (
    echo ERROR: Build failed
    cd ..
    pause
    exit /b 1
)

echo.
echo Build successful! Starting Universal Engine...
echo ========================================

:: Check if executable exists
if exist "%EXE_PATH%" (
    "%EXE_PATH%"
) else (
    echo ERROR: UniversalEngine.exe not found
    echo Expected location: %EXE_PATH%
    echo Searching for executable...
    echo Checking Release directory:
    dir Release\*.exe 2>nul
    echo Checking Debug directory:
    dir Debug\*.exe 2>nul
    echo Checking root build directory:
    dir *.exe 2>nul
)

:: Return to root directory
cd ..

echo.
echo ========================================
echo Universal Engine execution completed
echo ========================================
pause
