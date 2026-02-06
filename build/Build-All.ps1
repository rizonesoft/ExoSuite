<#
.SYNOPSIS
    Build all ExoSuite components (shell + extensions).

.DESCRIPTION
    Builds ExoSuite shell and all extensions with progress display.
    Output goes to Bin/Release/ for shell and Bin/Release/System/ for extensions.

.EXAMPLE
    .\build\Build-All.ps1 -Release
#>

param(
    [switch]$Release
)

$ErrorActionPreference = 'Stop'
$RepoRoot = Split-Path $PSScriptRoot -Parent

# Initialize ExoKit if available
$ExoKitInit = Join-Path $RepoRoot "exokit\Init-ExoKit.ps1"
if (Test-Path $ExoKitInit) {
    . $ExoKitInit -Quiet
}

$OutputDir = Join-Path $RepoRoot "Bin\Release"
$SystemDir = Join-Path $OutputDir "System"

# Ensure output directories exist
New-Item -ItemType Directory -Force -Path $OutputDir, $SystemDir | Out-Null

# Count components
$ExtensionsDir = Join-Path $RepoRoot "extensions"
$Extensions = @()
if (Test-Path $ExtensionsDir) {
    $Extensions = Get-ChildItem -Path $ExtensionsDir -Directory
}
$TotalSteps = 1 + $Extensions.Count + 1  # Shell + extensions + final step

$CurrentStep = 1

# Build shell
Write-Host "[$CurrentStep/$TotalSteps] Compiling ExoSuite shell..." -ForegroundColor Cyan
$cargoArgs = @("build", "-p", "exosuite")
if ($Release) { $cargoArgs += "--release" }

& cargo @cargoArgs
if ($LASTEXITCODE -ne 0) {
    Write-Host "Shell build failed!" -ForegroundColor Red
    exit 1
}

# Copy shell to output
if ($Release) {
    $ShellExe = Join-Path $RepoRoot "target\release\ExoSuite.exe"
    Copy-Item $ShellExe -Destination $OutputDir -Force
}
$CurrentStep++

# Build extensions
foreach ($Ext in $Extensions) {
    Write-Host "[$CurrentStep/$TotalSteps] Compiling $($Ext.Name)..." -ForegroundColor Cyan
    
    $ExtPath = $Ext.FullName
    $CmakeLists = Join-Path $ExtPath "CMakeLists.txt"
    $CargoToml = Join-Path $ExtPath "Cargo.toml"
    
    if (Test-Path $CargoToml) {
        # Rust extension
        $cargoArgs = @("build", "-p", $Ext.Name)
        if ($Release) { $cargoArgs += "--release" }
        & cargo @cargoArgs
        
        if ($Release) {
            $ExtExe = Join-Path $RepoRoot "target\release\$($Ext.Name).exe"
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
        cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
        ninja
        Pop-Location
        
        # Copy built exe to System folder
        $BuiltExe = Get-ChildItem -Path $BuildDir -Filter "*.exe" | Select-Object -First 1
        if ($BuiltExe) {
            Copy-Item $BuiltExe.FullName -Destination $SystemDir -Force
        }
    }
    
    $CurrentStep++
}

Write-Host "[$CurrentStep/$TotalSteps] Done! Output: Bin\Release\" -ForegroundColor Green
Write-Host ""
Write-Host "  Shell:      Bin\Release\ExoSuite.exe" -ForegroundColor DarkGray
if ($Extensions.Count -gt 0) {
    Write-Host "  Extensions: Bin\Release\System\" -ForegroundColor DarkGray
}
