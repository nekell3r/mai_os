# Script for quick rebuild and run Lab3
# Usage: .\rebuild.ps1

param(
    [switch]$Clean = $false,
    [switch]$Run = $true
)

$ErrorActionPreference = "Stop"

# Colors for output
function Write-ColorOutput($ForegroundColor) {
    $fc = $host.UI.RawUI.ForegroundColor
    $host.UI.RawUI.ForegroundColor = $ForegroundColor
    if ($args) {
        Write-Output $args
    }
    $host.UI.RawUI.ForegroundColor = $fc
}

Write-ColorOutput Green "========================================="
Write-ColorOutput Green "  Lab3 Rebuild Script"
Write-ColorOutput Green "========================================="

# Check that we are in the right directory
if (-not (Test-Path "CMakeLists.txt")) {
    Write-ColorOutput Red "Error: CMakeLists.txt not found"
    Write-ColorOutput Red "Run script from lab3 directory"
    exit 1
}

# Clean build directory if -Clean flag is specified
if ($Clean) {
    Write-ColorOutput Yellow "Cleaning build directory..."
    if (Test-Path "build") {
        Remove-Item -Recurse -Force build
    }
}

# Create build directory
if (-not (Test-Path "build")) {
    Write-ColorOutput Yellow "Creating build directory..."
    New-Item -ItemType Directory -Path "build" | Out-Null
}

# Go to build
Push-Location build

try {
    # Check for Ninja
    $ninjaExists = Get-Command ninja -ErrorAction SilentlyContinue
    
    if ($ninjaExists) {
        Write-ColorOutput Yellow "Configuring CMake (Ninja)..."
        cmake .. -G Ninja
        
        Write-ColorOutput Yellow "Building project..."
        ninja
    } else {
        Write-ColorOutput Yellow "Ninja not found, using Visual Studio..."
        Write-ColorOutput Yellow "Configuring CMake (Visual Studio)..."
        cmake .. -G "Visual Studio 17 2022"
        
        Write-ColorOutput Yellow "Building project..."
        cmake --build . --config Release
    }
    
    if ($LASTEXITCODE -ne 0) {
        Write-ColorOutput Red "Build error!"
        exit 1
    }
    
    Write-ColorOutput Green "[OK] Build completed successfully!"
    
    # Determine directory with executables
    $exeDir = "src"
    if (-not (Test-Path "$exeDir\parent.exe")) {
        $exeDir = "src\Release"
    }
    
    # Copy test file
    Write-ColorOutput Yellow "Copying test_numbers.txt..."
    Copy-Item -Force "..\test_numbers.txt" "$exeDir\"
    
    if ($Run) {
        Write-ColorOutput Green "========================================="
        Write-ColorOutput Green "  Running program"
        Write-ColorOutput Green "========================================="
        
        Push-Location $exeDir
        try {
            # Check for executables
            if (-not (Test-Path "parent.exe")) {
                Write-ColorOutput Red "Error: parent.exe not found!"
                exit 1
            }
            if (-not (Test-Path "child.exe")) {
                Write-ColorOutput Red "Error: child.exe not found!"
                exit 1
            }
            
            # Run program
            echo "test_numbers.txt" | .\parent.exe
            
            Write-ColorOutput Green "========================================="
            Write-ColorOutput Green "  Program completed"
            Write-ColorOutput Green "========================================="
        } finally {
            Pop-Location
        }
    } else {
        Write-ColorOutput Yellow "To run the program:"
        Write-ColorOutput Yellow "  cd build\$exeDir"
        Write-ColorOutput Yellow "  echo test_numbers.txt | .\parent.exe"
    }
    
} catch {
    Write-ColorOutput Red "Error: $_"
    exit 1
} finally {
    Pop-Location
}

Write-ColorOutput Green "[OK] Done!"
