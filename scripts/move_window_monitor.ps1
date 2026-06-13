# move_window_monitor.ps1 — move a process's main window onto a NON-PRIMARY
# monitor so test launches don't interrupt the user's main screen.
#
# Usage:  pwsh scripts/move_window_monitor.ps1 -ProcessId <pid> [-Retry 20]
# No-op (exit 0 with a note) on single-monitor setups.

param(
    [Parameter(Mandatory=$true)][int]$ProcessId,
    [int]$Retry = 20
)
$ErrorActionPreference = "Stop"

Add-Type @'
using System;
using System.Runtime.InteropServices;
public static class WinMove {
    [DllImport("user32.dll")] public static extern bool SetProcessDPIAware();
    [DllImport("user32.dll", SetLastError=true)]
    public static extern bool SetWindowPos(IntPtr hWnd, IntPtr after, int x, int y,
                                           int cx, int cy, uint flags);
}
'@
[WinMove]::SetProcessDPIAware() | Out-Null
Add-Type -AssemblyName System.Windows.Forms

$sec = [System.Windows.Forms.Screen]::AllScreens | Where-Object { -not $_.Primary } | Select-Object -First 1
if (-not $sec) { Write-Output "single monitor - leaving window in place"; exit 0 }

$hwnd = [IntPtr]::Zero
for ($i = 0; $i -lt $Retry; $i++) {
    try {
        $p = Get-Process -Id $ProcessId -ErrorAction Stop
        $p.Refresh()
        if ($p.MainWindowHandle -ne 0) { $hwnd = $p.MainWindowHandle; break }
    } catch { break }
    Start-Sleep -Milliseconds 250
}
if ($hwnd -eq [IntPtr]::Zero) { Write-Error "no main window for pid $ProcessId"; exit 1 }

$x = $sec.Bounds.X + 40; $y = $sec.Bounds.Y + 40
# SWP_NOSIZE(1) | SWP_NOZORDER(4) | SWP_NOACTIVATE(0x10) — move without
# resizing and WITHOUT stealing focus from the user's screen.
[WinMove]::SetWindowPos($hwnd, [IntPtr]::Zero, $x, $y, 0, 0, 0x15) | Out-Null
Write-Output "moved pid $ProcessId window to monitor at $($sec.Bounds.X),$($sec.Bounds.Y)"
exit 0
