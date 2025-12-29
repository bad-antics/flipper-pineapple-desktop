@echo off
setlocal enabledelayedexpansion

echo.
echo ================================================
echo  Flipper-Pineapple Manager - C Desktop Build
echo ================================================
echo.

set "PROJECT_DIR=%~dp0"
set "BUILD_DIR=%PROJECT_DIR%build"
set "MINGW_DIR=%PROJECT_DIR%..\mingw64"

REM Check if MinGW exists, if not try to download it
if not exist "%MINGW_DIR%\bin\gcc.exe" (
    echo MinGW not found at %MINGW_DIR%
    echo Attempting to download and extract MinGW...
    
    set "MINGW_ZIP=%PROJECT_DIR%..\mingw-download.zip"
    
    if not exist "!MINGW_ZIP!" (
        echo Downloading MinGW ^(this may take several minutes^)...
        powershell -NoProfile -Command "Invoke-WebRequest -Uri 'https://github.com/brechtsanders/winlibs_mingw/releases/download/15.2.0posix-13.0.0-msvcrt-r5/winlibs-x86_64-posix-seh-gcc-15.2.0-mingw-w64msvcrt-13.0.0-r5.zip' -OutFile '!MINGW_ZIP!' -UseBasicParsing"
        
        if !ERRORLEVEL! neq 0 (
            echo Download failed. Continuing without MinGW...
        )
    )
    
    if exist "!MINGW_ZIP!" (
        echo Extracting MinGW...
        powershell -NoProfile -Command "Expand-Archive '!MINGW_ZIP!' -DestinationPath '%PROJECT_DIR%..' -Force"
    )
)

REM Clean build directory
if exist "%BUILD_DIR%" (
    echo Cleaning old build...
    rmdir /s /q "%BUILD_DIR%" 2>nul
)

REM Configure with CMake using bundled tools in mingw64
if exist "%MINGW_DIR%\bin\cmake.exe" (
    echo Found MinGW with bundled CMake and Ninja at %MINGW_DIR%
    echo Configuring with MinGW toolchain...
    "%MINGW_DIR%\bin\cmake.exe" -S . -B "%BUILD_DIR%" -G "Ninja" -DCMAKE_C_COMPILER="%MINGW_DIR%\bin\gcc.exe" -DCMAKE_MAKE_PROGRAM="%MINGW_DIR%\bin\ninja.exe"
) else (
    echo CMake not found in MinGW. Trying external CMake...
    if exist "C:\Program Files\CMake\bin\cmake.exe" (
        "C:\Program Files\CMake\bin\cmake.exe" -S . -B "%BUILD_DIR%" -G "Ninja" -DCMAKE_C_COMPILER="%MINGW_DIR%\bin\gcc.exe" -DCMAKE_MAKE_PROGRAM="%MINGW_DIR%\bin\ninja.exe"
    ) else (
        echo CMake not found. Build cannot continue.
        pause
        exit /b 1
    )
)

if !ERRORLEVEL! neq 0 (
    echo CMake configuration failed.
    pause
    exit /b 1
)

echo Building project...
if exist "%MINGW_DIR%\bin\cmake.exe" (
    "%MINGW_DIR%\bin\cmake.exe" --build "%BUILD_DIR%" --config Release
) else (
    "C:\Program Files\CMake\bin\cmake.exe" --build "%BUILD_DIR%" --config Release
)

if !ERRORLEVEL! neq 0 (
    echo Build failed.
    pause
    exit /b 1
)

echo.
echo ================================================
echo  Build successful!
echo ================================================
echo.

if exist "%BUILD_DIR%\FlipperPineappleDesktopC.exe" (
    echo Launching application...
    start "" "%BUILD_DIR%\FlipperPineappleDesktopC.exe"
) else (
    echo.
    echo Build directory contents:
    dir "%BUILD_DIR%"
)

pause
