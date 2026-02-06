<#
.SYNOPSIS
    Build ExoSuite shell (main app).

.DESCRIPTION
    Builds the ExoSuite shell to Bin/Release/ExoSuite.exe with progress display.

.EXAMPLE
    .\build\Build-Shell.ps1
#>

param(
    [switch]$Release
)

$ErrorActionPreference = 'Stop'
$RepoRoot = Split-Path $PSScriptRoot -Parent

# Initialize ExoKit only if Rust toolchain is present
$ExoKitRust = Join-Path $RepoRoot "exokit\rust\bin\cargo.exe"
if (Test-Path $ExoKitRust) {
    $ExoKitInit = Join-Path $RepoRoot "exokit\Init-ExoKit.ps1"
    . $ExoKitInit -Quiet
}

# Find cargo (ExoKit or system)
$CargoPath = $null
if (Test-Path $ExoKitRust) {
    $CargoPath = $ExoKitRust
}
else {
    # Fall back to system cargo
    $SystemCargo = Join-Path $env:USERPROFILE ".cargo\bin\cargo.exe"
    if (Test-Path $SystemCargo) {
        $CargoPath = $SystemCargo
    }
}

if (-not $CargoPath) {
    Write-Host "Error: cargo not found. Please install Rust or populate ExoKit." -ForegroundColor Red
    exit 1
}

Write-Host "[1/2] Compiling ExoSuite shell..." -ForegroundColor Cyan

# Run cargo build
$cargoArgs = @("build", "-p", "exosuite")
if ($Release) { $cargoArgs += "--release" }

& $CargoPath @cargoArgs
if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed!" -ForegroundColor Red
    exit 1
}

# Copy to Bin/Release if release build
if ($Release) {
    $OutputDir = Join-Path $RepoRoot "Bin\Release"
    $TargetExe = Join-Path $RepoRoot "target\release\ExoSuite.exe"
    
    if (-not (Test-Path $OutputDir)) {
        New-Item -ItemType Directory -Force -Path $OutputDir | Out-Null
    }
    
    Copy-Item $TargetExe -Destination $OutputDir -Force
    Write-Host "[2/2] Done! Output: Bin\Release\ExoSuite.exe" -ForegroundColor Green
}
else {
    Write-Host "[2/2] Done! Output: target\debug\ExoSuite.exe" -ForegroundColor Green
}
