<#
.SYNOPSIS
    Build ExoSuite shell (main app).

.DESCRIPTION
    Builds the ExoSuite shell to Bin/Release/ExoSuite.exe with progress display.

.EXAMPLE
    .\exokit\Build-Shell.ps1 -Release
#>

param(
    [switch]$Release
)

$ErrorActionPreference = 'Stop'
$RepoRoot = Split-Path $PSScriptRoot -Parent

# Initialize ExoKit
$ExoKitInit = Join-Path $RepoRoot "exokit\Init-ExoKit.ps1"
if (Test-Path $ExoKitInit) {
    . $ExoKitInit -Quiet
}

$ShellDir = Join-Path $RepoRoot "shell"
if (-not (Test-Path $ShellDir)) {
    Write-Host "Error: shell/ directory not found." -ForegroundColor Red
    exit 1
}

$BuildType = if ($Release) { "Release" } else { "Debug" }
$BuildDir = Join-Path $ShellDir "build"

Write-Host "[1/3] Configuring ExoSuite shell ($BuildType)..." -ForegroundColor Cyan

if (-not (Test-Path $BuildDir)) {
    New-Item -ItemType Directory -Force -Path $BuildDir | Out-Null
}

Push-Location $BuildDir
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=$BuildType -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++
if ($LASTEXITCODE -ne 0) {
    Pop-Location
    Write-Host "CMake configure failed!" -ForegroundColor Red
    exit 1
}

Write-Host "[2/3] Compiling ExoSuite shell..." -ForegroundColor Cyan
ninja
Pop-Location

if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed!" -ForegroundColor Red
    exit 1
}

# Copy to Bin/Release
if ($Release) {
    $OutputDir = Join-Path $RepoRoot "Bin\Release"
    New-Item -ItemType Directory -Force -Path $OutputDir | Out-Null

    $BuiltExe = Get-ChildItem -Path $BuildDir -Filter "ExoSuite.exe" | Select-Object -First 1
    if ($BuiltExe) {
        Copy-Item $BuiltExe.FullName -Destination $OutputDir -Force
        Write-Host "[3/3] Done! Output: Bin\Release\ExoSuite.exe" -ForegroundColor Green
    }
    else {
        Write-Host "[3/3] Warning: ExoSuite.exe not found in build output" -ForegroundColor Yellow
    }
}
else {
    Write-Host "[3/3] Done! Output: shell\build\ExoSuite.exe" -ForegroundColor Green
}
