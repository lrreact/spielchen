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

REM Configure with CMake
echo Configuring with CMake...
cmake -DCMAKE_BUILD_TYPE=%BUILD_TYPE% ..
if errorlevel 1 (
    echo CMake configuration failed!
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
echo Executables:
echo   - %BUILD_DIR%\%BUILD_TYPE%\arena-shooter.exe (client)
echo   - %BUILD_DIR%\%BUILD_TYPE%\arena-shooter-server.exe (server)
echo.
echo Run with:
echo   cd %BUILD_DIR%\%BUILD_TYPE% ^&^& arena-shooter.exe

endlocal
