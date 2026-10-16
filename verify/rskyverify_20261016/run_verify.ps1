# rsky-verify: the measuring sequence on the merged tip 2dec160eb against its parent-side control 042acdac4 (both clean builds, MASHED_NO_DEPLOY=1).
param([Parameter(Mandatory)][string]$Root, [string]$Out = 'log\rsky')
$ErrorActionPreference = 'Continue'
$wt = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
Set-Location $wt
$py = 'py'
$run = 'verify\rsky_20261016\run13.py'
& $py -3.12 $run run --exe "$Out\exe_verify.exe" --root $Root --out "$Out\verify"
& $py -3.12 $run run --exe "$Out\exe_verify.exe" --root $Root --out "$Out\verify_n" --tracks 20
& $py -3.12 $run run --exe "$Out\exe_pre.exe"    --root $Root --out "$Out\pre"
& $py -3.12 $run run --exe "$Out\exe_pre.exe"    --root $Root --out "$Out\pre_n"    --tracks 20
& $py -3.12 $run run --exe "$Out\exe_verify.exe" --root $Root --out "$Out\ctl" --tracks 2,5,7,9,10,11,12 `
    MASHED_NO_SKYSLOTS=1 MASHED_NO_OBJBSP=1 MASHED_NO_SECTOR_BINMESH=1 MASHED_NO_HOOKSLOTS=1 MASHED_NO_CLUMPANIM=1 MASHED_NO_SCENARIOVIS=1
& $py -3.12 verify\rsky_20261016\run_gctrl_priv.py "$Out\exe_verify.exe" verify --root $Root --out "$Out\gctrl"
'RUN_VERIFY DONE'
