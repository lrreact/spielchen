@echo off
REM Arena Shooter Client Launch Script

set SCRIPT_DIR=%~dp0
set PROJECT_DIR=%SCRIPT_DIR%..
set BUILD_DIR=%PROJECT_DIR%\build

REM Try Release first, then Debug
if exist "%BUILD_DIR%\Release\arena-shooter.exe" (
    cd /d "%BUILD_DIR%\Release"
    arena-shooter.exe %*
) else if exist "%BUILD_DIR%\Debug\arena-shooter.exe" (
    cd /d "%BUILD_DIR%\Debug"
    arena-shooter.exe %*
) else (
    echo Executable not found. Building...
    call "%SCRIPT_DIR%\build.bat"
    if exist "%BUILD_DIR%\Release\arena-shooter.exe" (
        cd /d "%BUILD_DIR%\Release"
        arena-shooter.exe %*
    )
)
