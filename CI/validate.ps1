# Emergence acceptance oracle (runs on the l3e7 Windows UE host). Emits ONE JSON line for emerge_validate.
$ErrorActionPreference = 'SilentlyContinue'
$repo = Split-Path -Parent $PSScriptRoot
Set-Location $repo
git pull --quiet 2>$null
$ue = $env:UE_ROOT
if (-not $ue) { $ue = (Get-ChildItem "C:\Program Files\Epic Games" -Directory -Filter "UE_5.*" -EA SilentlyContinue | Sort-Object Name -Descending | Select-Object -First 1).FullName }
$cmd = if ($ue) { Join-Path $ue "Engine\Binaries\Win64\UnrealEditor-Cmd.exe" } else { $null }
if (-not $cmd -or -not (Test-Path $cmd)) { '{"available":false,"note":"UE5.8 not installed on l3e7 (host not provisioned)"}'; exit 0 }
$uproj = Join-Path $repo "ProjectEmergence.uproject"
& (Join-Path $ue "Engine\Build\BatchFiles\Build.bat") ProjectEmergenceEditor Win64 Development -Project="$uproj" -WaitMutex 2>&1 | Out-Null
$compile = ($LASTEXITCODE -eq 0)
$pass = 0; $total = 0
if ($compile) {
  $rd = Join-Path $repo "Saved\TestReports"
  $tests = if ($args[0]) { $args[0] } else { "Emergence." }
  & $cmd "$uproj" -execcmds="Automation RunTests $tests; Quit" -unattended -nopause -nosplash -nullrhi -stdout -ReportOutputPath="$rd" 2>&1 | Out-Null
  $idx = Join-Path $rd "index.json"
  if (Test-Path $idx) { $r = Get-Content $idx -Raw | ConvertFrom-Json; $total = @($r.tests).Count; $pass = @($r.tests | Where-Object { $_.state -eq "Success" }).Count }
}
$score = if (-not $compile) { 0.0 } elseif ($total -gt 0) { [math]::Round($pass/$total,3) } else { 0.5 }
(@{ available=$true; compile=$compile; tests_pass=$pass; tests_total=$total; score=$score } | ConvertTo-Json -Compress)
