# rsky-verify 2: the measuring sequence on race/first-frame-parity 32931432 (d2close merged). Same recipe as run_verify.ps1; the control for the
# row delta is the earlier merged-tip measurement (log\rsky\verify, 2dec160eb) and the all-knobs-off arm.
param([Parameter(Mandatory)][string]$Root, [string]$Out = 'log\rsky')
$ErrorActionPreference = 'Continue'
$wt = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
Set-Location $wt
$py = 'py'
$run = 'verify\rsky_20261016\run13.py'
& $py -3.12 $run run --exe "$Out\exe_verify2.exe" --root $Root --out "$Out\verify2"
& $py -3.12 $run run --exe "$Out\exe_verify2.exe" --root $Root --out "$Out\verify2_n" --tracks 20
& $py -3.12 $run run --exe "$Out\exe_verify2.exe" --root $Root --out "$Out\ctl2" --tracks 2,5,7,9,10,11,12 `
    MASHED_NO_SKYSLOTS=1 MASHED_NO_OBJBSP=1 MASHED_NO_SECTOR_BINMESH=1 MASHED_NO_HOOKSLOTS=1 MASHED_NO_CLUMPANIM=1 MASHED_NO_SCENARIOVIS=1
& $py -3.12 verify\rsky_20261016\run_gctrl_priv.py "$Out\exe_verify2.exe" verify2 --root $Root --out "$Out\gctrl2"
'RUN_VERIFY2 DONE'
