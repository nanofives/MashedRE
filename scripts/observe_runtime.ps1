# observe_runtime.ps1 — plain-launch runtime observation (NO Frida).
# Launches MASHED.exe normally, polls window-truth on a timeline, screenshots
# at intervals into verify/, and captures the process ExitCode on exit.
# ExitCode 0 => clean exit/quit; 0xC0000005-style => crash.
#
# Usage:  pwsh scripts\observe_runtime.ps1 [maxSeconds]
param([int]$MaxSeconds = 130)

Add-Type @"
using System;
using System.Runtime.InteropServices;
public class U32 {
  [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr hWnd, out RECT lpRect);
  [StructLayout(LayoutKind.Sequential)] public struct RECT { public int Left, Top, Right, Bottom; }
}
"@
Add-Type -AssemblyName System.Drawing

$root    = "C:\Users\maria\Desktop\Proyectos\Mashed"
$exe     = Join-Path $root "original\MASHED.exe"
$wd      = Join-Path $root "original"
$verify  = Join-Path $root "verify"
$logfile = Join-Path $root "log\runtime_timeline.txt"

function Get-Rect($hwnd) {
  $r = New-Object U32+RECT
  if ([U32]::GetWindowRect($hwnd, [ref]$r)) {
    return @{ ok=$true; w=($r.Right-$r.Left); h=($r.Bottom-$r.Top); l=$r.Left; t=$r.Top }
  }
  return @{ ok=$false }
}

function Take-Shot($hwnd, $name) {
  $rc = Get-Rect $hwnd
  if (-not $rc.ok)            { return "rect-fail" }
  if ($rc.w -le 0 -or $rc.h -le 0) { return ("rect 0x0 ({0}x{1})" -f $rc.w,$rc.h) }
  try {
    $bmp = New-Object System.Drawing.Bitmap $rc.w, $rc.h
    $g   = [System.Drawing.Graphics]::FromImage($bmp)
    $g.CopyFromScreen($rc.l, $rc.t, 0, 0, (New-Object System.Drawing.Size $rc.w, $rc.h))
    $path = Join-Path $verify $name
    $bmp.Save($path, [System.Drawing.Imaging.ImageFormat]::Png)
    $g.Dispose(); $bmp.Dispose()
    return ("saved {0} ({1}x{2})" -f $name,$rc.w,$rc.h)
  } catch { return "shot-exc: $_" }
}

$proc = Start-Process -FilePath $exe -WorkingDirectory $wd -PassThru
$procId = $proc.Id
$sw = [System.Diagnostics.Stopwatch]::StartNew()
$lines = New-Object System.Collections.ArrayList
[void]$lines.Add("launch pid=$procId at $(Get-Date -Format o)")
Write-Host "launch pid=$procId"
$nextShot = 7.0

while ($true) {
  try { $proc.Refresh() } catch {}
  if ($proc.HasExited) { break }
  $t = [math]::Round($sw.Elapsed.TotalSeconds,1)
  $title=""; $hwnd=[IntPtr]::Zero; $rectstr=""; $resp="?"
  try { $title = $proc.MainWindowTitle } catch {}
  try { $hwnd  = $proc.MainWindowHandle } catch {}
  if ($hwnd -ne [IntPtr]::Zero) {
    $rc = Get-Rect $hwnd
    if ($rc.ok) { $rectstr = ("{0}x{1}@{2},{3}" -f $rc.w,$rc.h,$rc.l,$rc.t) }
  }
  $line = "t=$t title='$title' hwnd=$hwnd rect=$rectstr"
  [void]$lines.Add($line); Write-Host $line
  if ($t -ge $nextShot -and $hwnd -ne [IntPtr]::Zero) {
    $res = Take-Shot $hwnd ("scene_t{0:D3}.png" -f [int]$t)
    [void]$lines.Add("  SHOT: $res"); Write-Host "  SHOT: $res"
    $nextShot = $t + 4.0
  }
  if ($t -gt $MaxSeconds) {
    [void]$lines.Add("timeout ${MaxSeconds}s, killing"); Write-Host "timeout, killing"
    try { Stop-Process -Id $procId -Force } catch {}
    break
  }
  Start-Sleep -Milliseconds 1200
}

try { $proc.Refresh() } catch {}
if ($proc.HasExited) {
  $code = $proc.ExitCode
  $t = [math]::Round($sw.Elapsed.TotalSeconds,1)
  $hex = ('0x{0:X8}' -f $code)
  $msg = "EXITED at t=$t exitcode=$code ($hex)"
  [void]$lines.Add($msg); Write-Host $msg
}
$lines | Out-File -FilePath $logfile -Encoding utf8
Write-Host "log -> $logfile"
