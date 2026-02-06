<#
.SYNOPSIS
    Bootstrap ExoKit toolchain for ExoSuite.

.DESCRIPTION
    Downloads and installs the required build tools into the exokit/ folder.
    Run this once before building ExoSuite.

.EXAMPLE
    .\exokit\Bootstrap-ExoKit.ps1
#>

param(
    [switch]$Force
)

$ErrorActionPreference = 'Stop'
$ExoKitRoot = $PSScriptRoot

Write-Host "ExoKit Bootstrap" -ForegroundColor Cyan
Write-Host "================" -ForegroundColor Cyan

# Tool versions
$RustVersion = "stable"
$LlvmVersion = "20250114"
$CmakeVersion = "3.31.4"
$NinjaVersion = "1.12.1"

# Directories
$RustDir = Join-Path $ExoKitRoot "rust"
$LlvmDir = Join-Path $ExoKitRoot "llvm-mingw"
$CmakeDir = Join-Path $ExoKitRoot "cmake"
$NinjaDir = Join-Path $ExoKitRoot "ninja"

# Download URLs
$LlvmUrl = "https://github.com/mstorsjo/llvm-mingw/releases/download/$LlvmVersion/llvm-mingw-$LlvmVersion-ucrt-x86_64.zip"
$CmakeUrl = "https://github.com/Kitware/CMake/releases/download/v$CmakeVersion/cmake-$CmakeVersion-windows-x86_64.zip"
$NinjaUrl = "https://github.com/ninja-build/ninja/releases/download/v$NinjaVersion/ninja-win.zip"

function Install-Tool {
    param(
        [string]$Name,
        [string]$Url,
        [string]$DestDir,
        [string]$ExtractPath
    )
    
    if ((Test-Path $DestDir) -and -not $Force) {
        Write-Host "  [SKIP] $Name already installed" -ForegroundColor DarkGray
        return
    }
    
    Write-Host "  [DOWNLOAD] $Name..." -ForegroundColor Yellow
    $tempZip = Join-Path $env:TEMP "$Name.zip"
    
    Invoke-WebRequest -Uri $Url -OutFile $tempZip -UseBasicParsing
    
    Write-Host "  [EXTRACT] $Name..." -ForegroundColor Yellow
    $tempExtract = Join-Path $env:TEMP "$Name-extract"
    if (Test-Path $tempExtract) { Remove-Item $tempExtract -Recurse -Force }
    Expand-Archive -Path $tempZip -DestinationPath $tempExtract -Force
    
    # Find the actual content (may be in a subfolder)
    $content = Get-ChildItem $tempExtract
    if ($content.Count -eq 1 -and $content[0].PSIsContainer) {
        $source = $content[0].FullName
    }
    else {
        $source = $tempExtract
    }
    
    if (Test-Path $DestDir) { Remove-Item $DestDir -Recurse -Force }
    Move-Item $source $DestDir -Force
    
    Remove-Item $tempZip -Force
    if (Test-Path $tempExtract) { Remove-Item $tempExtract -Recurse -Force }
    
    Write-Host "  [OK] $Name installed" -ForegroundColor Green
}

# Install Rust via rustup
Write-Host ""
Write-Host "[1/4] Rust Toolchain" -ForegroundColor Cyan

if ((Test-Path (Join-Path $RustDir "bin\cargo.exe")) -and -not $Force) {
    Write-Host "  [SKIP] Rust already installed" -ForegroundColor DarkGray
}
else {
    Write-Host "  [INSTALL] Rust $RustVersion..." -ForegroundColor Yellow
    
    # Set RUSTUP_HOME and CARGO_HOME to ExoKit
    $env:RUSTUP_HOME = $RustDir
    $env:CARGO_HOME = $RustDir
    
    # Download and run rustup-init
    $rustupInit = Join-Path $env:TEMP "rustup-init.exe"
    Invoke-WebRequest -Uri "https://win.rustup.rs/x86_64" -OutFile $rustupInit -UseBasicParsing
    
    & $rustupInit -y --default-toolchain $RustVersion --no-modify-path
    
    Remove-Item $rustupInit -Force
    Write-Host "  [OK] Rust installed" -ForegroundColor Green
}

# Install LLVM-MinGW (for C++ extensions)
Write-Host ""
Write-Host "[2/4] LLVM-MinGW" -ForegroundColor Cyan
Install-Tool -Name "llvm-mingw" -Url $LlvmUrl -DestDir $LlvmDir

# Install CMake
Write-Host ""
Write-Host "[3/4] CMake" -ForegroundColor Cyan
Install-Tool -Name "cmake" -Url $CmakeUrl -DestDir $CmakeDir

# Install Ninja
Write-Host ""
Write-Host "[4/4] Ninja" -ForegroundColor Cyan
Install-Tool -Name "ninja" -Url $NinjaUrl -DestDir $NinjaDir

Write-Host ""
Write-Host "ExoKit bootstrap complete!" -ForegroundColor Green
Write-Host "Run '.\exokit\Init-ExoKit.ps1' to activate the toolchain." -ForegroundColor DarkGray
