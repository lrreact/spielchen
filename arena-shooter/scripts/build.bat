@echo off
REM Arena Shooter Windows Build Script
REM Usage: build.bat [Debug/Release]

setlocal enabledelayedexpansion

set BUILD_TYPE=%1
if "%BUILD_TYPE%"=="" set BUILD_TYPE=Release

echo ==================================
echo   Arena Shooter Build Script
echo ==================================
echo Build type: %BUILD_TYPE%
echo.

REM Get script directory
set SCRIPT_DIR=%~dp0
set PROJECT_DIR=%SCRIPT_DIR%..

REM Create build directory
set BUILD_DIR=%PROJECT_DIR%\build
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

cd /d "%BUILD_DIR%"

REM Detect Visual Studio version and set generator
set VS_GENERATOR=
where cl >nul 2>&1
if %errorlevel%==0 (
    REM cl.exe is in PATH - use Ninja if available, otherwise NMake
    where ninja >nul 2>&1
    if %errorlevel%==0 (
        set VS_GENERATOR=-G "Ninja"
    ) else (
        set VS_GENERATOR=-G "NMake Makefiles"
    )
) else (
    REM Use Visual Studio generator (auto-detect version)
    REM Try VS 2022 first, then 2019
    if exist "%ProgramFiles%\Microsoft Visual Studio\2022" (
        set VS_GENERATOR=-G "Visual Studio 17 2022" -A x64
    ) else if exist "%ProgramFiles(x86)%\Microsoft Visual Studio\2019" (
        set VS_GENERATOR=-G "Visual Studio 16 2019" -A x64
    ) else if exist "%ProgramFiles%\Microsoft Visual Studio\2019" (
        set VS_GENERATOR=-G "Visual Studio 16 2019" -A x64
    ) else (
        echo Error: Visual Studio 2019 or 2022 not found!
        echo Please install Visual Studio with C++ Desktop Development workload.
        echo.
        echo Or run this script from "Developer Command Prompt for VS"
        exit /b 1
    )
)

REM Configure with CMake
echo Configuring with CMake...
echo Generator: %VS_GENERATOR%
cmake %VS_GENERATOR% -DCMAKE_BUILD_TYPE=%BUILD_TYPE% -DCMAKE_POLICY_VERSION_MINIMUM=3.5 ..
if errorlevel 1 (
    echo.
    echo CMake configuration failed!
    echo.
    echo Make sure you have:
    echo   1. Visual Studio 2019 or 2022 with C++ Desktop Development
    echo   2. CMake 3.14+ installed and in PATH
    echo.
    echo Or try running from "Developer Command Prompt for VS"
    exit /b 1
)

REM Build
echo.
echo Building...
cmake --build . --config %BUILD_TYPE% --parallel
if errorlevel 1 (
    echo Build failed!
    exit /b 1
)

echo.
echo ==================================
echo   Build complete!
echo ==================================
echo.

REM Find executable location (varies by generator)
if exist "%BUILD_DIR%\%BUILD_TYPE%\arena-shooter.exe" (
    set EXE_DIR=%BUILD_DIR%\%BUILD_TYPE%
) else if exist "%BUILD_DIR%\arena-shooter.exe" (
    set EXE_DIR=%BUILD_DIR%
) else (
    set EXE_DIR=%BUILD_DIR%\%BUILD_TYPE%
)

echo Executables location: %EXE_DIR%
echo.
echo Run the game:
echo   %EXE_DIR%\arena-shooter.exe
echo.
echo Run the server:
echo   %EXE_DIR%\arena-shooter-server.exe --port 7777

endlocal
