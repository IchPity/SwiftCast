@echo off
setlocal EnableDelayedExpansion

echo ===================================================
echo  SwiftCast Installer Builder
echo ===================================================

:: Paths - adjust if needed
set QT_DIR=C:\Qt\6.5.0\msvc2019_64
set CMAKE_BUILD=..\build
set NSIS_DIR=C:\Program Files (x86)\NSIS

:: 1. Build release
echo [1/4] Building SwiftCast (Release)...
cmake -B "%CMAKE_BUILD%" -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="%QT_DIR%"
cmake --build "%CMAKE_BUILD%" --config Release
if !ERRORLEVEL! NEQ 0 (
    echo ERROR: Build failed.
    exit /b 1
)

:: 2. Deploy Qt DLLs
echo [2/4] Deploying Qt runtime...
"%QT_DIR%\bin\windeployqt.exe" --release --no-translations ^
    "%CMAKE_BUILD%\Release\SwiftCast.exe"
if !ERRORLEVEL! NEQ 0 (
    echo ERROR: windeployqt failed.
    exit /b 1
)

:: 3. Copy resources
echo [3/4] Copying resources...
xcopy /E /Y "..\resources\styles" "%CMAKE_BUILD%\Release\resources\styles\"

:: 4. Build installer
echo [4/4] Building NSIS installer...
"%NSIS_DIR%\makensis.exe" swiftcast.nsi
if !ERRORLEVEL! NEQ 0 (
    echo ERROR: NSIS build failed.
    exit /b 1
)

echo.
echo ===================================================
echo  Done! Installer: SwiftCast-0.1.0-Setup.exe
echo ===================================================
