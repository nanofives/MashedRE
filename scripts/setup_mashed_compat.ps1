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
# EMULATEHEAP — BUILD-DEPENDENT, the relationship INVERTS across Win11 builds:
#   * Build 26100 (dropped 2026-06-16): WIN98RTM + EMULATEHEAP together smash a
#     heap free-list pointer during CRT init -> 0xC0000005 WRITE in
#     ntdll!RtlpHeap (+0x542f0) ~4s in, before the menu. Booted clean WITHOUT it.
#   * Build 26200 (re-added 2026-06-16, same day, after an overnight feature
#     update 26100->26200 + reboot): now the *native* heap is what MASHED's
#     legacy MSVC CRT init corrupts (IDENTICAL signature: ntdll +0x542f0,
#     ECX=0x5477, heap base 0x30000, CRT ret-chain 0x4ac660/0x4a5f49/0x4a4274/
#     0x49644c) regardless of compat layer / apphelp / our .asi / our d3d9 proxy.
#     EMULATEHEAP (legacy heap emulation) side-steps it -> boots + run_diff GREEN.
# So the shim is toggled on OS build below. If a future update inverts this AGAIN
# (boot AV at ntdll +0x542f0 with the layer "correct"), flip $useEmulateHeap and
# re-diagnose with scripts/parse_minidump.py (newest %LOCALAPPDATA%\CrashDumps
# dump): identical ECX=0x5477 / CRT ret-chain == this same heap-inversion class.

$mashedPath = (Resolve-Path "$PSScriptRoot\..\original\MASHED.exe").Path
$layersKey  = "HKCU:\Software\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Layers"
# Observed inversion point: native heap corrupts on 26200+, so EMULATEHEAP helps.
$build = [int]([Environment]::OSVersion.Version.Build)
$useEmulateHeap = ($build -ge 26200)
$layer = if ($useEmulateHeap) { "~ RUNASINVOKER WIN98RTM HIGHDPIAWARE EMULATEHEAP" }
         else                  { "~ RUNASINVOKER WIN98RTM HIGHDPIAWARE" }
Write-Host "OS build $build -> EMULATEHEAP $(if($useEmulateHeap){'ON'}else{'OFF'})"

if (-not (Test-Path $layersKey)) {
    New-Item -Path $layersKey -Force | Out-Null
}
Set-ItemProperty -Path $layersKey -Name $mashedPath -Value $layer -Type String

# Clear the Program Compatibility Assistant (PCA) record for MASHED. After
# repeated crashes (e.g. during Frida diff iteration) PCA flags the exe in its
# Store and applies a heap/FTH-style compat shim OUTSIDE the Layers key above
# that re-breaks boot (0xC0000005 in ntdll heap during CRT init) — and setting
# the layer alone does NOT undo it. Root-caused 2026-06-16. Idempotent.
$pcaStore = "HKCU:\Software\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Compatibility Assistant\Store"
if ((Test-Path $pcaStore) -and ((Get-Item $pcaStore).Property -contains $mashedPath)) {
    Remove-ItemProperty -Path $pcaStore -Name $mashedPath -Force
    Write-Host "Cleared PCA Store record for MASHED.exe (post-crash compat shim)"
}

Write-Host "AppCompat layer set on $mashedPath"
Write-Host "  -> $layer"
Write-Host ""
Write-Host "Verify by running (from non-elevated shell):"
Write-Host "  py -3.12 re\frida\run_diff.py vec3_magnitude"
