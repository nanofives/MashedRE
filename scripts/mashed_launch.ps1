# mashed_launch.ps1 — one-click QoL launcher for patched original\MASHED.exe.
# Plan: re/analysis/QOL_PATCH_PLAN_2026-08.md (Item 2).
#
# Profiles:
#   pc  — desktop monitor (target 165 Hz)
#   tv  — living-room TV  (target 120 Hz)
#
# Until the fixed-tick decouple (plan Item 1) is live, the game's speed is tied to
# framerate, so the FPS cap stays at 60 regardless of profile target; flip
# $DecoupleReady below when mashed_qol.asi gains MASHED_TICK_HZ support.
#
# Defaults per launch: everything unlocked (MASHED_UNLOCK), no gamesave.bin writes
# (MASHED_NO_SAVE), dev RE hooks kept out (MASHED_RE_NO_AUTO_HOOK). All are
# runtime-only — nothing on disk is modified by launching.
#
# Usage:
#   pwsh scripts\mashed_launch.ps1                 # pc profile
#   pwsh scripts\mashed_launch.ps1 -Profile tv
#   pwsh scripts\mashed_launch.ps1 -FpsCap 60      # explicit cap override
#   pwsh scripts\mashed_launch.ps1 -EnableSave     # allow gamesave.bin writes
#   pwsh scripts\mashed_launch.ps1 -NoUnlock       # stock lock state
#   pwsh scripts\mashed_launch.ps1 -Repatch        # run repatch_original.py first

[CmdletBinding()]
param(
    [ValidateSet('pc', 'tv')]
    [string]$Profile = 'pc',
    [int]$FpsCap = 0,          # 0 = use profile logic
    [int]$MenuCap = 0,         # 0 = follow profile target; e.g. 60 = classic menu pace
                               # (menu logic is per-frame-coupled: higher cap = faster menus)
    [string]$Res = '',         # 'WxH' override; '' = profile default; '0' = classic 640x480 titled window
    [switch]$EnableSave,
    [switch]$NoUnlock,
    [switch]$NoJumpFix,    # opt out of the ramp-takeoff velocity fix (on by default; verified 2026-08-02)
    [switch]$Osd,          # start with the FPS/ms overlay visible (F11 toggles it any time)
    [switch]$NoShadowInterp,  # A/B: disable the shadow-anchor interpolation
    [switch]$WheelInterp,     # A/B: interpolate wheel local matrices (spin/suspension)
    [switch]$Repatch,
    [switch]$QolLog
)

$ErrorActionPreference = 'Stop'
$Root = Split-Path -Parent $PSScriptRoot
$Orig = Join-Path $Root 'original'

# Decouple landed 2026-08-01 (mashed_qol.asi MASHED_DECOUPLE + d3d9 shim
# MASHED_FPS_CAP_RACE): menus stay at 60, races run at the profile target.
$DecoupleReady = $true

$ProfileTargets = @{ pc = 165; tv = 120 }
# Borderless play resolution per profile (MASHED_RES + MASHED_QOL_BORDERLESS;
# the qol .asi retargets the screen-dim getters to match the shim backbuffer).
$ProfileRes     = @{ pc = '2560x1440'; tv = '1920x1080' }

# ── sanity: required deploy artifacts ────────────────────────────────────────
$required = @('MASHED.exe', 'MASHED.exe.unpatched', 'd3d9.dll', 'dinput8.dll', 'mashed_qol.asi')
$missing = $required | Where-Object { -not (Test-Path (Join-Path $Orig $_)) }
if ($missing) {
    Write-Error ("missing in original\: {0}. Run repatch_original.py / build_d3d9_shim.bat / " +
        "build_dinput8_shim.bat / build_qol_asi.bat first." -f ($missing -join ', '))
}

if ($Repatch) {
    Write-Host '== repatch_original.py (idempotent) =='
    py -3.12 (Join-Path $Root 'scripts\repatch_original.py')
    if ($LASTEXITCODE -ne 0) { Write-Error 'repatch_original.py failed' }
}

# ── videocfg: ensure the canonical windowed config is in place ───────────────
$canon = Join-Path $Root 'scripts\canonical\videocfg_windowed.bin'
$vcfg  = Join-Path $Orig 'videocfg.bin'
if (Test-Path $canon) {
    $same = (Test-Path $vcfg) -and
        ((Get-FileHash $canon).Hash -eq (Get-FileHash $vcfg).Hash)
    if (-not $same) {
        Copy-Item $canon $vcfg -Force
        Write-Host 'videocfg.bin: restored canonical windowed config'
    }
}

# ── resolve FPS caps ─────────────────────────────────────────────────────────
# Race cap = profile target (the decouple makes race speed framerate-independent).
# Menu cap follows it by default (menus run proportionally faster — see -MenuCap).
$target = $ProfileTargets[$Profile]
$raceCap = if ($FpsCap -gt 0) { $FpsCap } elseif ($DecoupleReady) { $target } else { 60 }

# ── environment for the child process ────────────────────────────────────────
$resolved = if ($Res -eq '0') { '' } elseif ($Res) { $Res } else { $ProfileRes[$Profile] }
if ($resolved) {
    $env:MASHED_RES            = $resolved
    $env:MASHED_QOL_BORDERLESS = '1'
} else {
    Remove-Item Env:MASHED_RES            -ErrorAction SilentlyContinue
    Remove-Item Env:MASHED_QOL_BORDERLESS -ErrorAction SilentlyContinue
}

# Menu/frontend cap. Menus are per-frame-coupled (no tick quantizer outside
# the race phases), so raising this speeds menu animation/nav proportionally —
# uncapped-to-target reads as "snappy". -MenuCap 60 restores the classic pace.
$menuCap = if ($MenuCap -gt 0) { $MenuCap } else { $raceCap }
$env:MASHED_FPS_CAP         = "$menuCap"
$env:MASHED_FPS_CAP_RACE    = if ($DecoupleReady) { "$raceCap" } else { $null }
$env:MASHED_DECOUPLE        = if ($DecoupleReady) { '1' } else { '0' }
$env:MASHED_INTERP          = if ($DecoupleReady) { '1' } else { '0' }  # smooth >60fps camera
$env:MASHED_NO_SAVE         = if ($EnableSave) { '0' } else { '1' }
$env:MASHED_UNLOCK          = if ($NoUnlock)   { '0' } else { '1' }
$env:MASHED_JUMPFIX         = if ($NoJumpFix) { '0' } else { '1' }  # ramp-takeoff velocity fix (verified)
$env:MASHED_FPS_OSD         = if ($Osd) { '1' } else { '0' }  # F11 toggles in-game either way
$env:MASHED_INTERP_SHADOW   = if ($NoShadowInterp) { '0' } else { '1' }
$env:MASHED_INTERP_WHEELS   = if ($WheelInterp) { '1' } else { '0' }
$env:MASHED_RE_NO_AUTO_HOOK = '1'    # keep dev RE hooks out of play sessions
$env:MASHED_QOL_LOG         = if ($QolLog) { '1' } else { '0' }

$resLabel = if ($resolved) { "$resolved borderless" } else { '640x480 windowed' }
Write-Host ("profile={0}  res={1}  menu_cap={2}  race_cap={3}  decouple={4}  no_save={5}  unlock={6}" -f
    $Profile, $resLabel, $menuCap, $raceCap, $env:MASHED_DECOUPLE, $env:MASHED_NO_SAVE, $env:MASHED_UNLOCK)

$p = Start-Process -FilePath (Join-Path $Orig 'MASHED.exe') -WorkingDirectory $Orig -PassThru
Write-Host ("MASHED.exe started, PID {0}" -f $p.Id)
