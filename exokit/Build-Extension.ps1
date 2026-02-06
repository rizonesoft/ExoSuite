<#
.SYNOPSIS
    Build a single extension.

.DESCRIPTION
    Builds a specific extension to Bin/Release/System/.

.PARAMETER Name
    Name of the extension (folder name in extensions/).

.EXAMPLE
    .\exokit\Build-Extension.ps1 -Name regstudio
#>

param(
    [Parameter(Mandatory = $true)]
    [string]$Name,
    [switch]$Release
)

$ErrorActionPreference = 'Stop'
$RepoRoot = Split-Path $PSScriptRoot -Parent

# Initialize ExoKit if available
$ExoKitInit = Join-Path $RepoRoot "exokit\Init-ExoKit.ps1"
if (Test-Path $ExoKitInit) {
    . $ExoKitInit -Quiet
}

$ExtPath = Join-Path $RepoRoot "extensions\$Name"
if (-not (Test-Path $ExtPath)) {
    Write-Host "Extension not found: $Name" -ForegroundColor Red
    exit 1
}

$SystemDir = Join-Path $RepoRoot "Bin\Release\System"
New-Item -ItemType Directory -Force -Path $SystemDir | Out-Null

Write-Host "[1/2] Compiling $Name..." -ForegroundColor Cyan

$CmakeLists = Join-Path $ExtPath "CMakeLists.txt"
$CargoToml = Join-Path $ExtPath "Cargo.toml"

if (Test-Path $CargoToml) {
    # Rust extension
    $cargoArgs = @("build", "-p", $Name)
    if ($Release) { $cargoArgs += "--release" }
    & cargo @cargoArgs
    
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Build failed!" -ForegroundColor Red
        exit 1
    }
    
    if ($Release) {
        $ExtExe = Join-Path $RepoRoot "target\release\$Name.exe"
        if (Test-Path $ExtExe) {
            Copy-Item $ExtExe -Destination $SystemDir -Force
        }
    }
}
elseif (Test-Path $CmakeLists) {
    # C++ extension (CMake)
    $BuildDir = Join-Path $ExtPath "build"
    if (-not (Test-Path $BuildDir)) {
        New-Item -ItemType Directory -Force -Path $BuildDir | Out-Null
    }
    
    Push-Location $BuildDir
    cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++
    ninja
    Pop-Location
    
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Build failed!" -ForegroundColor Red
        exit 1
    }
    
    # Copy built exe to System folder (check both source bin/ and build/ folder)
    $ExtBinDir = Join-Path $ExtPath "bin"
    $BuiltExe = $null
    if (Test-Path $ExtBinDir) {
        $BuiltExe = Get-ChildItem -Path $ExtBinDir -Filter "*.exe" | Select-Object -First 1
    }
    if (-not $BuiltExe) {
        $BuiltExe = Get-ChildItem -Path $BuildDir -Filter "*.exe" -Exclude "*.dir" | Select-Object -First 1
    }
    if ($BuiltExe) {
        Copy-Item $BuiltExe.FullName -Destination $SystemDir -Force
    }
}
else {
    Write-Host "No build system found for $Name" -ForegroundColor Red
    exit 1
}

Write-Host "[2/2] Done! Output: Bin\Release\System\$Name.exe" -ForegroundColor Green
