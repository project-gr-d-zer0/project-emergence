# Emergence acceptance oracle (l3e7 UE host). Build + test + CAPTURE ERRORS for the autonomous builder.
# Emits ONE JSON line: {available,compile,tests_pass,tests_total,score,build_errors,failures}
$ErrorActionPreference = 'SilentlyContinue'
$repo = Split-Path -Parent $PSScriptRoot
Set-Location $repo
# HARD-MIRROR origin/master (build-drive is a slave checkout; merge-pull can silently build stale code).
git fetch --quiet origin 2>$null
git reset --hard origin/master 2>$null
git clean -fd 2>$null
# Kill any lingering test-runner from a previous cycle - it holds a write-lock on the module DLLs and makes
# the next build fail spuriously (UbaSessionServer 'file being used' retries).
Get-Process UnrealEditor-Cmd -ErrorAction SilentlyContinue | Stop-Process -Force -ErrorAction SilentlyContinue
Start-Sleep -Milliseconds 500
$ue = $env:UE_ROOT
if (-not $ue) {
  foreach ($b in @("C:\Program Files\Epic Games","E:\EpicGamesLibrary","D:\EpicGamesLibrary","C:\EpicGamesLibrary","E:\Epic Games","D:\Epic Games")) {
    $c = Get-ChildItem $b -Directory -Filter "UE_5.*" -EA SilentlyContinue | Sort-Object Name -Descending | Select-Object -First 1
    if ($c) { $ue = $c.FullName; break }
  }
}
$cmd = if ($ue) { Join-Path $ue "Engine\Binaries\Win64\UnrealEditor-Cmd.exe" } else { $null }
if (-not $cmd -or -not (Test-Path $cmd)) { '{"available":false,"note":"UE5.8 not installed on l3e7"}'; exit 0 }
$uproj = Join-Path $repo "ProjectEmergence.uproject"
$blog = Join-Path $repo "Saved\build_last.log"
& (Join-Path $ue "Engine\Build\BatchFiles\Build.bat") ProjectEmergenceEditor Win64 Development -Project="$uproj" -WaitMutex 2>&1 | Out-File -FilePath $blog -Encoding UTF8
$compile = ($LASTEXITCODE -eq 0)
$pass = 0; $total = 0; $errtext = ""; $fails = @()
if (-not $compile) {
  $errlines = Select-String -Path $blog -Pattern "error C\d|error LNK|error MSB|: error|error :" | ForEach-Object { $_.Line.Trim() } | Select-Object -First 30
  $errtext = ($errlines -join "`n")
  if ($errtext.Length -gt 3500) { $errtext = $errtext.Substring(0, 3500) }
} else {
  $rd = Join-Path $repo "Saved\TestReports"
  Remove-Item (Join-Path $rd "index.json") -Force -EA SilentlyContinue
  $tests = if ($args[0]) { $args[0] } else { "Emergence." }
  & $cmd "$uproj" -execcmds="Automation RunTests $tests; Quit" -unattended -nopause -nosplash -nullrhi -stdout -ReportOutputPath="$rd" 2>&1 | Out-Null
  $idx = Join-Path $rd "index.json"
  if (Test-Path $idx) {
    $r = Get-Content $idx -Raw | ConvertFrom-Json
    $total = @($r.tests).Count
    $pass = @($r.tests | Where-Object { $_.state -eq "Success" }).Count
    foreach ($t in @($r.tests | Where-Object { $_.state -ne "Success" })) {
      $msg = (@($t.entries | Where-Object { $_.event.type -eq "Error" } | ForEach-Object { $_.event.message })) -join "; "
      $fails += ("{0} :: {1}" -f $t.fullTestPath, $msg)
    }
  }
}
$score = if (-not $compile) { 0.0 } elseif ($total -gt 0) { [math]::Round($pass / $total, 3) } else { 0.5 }
(@{ available = $true; compile = $compile; tests_pass = $pass; tests_total = $total; score = $score; build_errors = $errtext; failures = $fails } | ConvertTo-Json -Compress)
