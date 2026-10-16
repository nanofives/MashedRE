# txdscope: the measuring sequence after the baseline arm (serial; one game process at a time, own PIDs only).
#   pwsh -NoProfile -File verify\txdscope_20261016\run_txdscope.ps1 -Root <private gameroot> -Out log\txdscope
# Order: the 13 tracks (after), the opt-out control (MASHED_NO_TXDSCOPE=1, tracks 2,5,10,11,12,13), G-CTRL on the base exe, G-CTRL on the
# after exe. The base 13 tracks ran earlier (log\txdscope\base, exe_base.exe = build of 2cc15b2cf, SHA-256 E2102CEA...).
# The private root's mashed_re.log is archived into each arm's directory and removed between arms so each arm's lines stand alone.
param([Parameter(Mandatory)][string]$Root, [string]$Out = 'log\txdscope')
$ErrorActionPreference = 'Continue'
$wt = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
Set-Location $wt
$py = 'py'
$run = 'verify\rsky_20261016\run13.py'
function Archive-RootLog([string]$dest) {
    $l = Join-Path $Root 'mashed_re.log'
    if (Test-Path $l) { Copy-Item $l (Join-Path $dest 'mashed_re.log') -Force; Remove-Item $l -Force }
}
& $py -3.12 $run run --exe "$Out\exe_after.exe" --root $Root --out "$Out\after"
Archive-RootLog "$Out\after"
& $py -3.12 $run run --exe "$Out\exe_after.exe" --root $Root --out "$Out\ctl" --tracks 2,5,10,11,12,13 MASHED_NO_TXDSCOPE=1
Archive-RootLog "$Out\ctl"
& $py -3.12 verify\rsky_20261016\run_gctrl_priv.py "$Out\exe_base.exe" base --root $Root --out "$Out\gctrl"
& $py -3.12 verify\rsky_20261016\run_gctrl_priv.py "$Out\exe_after.exe" after --root $Root --out "$Out\gctrl"
'RUN_TXDSCOPE DONE'
