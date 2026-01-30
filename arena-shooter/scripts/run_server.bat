@echo off
REM Arena Shooter Dedicated Server Launch Script

set SCRIPT_DIR=%~dp0
set PROJECT_DIR=%SCRIPT_DIR%..
set BUILD_DIR=%PROJECT_DIR%\build

set PORT=%1
set MAX_PLAYERS=%2
if "%PORT%"=="" set PORT=7777
if "%MAX_PLAYERS%"=="" set MAX_PLAYERS=16

REM Try Release first, then Debug
if exist "%BUILD_DIR%\Release\arena-shooter-server.exe" (
    cd /d "%BUILD_DIR%\Release"
    arena-shooter-server.exe --port %PORT% --max-players %MAX_PLAYERS%
) else if exist "%BUILD_DIR%\Debug\arena-shooter-server.exe" (
    cd /d "%BUILD_DIR%\Debug"
    arena-shooter-server.exe --port %PORT% --max-players %MAX_PLAYERS%
) else (
    echo Server executable not found. Building...
    call "%SCRIPT_DIR%\build.bat"
    if exist "%BUILD_DIR%\Release\arena-shooter-server.exe" (
        cd /d "%BUILD_DIR%\Release"
        arena-shooter-server.exe --port %PORT% --max-players %MAX_PLAYERS%
    )
)
