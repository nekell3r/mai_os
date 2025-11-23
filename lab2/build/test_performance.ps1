# PowerShell script for testing median filter performance
# Usage: .\test_performance.ps1

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  Performance Testing" -ForegroundColor Cyan
Write-Host "  Median Filter" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Check for executable file
$exePath = ""
if (Test-Path ".\src\median_filter.exe") {
    # Ninja build
    $exePath = ".\src\median_filter.exe"
    Write-Host "Found executable (Ninja): $exePath" -ForegroundColor Green
} elseif (Test-Path ".\src\Release\median_filter.exe") {
    # Visual Studio Release build
    $exePath = ".\src\Release\median_filter.exe"
    Write-Host "Found executable (VS Release): $exePath" -ForegroundColor Green
} elseif (Test-Path ".\src\Debug\median_filter.exe") {
    # Visual Studio Debug build
    $exePath = ".\src\Debug\median_filter.exe"
    Write-Host "Found executable (VS Debug): $exePath" -ForegroundColor Yellow
    Write-Host "Warning: using Debug version, results may be slower" -ForegroundColor Yellow
} elseif (Test-Path ".\median_filter.exe") {
    $exePath = ".\median_filter.exe"
    Write-Host "Found executable: $exePath" -ForegroundColor Green
} else {
    Write-Host "Error: median_filter.exe not found!" -ForegroundColor Red
    Write-Host "Make sure the program is built and you are in the build/ directory" -ForegroundColor Yellow
    Write-Host "" -ForegroundColor Yellow
    Write-Host "To build with Ninja:" -ForegroundColor Cyan
    Write-Host "  cmake -G Ninja -DCMAKE_BUILD_TYPE=Release .." -ForegroundColor Cyan
    Write-Host "  ninja" -ForegroundColor Cyan
    Write-Host "" -ForegroundColor Yellow
    Write-Host "Or for Visual Studio:" -ForegroundColor Cyan
    Write-Host "  cmake .." -ForegroundColor Cyan
    Write-Host "  cmake --build . --config Release" -ForegroundColor Cyan
    exit 1
}

Write-Host "Using: $exePath" -ForegroundColor Green
Write-Host ""

# Test parameters
$sizes = @(100, 500, 1000)
$threads = @(1, 2, 4, 8)
$windowSize = 5
$iterations = 3
$runs = 3  # Number of runs for averaging

# Results array
$results = @()

$totalTests = $sizes.Count * $threads.Count
$currentTest = 0

foreach ($size in $sizes) {
    foreach ($t in $threads) {
        $currentTest++
        Write-Host "[$currentTest/$totalTests] Test: matrix ${size}x${size}, threads: $t" -ForegroundColor Yellow
        
        $times = @()
        
        for ($run = 1; $run -le $runs; $run++) {
            Write-Host "  Run $run/$runs... " -NoNewline
            
            # Execute program and capture output
            $output = & $exePath -g $size $size -w $windowSize -k $iterations -t $t 2>&1
            
            # Find line with total time
            $timeLine = $output | Select-String "Total time: ([\d.]+) ms"
            
            if ($timeLine -and $timeLine.Matches.Groups.Count -ge 2) {
                $time = [double]$timeLine.Matches.Groups[1].Value
                $times += $time
                Write-Host "$time ms" -ForegroundColor Green
            } else {
                Write-Host "Parse error!" -ForegroundColor Red
            }
            
            # Small pause between runs
            Start-Sleep -Milliseconds 500
        }
        
        if ($times.Count -gt 0) {
            $avgTime = ($times | Measure-Object -Average).Average
            $minTime = ($times | Measure-Object -Minimum).Minimum
            $maxTime = ($times | Measure-Object -Maximum).Maximum
            
            Write-Host "  Average: $([math]::Round($avgTime, 2)) ms (min: $([math]::Round($minTime, 2)), max: $([math]::Round($maxTime, 2)))" -ForegroundColor Cyan
            
            # Calculate speedup relative to 1 thread
            $speedup = 0
            $efficiency = 0
            
            if ($t -eq 1) {
                $baseTime = $avgTime
                $speedup = 1.0
                $efficiency = 1.0
            } else {
                # Find base time for this size
                $baseResult = $results | Where-Object { $_.Size -eq $size -and $_.Threads -eq 1 }
                if ($baseResult) {
                    $baseTime = $baseResult.AvgTime
                    $speedup = $baseTime / $avgTime
                    $efficiency = $speedup / $t
                }
            }
            
            $results += [PSCustomObject]@{
                Size = $size
                Threads = $t
                AvgTime = [math]::Round($avgTime, 2)
                MinTime = [math]::Round($minTime, 2)
                MaxTime = [math]::Round($maxTime, 2)
                Speedup = [math]::Round($speedup, 2)
                Efficiency = [math]::Round($efficiency, 2)
            }
        }
        
        Write-Host ""
    }
}

# Display results table
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  Results" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

$results | Format-Table -AutoSize

# Save to CSV
$csvPath = "performance_results.csv"
$results | Export-Csv -Path $csvPath -NoTypeInformation
Write-Host "Results saved to: $csvPath" -ForegroundColor Green

# Analysis
Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  Analysis" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

foreach ($size in $sizes) {
    Write-Host "Matrix ${size}x${size}:" -ForegroundColor Yellow
    $sizeResults = $results | Where-Object { $_.Size -eq $size }
    
    Write-Host "  Threads | Time (ms) | Speedup | Efficiency"
    Write-Host "  --------|-----------|---------|------------"
    
    foreach ($r in $sizeResults) {
        $threadsStr = $r.Threads.ToString().PadLeft(6)
        $timeStr = $r.AvgTime.ToString().PadLeft(9)
        $speedupStr = $r.Speedup.ToString().PadLeft(7)
        $effStr = ($r.Efficiency * 100).ToString("F1").PadLeft(10) + "%"
        
        Write-Host "  $threadsStr | $timeStr | $speedupStr | $effStr"
    }
    
    Write-Host ""
}

# Recommendations
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  Recommendations" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Find optimal number of threads (maximum efficiency > 0.7)
$optimal = $results | Where-Object { $_.Efficiency -gt 0.7 } | Sort-Object -Property Threads -Descending | Select-Object -First 1

if ($optimal) {
    Write-Host "Optimal thread count: $($optimal.Threads)" -ForegroundColor Green
    Write-Host "  Efficiency: $([math]::Round($optimal.Efficiency * 100, 1))%" -ForegroundColor Green
} else {
    Write-Host "All tests have efficiency < 70%" -ForegroundColor Yellow
}

Write-Host ""
Write-Host "Done!" -ForegroundColor Green
