# PowerShell скрипт для тестирования производительности медианного фильтра
# Использование: .\test_performance.ps1

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  Тестирование производительности" -ForegroundColor Cyan
Write-Host "  Медианный фильтр" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Проверка наличия исполняемого файла
$exePath = ""
if (Test-Path ".\src\Release\median_filter.exe") {
    $exePath = ".\src\Release\median_filter.exe"
} elseif (Test-Path ".\src\median_filter.exe") {
    $exePath = ".\src\median_filter.exe"
} elseif (Test-Path ".\median_filter.exe") {
    $exePath = ".\median_filter.exe"
} else {
    Write-Host "Ошибка: median_filter.exe не найден!" -ForegroundColor Red
    Write-Host "Убедитесь, что программа собрана и вы находитесь в директории build/" -ForegroundColor Yellow
    exit 1
}

Write-Host "Используется: $exePath" -ForegroundColor Green
Write-Host ""

# Параметры тестирования
$sizes = @(100, 500, 1000)
$threads = @(1, 2, 4, 8)
$windowSize = 5
$iterations = 3
$runs = 3  # Количество повторений для усреднения

# Массив для результатов
$results = @()

$totalTests = $sizes.Count * $threads.Count
$currentTest = 0

foreach ($size in $sizes) {
    foreach ($t in $threads) {
        $currentTest++
        Write-Host "[$currentTest/$totalTests] Тест: матрица ${size}x${size}, потоков: $t" -ForegroundColor Yellow
        
        $times = @()
        
        for ($run = 1; $run -le $runs; $run++) {
            Write-Host "  Запуск $run/$runs... " -NoNewline
            
            # Запускаем программу и захватываем вывод
            $output = & $exePath -g $size $size -w $windowSize -k $iterations -t $t 2>&1
            
            # Ищем строку с общим временем
            $timeLine = $output | Select-String "Общее время: ([\d.]+) мс"
            
            if ($timeLine -and $timeLine.Matches.Groups.Count -ge 2) {
                $time = [double]$timeLine.Matches.Groups[1].Value
                $times += $time
                Write-Host "$time мс" -ForegroundColor Green
            } else {
                Write-Host "Ошибка парсинга!" -ForegroundColor Red
            }
            
            # Небольшая пауза между запусками
            Start-Sleep -Milliseconds 500
        }
        
        if ($times.Count -gt 0) {
            $avgTime = ($times | Measure-Object -Average).Average
            $minTime = ($times | Measure-Object -Minimum).Minimum
            $maxTime = ($times | Measure-Object -Maximum).Maximum
            
            Write-Host "  Среднее: $([math]::Round($avgTime, 2)) мс (мин: $([math]::Round($minTime, 2)), макс: $([math]::Round($maxTime, 2)))" -ForegroundColor Cyan
            
            # Вычисляем ускорение относительно 1 потока
            $speedup = 0
            $efficiency = 0
            
            if ($t -eq 1) {
                $baseTime = $avgTime
                $speedup = 1.0
                $efficiency = 1.0
            } else {
                # Найти базовое время для этого размера
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

# Вывод итоговой таблицы
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  Итоговые результаты" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

$results | Format-Table -AutoSize

# Сохранение в CSV
$csvPath = "performance_results.csv"
$results | Export-Csv -Path $csvPath -NoTypeInformation -Encoding UTF8
Write-Host "Результаты сохранены в файл: $csvPath" -ForegroundColor Green

# Анализ результатов
Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  Анализ" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

foreach ($size in $sizes) {
    Write-Host "Матрица ${size}x${size}:" -ForegroundColor Yellow
    $sizeResults = $results | Where-Object { $_.Size -eq $size }
    
    Write-Host "  Потоки | Время (мс) | Ускорение | Эффективность"
    Write-Host "  -------|------------|-----------|---------------"
    
    foreach ($r in $sizeResults) {
        $threadsStr = $r.Threads.ToString().PadLeft(6)
        $timeStr = $r.AvgTime.ToString().PadLeft(10)
        $speedupStr = $r.Speedup.ToString().PadLeft(9)
        $effStr = ($r.Efficiency * 100).ToString("F1").PadLeft(12) + "%"
        
        Write-Host "  $threadsStr | $timeStr | $speedupStr | $effStr"
    }
    
    Write-Host ""
}

# Рекомендации
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  Рекомендации" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Найти оптимальное количество потоков (максимальная эффективность > 0.7)
$optimal = $results | Where-Object { $_.Efficiency -gt 0.7 } | Sort-Object -Property Threads -Descending | Select-Object -First 1

if ($optimal) {
    Write-Host "Оптимальное количество потоков: $($optimal.Threads)" -ForegroundColor Green
    Write-Host "  Эффективность: $([math]::Round($optimal.Efficiency * 100, 1))%" -ForegroundColor Green
} else {
    Write-Host "Для всех тестов эффективность < 70%" -ForegroundColor Yellow
}

Write-Host ""
Write-Host "Готово!" -ForegroundColor Green

