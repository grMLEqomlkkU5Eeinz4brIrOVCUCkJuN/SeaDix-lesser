# PowerShell script to optimize system for stable benchmarking
# Run as Administrator for best results

Write-Host "Optimizing system for stable benchmarking..." -ForegroundColor Green

# Set high performance power plan
Write-Host "Setting high performance power plan..." -ForegroundColor Yellow
powercfg /setactive 8c5e7fda-e8bf-4a96-9a85-a6e23a8c635c

# Disable Windows Defender real-time protection temporarily (optional)
Write-Host "Note: Consider temporarily disabling Windows Defender for benchmarking" -ForegroundColor Yellow
Write-Host "Run: Set-MpPreference -DisableRealtimeMonitoring $true" -ForegroundColor Cyan
Write-Host "Remember to re-enable after: Set-MpPreference -DisableRealtimeMonitoring $false" -ForegroundColor Cyan

# Set process priority
Write-Host "Setting Node.js process priority to High..." -ForegroundColor Yellow
$nodeProcesses = Get-Process node -ErrorAction SilentlyContinue
if ($nodeProcesses) {
    foreach ($proc in $nodeProcesses) {
        $proc.PriorityClass = [System.Diagnostics.ProcessPriorityClass]::High
    }
    Write-Host "Node.js processes set to high priority" -ForegroundColor Green
} else {
    Write-Host "No running Node.js processes found" -ForegroundColor Yellow
}

# Check for thermal throttling indicators
Write-Host "Checking CPU temperature indicators..." -ForegroundColor Yellow
$cpu = Get-WmiObject -Class Win32_Processor
Write-Host "CPU Load: $($cpu.LoadPercentage)%" -ForegroundColor Cyan

# Check available memory
$memory = Get-WmiObject -Class Win32_OperatingSystem
$freeMemoryGB = [math]::Round($memory.FreePhysicalMemory / 1024 / 1024, 2)
$totalMemoryGB = [math]::Round($memory.TotalVisibleMemorySize / 1024 / 1024, 2)
Write-Host "Memory: $freeMemoryGB GB free of $totalMemoryGB GB total" -ForegroundColor Cyan

if ($freeMemoryGB -lt 2) {
    Write-Host "⚠️  Low memory detected. Consider closing applications." -ForegroundColor Red
}

# Recommend benchmark command
Write-Host "`nRecommended benchmark command:" -ForegroundColor Green
Write-Host "node --expose-gc --max-old-space-size=4096 -r ts-node/register benchmark/stable-benchmark.ts" -ForegroundColor Cyan

Write-Host "`nOptimization complete! Run benchmarks now for best stability." -ForegroundColor Green
