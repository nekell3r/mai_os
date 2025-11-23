# Script for quick project rebuild
# Usage: .\rebuild.ps1 [clean]

param(
    [switch]$Clean = $false
)

$ErrorActionPreference = "Stop"

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  Rebuilding lab2" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Check for Ninja
try {
    $ninjaVersion = ninja --version 2>$null
    Write-Host "Ninja version: $ninjaVersion" -ForegroundColor Green
} catch {
    Write-Host "Error: Ninja not found!" -ForegroundColor Red
    Write-Host "Install Ninja:" -ForegroundColor Yellow
    Write-Host "  choco install ninja" -ForegroundColor Yellow
    Write-Host "  or" -ForegroundColor Yellow
    Write-Host "  scoop install ninja" -ForegroundColor Yellow
    exit 1
}

# Check for CMake
try {
    $cmakeVersion = cmake --version 2>$null | Select-Object -First 1
    Write-Host "CMake: $cmakeVersion" -ForegroundColor Green
} catch {
    Write-Host "Error: CMake not found!" -ForegroundColor Red
    exit 1
}

Write-Host ""

# If clean parameter is specified, remove build directory
if ($Clean) {
    Write-Host "Full clean..." -ForegroundColor Yellow
    if (Test-Path "build") {
        Remove-Item -Path "build" -Recurse -Force
        Write-Host "Build directory removed" -ForegroundColor Green
    }
}

# Create build directory if it doesn't exist
if (!(Test-Path "build")) {
    Write-Host "Creating build directory..." -ForegroundColor Yellow
    New-Item -ItemType Directory -Path "build" | Out-Null
    
    # First build - run CMake
    Push-Location build
    
    Write-Host "Running CMake..." -ForegroundColor Yellow
    cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ..
    
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Error during build file generation!" -ForegroundColor Red
        Pop-Location
        exit 1
    }
    
    Pop-Location
}

# Go to build and compile
Push-Location build

Write-Host "Building project..." -ForegroundColor Yellow
ninja

if ($LASTEXITCODE -ne 0) {
    Write-Host ""
    Write-Host "Build error!" -ForegroundColor Red
    Pop-Location
    exit 1
}

Write-Host ""
Write-Host "========================================" -ForegroundColor Green
Write-Host "  Build successful!" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green
Write-Host ""

# Check for executable file
if (Test-Path "src\median_filter.exe") {
    Write-Host "Executable: $(Resolve-Path 'src\median_filter.exe')" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "To run:" -ForegroundColor Yellow
    Write-Host "  cd build\src" -ForegroundColor White
    Write-Host "  .\median_filter.exe -h" -ForegroundColor White
    Write-Host ""
    Write-Host "For performance testing:" -ForegroundColor Yellow
    Write-Host "  copy ..\test_performance.ps1 ." -ForegroundColor White
    Write-Host "  .\test_performance.ps1" -ForegroundColor White
} else {
    Write-Host "Warning: executable file not found!" -ForegroundColor Yellow
}

Pop-Location
