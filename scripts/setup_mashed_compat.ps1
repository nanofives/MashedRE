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
#
# DROPPED 2026-05-15:
#   DISABLEDXMAXIMIZEDWINDOWEDMODE  conflicts with mashedmod d3d9_shim
#     (AcLayers's d3d9 export hooks deadlock against our proxy at
#     process init; see mashedmod/src/d3d9_shim/d3d9_shim.cpp). The
#     proxy forces Windowed=TRUE at CreateDevice anyway, which is what
#     this shim was working around.
# DROPPED 2026-06-16:
#   EMULATEHEAP                     heap-corrupts MASHED at boot under the
#     current Win11 ntdll (10.0.26100.x). WIN98RTM + EMULATEHEAP together
#     smash a heap free-list pointer during CRT init -> 0xC0000005 WRITE in
#     ntdll!RtlpHeap (+0x542f0) ~4s in, before the menu. Removing EMULATEHEAP
#     (keeping WIN98RTM) boots clean AND survives a live race (verified via
#     run_diff: sa1 hooks GREEN 4/4). Root cause + minidump analysis 2026-06-16.

$mashedPath = (Resolve-Path "$PSScriptRoot\..\original\MASHED.exe").Path
$layersKey  = "HKCU:\Software\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Layers"
$layer      = "~ RUNASINVOKER WIN98RTM HIGHDPIAWARE"

if (-not (Test-Path $layersKey)) {
    New-Item -Path $layersKey -Force | Out-Null
}
Set-ItemProperty -Path $layersKey -Name $mashedPath -Value $layer -Type String

Write-Host "AppCompat layer set on $mashedPath"
Write-Host "  -> $layer"
Write-Host ""
Write-Host "Verify by running (from non-elevated shell):"
Write-Host "  py -3.12 re\frida\run_diff.py vec3_magnitude"
