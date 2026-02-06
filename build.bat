@echo off
REM ==============================================================================
REM ExoSuite Build Script (Windows Batch)
REM ==============================================================================
setlocal EnableDelayedExpansion

set "ROOT=%~dp0"
set "BUILD_DIR=%ROOT%build"
set "PRESET=default"
set "CONFIG=Debug"

REM Parse arguments
:parse_args
if "%~1"=="" goto :start_build
if /I "%~1"=="--release" set "CONFIG=Release" & set "PRESET=release"
if /I "%~1"=="--debug" set "CONFIG=Debug" & set "PRESET=debug"
if /I "%~1"=="--clean" goto :clean
if /I "%~1"=="--help" goto :show_help
shift
goto :parse_args

:show_help
echo ExoSuite Build Script
echo.
echo Usage: build.bat [options]
echo.
echo Options:
echo   --debug     Build debug configuration (default)
echo   --release   Build release configuration
echo   --clean     Remove build directory
echo   --help      Show this help message
exit /b 0

:clean
echo Cleaning build directory...
if exist "%BUILD_DIR%" rmdir /s /q "%BUILD_DIR%"
echo Done.
exit /b 0

:start_build
echo ==============================================================================
echo ExoSuite Build - %CONFIG%
echo ==============================================================================

REM Configure
echo [1/2] Configuring with preset: %PRESET%
cmake --preset %PRESET%
if errorlevel 1 (
    echo Configuration failed!
    exit /b 1
)

REM Build
echo [2/2] Building %CONFIG%...
cmake --build "%BUILD_DIR%" --config %CONFIG%
if errorlevel 1 (
    echo Build failed!
    exit /b 1
)

echo ==============================================================================
echo Build complete: %BUILD_DIR%\bin\%CONFIG%
echo ==============================================================================
