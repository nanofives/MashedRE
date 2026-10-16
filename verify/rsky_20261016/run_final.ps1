# rsky: the measuring sequence of the final arm (serial; one game process at a time, own PIDs only).
#   pwsh -NoProfile -File verify\rsky_20261016\run_final.ps1 -Root <private gameroot> -Out log\rsky
# Order: new Neustein sample (baseline, then after), the 13 tracks (after), the opt-out control (every knob off) on the tracks the
# change moved, G-CTRL (after). The baseline 13 tracks and its G-CTRL ran earlier (log\rsky\base, log\rsky\gctrl).
param([Parameter(Mandatory)][string]$Root, [string]$Out = 'log\rsky')
$ErrorActionPreference = 'Continue'
$wt = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
Set-Location $wt
$py = 'py'
& $py -3.12 verify\rsky_20261016\run13.py run --exe "$Out\exe_base.exe"   --root $Root --out "$Out\base_n"   --tracks 20
& $py -3.12 verify\rsky_20261016\run13.py run --exe "$Out\exe_after4.exe" --root $Root --out "$Out\after4_n" --tracks 20
& $py -3.12 verify\rsky_20261016\run13.py run --exe "$Out\exe_after4.exe" --root $Root --out "$Out\after4"
& $py -3.12 verify\rsky_20261016\run13.py run --exe "$Out\exe_after4.exe" --root $Root --out "$Out\ctl" --tracks 2,5,7,9,10,11,12 `
    MASHED_NO_SKYSLOTS=1 MASHED_NO_OBJBSP=1 MASHED_NO_SECTOR_BINMESH=1
& $py -3.12 verify\rsky_20261016\run_gctrl_priv.py "$Out\exe_after4.exe" after4 --root $Root --out "$Out\gctrl"
'RUN_FINAL DONE'
