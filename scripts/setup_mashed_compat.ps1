# Mashed AppCompat shim setup. Run once per machine, no admin needed.
#
# Sets the registry layer that lets MASHED.exe progress past D3DX texture
# init on Win11. RUNASINVOKER suppresses UAC elevation that the other
# tokens would otherwise trigger, so spawning MASHED via subprocess from
# a non-elevated shell works (required for Frida verification harnesses
# under re/frida/).
#
# Tokens:
#   RUNASINVOKER                  - tell Windows to NOT elevate, ever
#   WIN98RTM                      - Win98 emulation (RW3-friendly)
#   HIGHDPIAWARE                  - prevent auto-DPI-scale
#   DISABLEDXMAXIMIZEDWINDOWEDMODE - disable DWM maximized-windowed D3D9
#   EMULATEHEAP                   - XP-style heap (helps some 2000s games)

$mashedPath = (Resolve-Path "$PSScriptRoot\..\original\MASHED.exe").Path
$layersKey  = "HKCU:\Software\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Layers"
$layer      = "~ RUNASINVOKER WIN98RTM HIGHDPIAWARE DISABLEDXMAXIMIZEDWINDOWEDMODE EMULATEHEAP"

if (-not (Test-Path $layersKey)) {
    New-Item -Path $layersKey -Force | Out-Null
}
Set-ItemProperty -Path $layersKey -Name $mashedPath -Value $layer -Type String

Write-Host "AppCompat layer set on $mashedPath"
Write-Host "  -> $layer"
Write-Host ""
Write-Host "Verify by running (from non-elevated shell):"
Write-Host "  py -3.12 re\frida\run_diff.py vec3_magnitude"
