# ==============================================================================
# ExoSuite Build Script (PowerShell)
# ==============================================================================
param(
    [switch]$Release,
    [switch]$Debug,
    [switch]$Clean,
    [switch]$Help
)

$ErrorActionPreference = "Stop"
$Root = $PSScriptRoot
$BuildDir = Join-Path $Root "build"

if ($Help) {
    Write-Host @"
ExoSuite Build Script

Usage: .\build.ps1 [options]

Options:
  -Debug      Build debug configuration (default)
  -Release    Build release configuration
  -Clean      Remove build directory
  -Help       Show this help message
"@
    exit 0
}

if ($Clean) {
    Write-Host "Cleaning build directory..."
    if (Test-Path $BuildDir) {
        Remove-Item -Recurse -Force $BuildDir
    }
    Write-Host "Done." -ForegroundColor Green
    exit 0
}

# Determine configuration
$Config = if ($Release) { "Release" } else { "Debug" }
$Preset = if ($Release) { "release" } else { "debug" }

Write-Host "==============================================================================" -ForegroundColor Cyan
Write-Host "ExoSuite Build - $Config" -ForegroundColor Cyan
Write-Host "==============================================================================" -ForegroundColor Cyan

# Configure
Write-Host "`n[1/2] Configuring with preset: $Preset" -ForegroundColor Yellow
cmake --preset $Preset
if ($LASTEXITCODE -ne 0) {
    Write-Host "Configuration failed!" -ForegroundColor Red
    exit 1
}

# Build - use preset build directory
$PresetBuildDir = Join-Path $BuildDir $Preset
Write-Host "`n[2/2] Building $Config..." -ForegroundColor Yellow
cmake --build $PresetBuildDir --config $Config
if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed!" -ForegroundColor Red
    exit 1
}

Write-Host "`n==============================================================================" -ForegroundColor Green
Write-Host "Build complete: $BuildDir\bin\$Config" -ForegroundColor Green
Write-Host "==============================================================================" -ForegroundColor Green
