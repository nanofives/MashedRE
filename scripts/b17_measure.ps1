# b17_measure.ps1 — cold-start reliability harness for the standalone exe.
#
# For each run: copies mashed_re.exe to a fresh temp dir (to dodge the shim
# d3d9.dll/dinput8.dll in mashedmod\build\ that crash the standalone at load),
# launches it with WorkingDirectory = repo root (so original\TOASTART\... and
# the mashed_re.log path resolve), forces the window TOPMOST, screenshots it,
# kills it, archives mashed_re.log, and parses the B17 summary + B17-GRAN lines.
#
# Aggregates over N runs: boot-success%, chrome-ready%, and per-needed-granule
# blocked-frequency (the cold-start lottery, made into numbers).
#
# Usage:  pwsh scripts\b17_measure.ps1 [-Runs 20] [-Tag baseline] [-WaitSec 7] [-Shots 3]
param(
  [int]$Runs    = 20,
  [string]$Tag  = "baseline",
  [double]$WaitSec = 7.0,   # settle time before screenshot
  [int]$Shots   = 3         # how many of the runs to screenshot (first N)
)

Add-Type @"
using System;
using System.Runtime.InteropServices;
public class W32 {
  [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr h, out RECT r);
  [DllImport("user32.dll")] public static extern bool SetWindowPos(IntPtr h, IntPtr after, int x, int y, int cx, int cy, uint flags);
  [DllImport("user32.dll")] public static extern bool IsWindowVisible(IntPtr h);
  [StructLayout(LayoutKind.Sequential)] public struct RECT { public int Left, Top, Right, Bottom; }
}
"@
Add-Type -AssemblyName System.Drawing

$root   = "C:\Users\maria\Desktop\Proyectos\Mashed"
$srcExe = Join-Path $root "mashedmod\build\mashed_re.exe"
$verify = Join-Path $root "verify"
$logdir = Join-Path $root ("log\b17_" + $Tag)
$logsrc = Join-Path $root "mashed_re.log"
$tmpdir = Join-Path $env:TEMP ("b17_run_" + [System.Guid]::NewGuid().ToString("N").Substring(0,8))

$HWND_TOPMOST = New-Object IntPtr(-1)
$SWP = 0x0040 -bor 0x0010  # SHOWWINDOW | NOACTIVATE-ish (SHOWWINDOW|NOOWNERZORDER)

if (-not (Test-Path $srcExe)) { Write-Error "missing $srcExe — build first"; exit 1 }
New-Item -ItemType Directory -Force -Path $tmpdir | Out-Null
New-Item -ItemType Directory -Force -Path $logdir | Out-Null
$runExe = Join-Path $tmpdir "mashed_re.exe"
Copy-Item $srcExe $runExe -Force

# Needed granules, mirrors kNeededGranules in exe_main.cpp.
$granKeys = @('00420000','00470000','005C0000','00630000','00670000','007D0000','007F0000','00890000')

function Take-Shot($hwnd, $name) {
  $r = New-Object W32+RECT
  if (-not [W32]::GetWindowRect($hwnd, [ref]$r)) { return "rect-fail" }
  $w = $r.Right - $r.Left; $h = $r.Bottom - $r.Top
  if ($w -le 0 -or $h -le 0) { return "rect 0x0" }
  try {
    $bmp = New-Object System.Drawing.Bitmap $w, $h
    $g = [System.Drawing.Graphics]::FromImage($bmp)
    $g.CopyFromScreen($r.Left, $r.Top, 0, 0, (New-Object System.Drawing.Size $w, $h))
    $p = Join-Path $verify $name
    $bmp.Save($p, [System.Drawing.Imaging.ImageFormat]::Png)
    $g.Dispose(); $bmp.Dispose()
    return "saved $name (${w}x${h})"
  } catch { return "shot-exc: $_" }
}

$results = New-Object System.Collections.ArrayList
$granBlocked = @{}; foreach ($k in $granKeys) { $granBlocked[$k] = 0 }
$bootOk = 0; $chromeOk = 0

for ($i = 1; $i -le $Runs; $i++) {
  if (Test-Path $logsrc) { Remove-Item $logsrc -Force -ErrorAction SilentlyContinue }
  $proc = Start-Process -FilePath $runExe -WorkingDirectory $root -PassThru
  $procId = $proc.Id
  # Poll for the window handle.
  $hwnd = [IntPtr]::Zero
  $deadline = (Get-Date).AddSeconds(6)
  while ((Get-Date) -lt $deadline) {
    try { $proc.Refresh() } catch {}
    if ($proc.HasExited) { break }
    try { $hwnd = $proc.MainWindowHandle } catch {}
    if ($hwnd -ne [IntPtr]::Zero) { break }
    Start-Sleep -Milliseconds 150
  }
  # Settle: let the boot chain + render loop run.
  Start-Sleep -Seconds $WaitSec

  $crashedEarly = $false
  try { $proc.Refresh() } catch {}
  if ($proc.HasExited) { $crashedEarly = $true }

  $shotRes = "(skipped)"
  if (-not $crashedEarly -and $hwnd -ne [IntPtr]::Zero -and $i -le $Shots) {
    [void][W32]::SetWindowPos($hwnd, $HWND_TOPMOST, 0, 0, 0, 0, (0x0001 -bor 0x0002 -bor 0x0040)) # NOSIZE|NOMOVE|SHOWWINDOW
    Start-Sleep -Milliseconds 400
    $shotRes = Take-Shot $hwnd ("b17_{0}_run{1:D2}.png" -f $Tag, $i)
  }

  $exitcode = $null
  try { $proc.Refresh(); if ($proc.HasExited) { $exitcode = $proc.ExitCode } } catch {}
  try { Stop-Process -Id $procId -Force -ErrorAction SilentlyContinue } catch {}
  Start-Sleep -Milliseconds 300

  # Archive + parse the log.
  $reachedMain = $false; $chrome = "?"; $thunks = "?"; $mapText = "?"
  $perRunGran = @{}
  if (Test-Path $logsrc) {
    $dst = Join-Path $logdir ("run_{0:D2}.log" -f $i)
    Copy-Item $logsrc $dst -Force
    $txt = Get-Content $dst -Raw
    if ($txt -match 'B17-SUMMARY reached-main-loop chrome=(\w+) thunks=(\d+)/6') {
      $reachedMain = $true; $chrome = $Matches[1]; $thunks = $Matches[2]
    }
    if ($txt -match 'map_text=(\d)') { $mapText = $Matches[1] }
    # Parse post-initd3d9 granule states.
    foreach ($k in $granKeys) {
      $m = [regex]::Match($txt, ("B17-GRAN \[post-initd3d9\] 0x{0} (\w+)" -f $k))
      if ($m.Success) {
        $perRunGran[$k] = $m.Groups[1].Value
        if ($m.Groups[1].Value -ne 'COMMIT' -and $m.Groups[1].Value -ne 'FREE') {
          # FREE is fine (we can claim it); COMMIT-by-us is fine. Only non-claimable
          # = already COMMIT by something else at post-d3d9. But we can't tell owner
          # here; treat anything not FREE as "contended" for the histogram.
        }
      } else { $perRunGran[$k] = 'NA' }
    }
  }
  if ($reachedMain) { $bootOk++ }
  if ($chrome -eq 'YES') { $chromeOk++ }
  # Histogram: count granules that were NOT free at post-d3d9 (i.e. something
  # else already sat there — the contention we must beat).
  foreach ($k in $granKeys) {
    $st = $perRunGran[$k]
    if ($st -ne 'FREE' -and $st -ne 'NA') { $granBlocked[$k]++ }
  }

  $row = [PSCustomObject]@{
    run=$i; boot=$reachedMain; chrome=$chrome; thunks=$thunks; map_text=$mapText;
    early_crash=$crashedEarly; exit=$exitcode; shot=$shotRes
  }
  [void]$results.Add($row)
  Write-Host ("run {0,2}: boot={1,-5} chrome={2,-3} thunks={3} map_text={4} crash={5} {6}" -f `
    $i, $reachedMain, $chrome, $thunks, $mapText, $crashedEarly, $shotRes)
}

# Cleanup temp exe dir.
try { Remove-Item $tmpdir -Recurse -Force -ErrorAction SilentlyContinue } catch {}

Write-Host ""
Write-Host ("===== B17 [{0}] over {1} runs =====" -f $Tag, $Runs)
Write-Host ("boot-success : {0}/{1} ({2:P0})" -f $bootOk, $Runs, ($bootOk/$Runs))
Write-Host ("chrome=YES   : {0}/{1} ({2:P0})" -f $chromeOk, $Runs, ($chromeOk/$Runs))
Write-Host "per-granule NOT-FREE at post-initd3d9 (contention count):"
foreach ($k in $granKeys) {
  Write-Host ("  0x{0}: {1,2}/{2}" -f $k, $granBlocked[$k], $Runs)
}
$csv = Join-Path $logdir "_summary.csv"
$results | Export-Csv -Path $csv -NoTypeInformation
Write-Host "rows -> $csv"
Write-Host "logs -> $logdir"
