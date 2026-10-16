# rsky: the measuring sequence after the rebase onto f76ae153b (baseline built from THAT base in a scratch worktree, MASHED_NO_DEPLOY=1).
#   pwsh -NoProfile -File verify\rsky_20261016\run_final2.ps1 -Root <private gameroot> -Out log\rsky
# Serial, one game process at a time, own PIDs only. exe_base2 = clean build of f76ae153b; exe_after5 = work/rsky on f76ae153b.
param([Parameter(Mandatory)][string]$Root, [string]$Out = 'log\rsky')
$ErrorActionPreference = 'Continue'
$wt = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
Set-Location $wt
$py = 'py'
& $py -3.12 verify\rsky_20261016\run13.py run --exe "$Out\exe_base2.exe"  --root $Root --out "$Out\base2"
& $py -3.12 verify\rsky_20261016\run13.py run --exe "$Out\exe_base2.exe"  --root $Root --out "$Out\base2_n"  --tracks 20
& $py -3.12 verify\rsky_20261016\run13.py run --exe "$Out\exe_after5.exe" --root $Root --out "$Out\after5_n" --tracks 20
& $py -3.12 verify\rsky_20261016\run13.py run --exe "$Out\exe_after5.exe" --root $Root --out "$Out\after5"
& $py -3.12 verify\rsky_20261016\run13.py run --exe "$Out\exe_after5.exe" --root $Root --out "$Out\ctl2" --tracks 2,5,7,9,10,11,12 `
    MASHED_NO_SKYSLOTS=1 MASHED_NO_OBJBSP=1 MASHED_NO_SECTOR_BINMESH=1
& $py -3.12 verify\rsky_20261016\run_gctrl_priv.py "$Out\exe_base2.exe"  base2  --root $Root --out "$Out\gctrl2"
& $py -3.12 verify\rsky_20261016\run_gctrl_priv.py "$Out\exe_after5.exe" after5 --root $Root --out "$Out\gctrl2"
'RUN_FINAL2 DONE'
