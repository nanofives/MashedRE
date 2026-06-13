# capture_window.ps1 — capture a window's PRESENTED content (post-Present,
# post-DWM) to a PNG, even when the window is in the BACKGROUND (occluded by
# other windows; minimized does NOT work — DWM keeps no surface for minimized
# windows).
#
# Why: plain BitBlt/GetDC captures of D3D9 windows render WHITE on this
# machine (the long-standing "window screenshots are untrustworthy" issue).
# PrintWindow with PW_RENDERFULLCONTENT (0x2, Win 8.1+) asks DWM to render
# the window's actual composed surface instead, which works for GPU swap
# chains. This captures what the USER sees (including any DWM scaling) —
# the verification channel the backbuffer dump (MASHED_DBG_BBDUMP) cannot
# provide.
#
# Usage:
#   pwsh scripts/capture_window.ps1 -Title "Mashed"            -Out verify/win.png
#   pwsh scripts/capture_window.ps1 -ProcessId 1234            -Out verify/win.png
#   pwsh scripts/capture_window.ps1 -Title "MASHED [D3D9]"     -Out verify/orig_win.png
# Exit 0 on success; the PNG is the window's client+frame at physical pixels.

param(
    [string]$Title = "",
    [int]$ProcessId = 0,
    [string]$Out = "verify/window_capture.png"
)

$ErrorActionPreference = "Stop"

Add-Type @'
using System;
using System.Runtime.InteropServices;
public static class WinCap {
    [DllImport("user32.dll")] public static extern bool SetProcessDPIAware();
    [DllImport("user32.dll", SetLastError=true)]
    public static extern bool PrintWindow(IntPtr hwnd, IntPtr hdc, uint flags);
    [DllImport("user32.dll")]
    public static extern bool GetWindowRect(IntPtr hwnd, out RECT rect);
    [DllImport("user32.dll")] public static extern bool IsIconic(IntPtr hwnd);
    [StructLayout(LayoutKind.Sequential)]
    public struct RECT { public int L, T, R, B; }
}
'@

# Physical-pixel coordinates regardless of the host's DPI virtualization.
[WinCap]::SetProcessDPIAware() | Out-Null

# Resolve the target HWND.
$hwnd = [IntPtr]::Zero
if ($ProcessId -ne 0) {
    $proc = Get-Process -Id $ProcessId
    $hwnd = $proc.MainWindowHandle
} elseif ($Title -ne "") {
    $cands = Get-Process | Where-Object {
        $_.MainWindowHandle -ne 0 -and $_.MainWindowTitle -like "*$Title*"
    }
    if (-not $cands) { Write-Error "no window matching title '*$Title*'"; exit 1 }
    $hwnd = ($cands | Select-Object -First 1).MainWindowHandle
} else {
    Write-Error "pass -Title or -ProcessId"; exit 1
}
if ($hwnd -eq [IntPtr]::Zero) { Write-Error "target has no main window"; exit 1 }
if ([WinCap]::IsIconic($hwnd)) {
    Write-Error "window is MINIMIZED - DWM keeps no surface; restore it first"
    exit 1
}

$rect = New-Object WinCap+RECT
[WinCap]::GetWindowRect($hwnd, [ref]$rect) | Out-Null
$w = $rect.R - $rect.L; $h = $rect.B - $rect.T
if ($w -le 0 -or $h -le 0) { Write-Error "degenerate window rect ${w}x${h}"; exit 1 }

Add-Type -AssemblyName System.Drawing
$bmp = New-Object System.Drawing.Bitmap($w, $h)
$gfx = [System.Drawing.Graphics]::FromImage($bmp)
$hdc = $gfx.GetHdc()
# PW_RENDERFULLCONTENT = 0x2: DWM-composed surface (works for D3D swap chains
# and for occluded/background windows).
$ok = [WinCap]::PrintWindow($hwnd, $hdc, 2)
$gfx.ReleaseHdc($hdc)
$gfx.Dispose()
if (-not $ok) { $bmp.Dispose(); Write-Error "PrintWindow failed"; exit 1 }

$dir = Split-Path -Parent $Out
if ($dir -and -not (Test-Path $dir)) { New-Item -ItemType Directory -Path $dir | Out-Null }
$bmp.Save($Out, [System.Drawing.Imaging.ImageFormat]::Png)
$bmp.Dispose()
Write-Output "captured ${w}x${h} -> $Out"
exit 0
