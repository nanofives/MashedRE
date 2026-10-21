# Replay + decision-diff every power-up capture in one pass (D3 powerups).
#
#   pwsh -NoProfile -File re\tools\pu_replay\sweep.ps1 [-Exe <pu_replay.exe>] [-Out <dir>]
#
# For each capture it runs pu_replay.exe (which prints the CONTACT verdict) and
# then re/tools/pu_diff.py (the 9-type DECISION verdict), and prints one row per
# capture. Outputs land in -Out (default: a scratch dir), never over the committed
# .port.csv / .replay.txt evidence unless -Out points there.
param(
    [string]$Exe = "$PSScriptRoot\out\pu_replay.exe",
    [string]$Out = "$PSScriptRoot\sweep_out"
)
$ErrorActionPreference = 'Stop'
$repo = Resolve-Path "$PSScriptRoot\..\..\.."
$caps = @(
    @{ n = 'o3'; p = 'verify/d3_pu_20260926/o3.msd' },
    @{ n = 'o4'; p = 'verify/d3_pu_20260926/o4.msd' },
    @{ n = 'c2'; p = 'verify/d3_contact_20260927/c2.msd' },
    @{ n = 'c3'; p = 'verify/d3_contact_20260927/c3.msd' },
    @{ n = 'g2'; p = 'verify/d3_contact_20260928/g2.msd' },
    @{ n = 'g3'; p = 'verify/d3_contact_20260928/g3.msd' },
    @{ n = 'g4'; p = 'verify/d3_contact_20260928/g4.msd' },
    # m1 / m2 / s1 / s2: the 2026-09-28b .puhook.csv were lost; these are the d3close re-captures
    # from the same argv (verify/d3close_20261021/RESULT_d3close.md). b3r forces box state 3 (--pu-box).
    @{ n = 'm1r'; p = 'verify/d3close_20261021/pu/m1r.msd' },
    @{ n = 'm2r'; p = 'verify/d3close_20261021/pu/m2r.msd' },
    @{ n = 's1r'; p = 'verify/d3close_20261021/pu/s1r.msd' },
    @{ n = 's2r'; p = 'verify/d3close_20261021/pu/s2r.msd' },
    @{ n = 'b3r'; p = 'verify/d3close_20261021/pu/b3r.msd' }
)
if (-not (Test-Path $Out)) { New-Item -ItemType Directory -Force $Out | Out-Null }
$rows = @()
foreach ($c in $caps) {
    $src = Join-Path $repo $c.p
    if (-not (Test-Path "$src.puhook.csv")) { Write-Host "$($c.n): MISSING"; continue }
    $port = Join-Path $Out "$($c.n).port.csv"
    $pcx  = Join-Path $Out "$($c.n).port.pucontact.csv"
    $log  = Join-Path $Out "$($c.n).replay.txt"
    $dif  = Join-Path $Out "$($c.n).diff.txt"
    Remove-Item -Force $port, $pcx -ErrorAction SilentlyContinue
    $env:MASHED_PU_STEPDUMP = $port
    $env:MASHED_PU_CONTACTDUMP = $pcx
    & $Exe "$src.puhook.csv" 0 2>&1 | Set-Content -Encoding utf8 $log
    Remove-Item Env:MASHED_PU_STEPDUMP, Env:MASHED_PU_CONTACTDUMP
    & py -3.12 (Join-Path $repo 're/tools/pu_diff.py') "$src.puhook.csv" $port --slot 0 2>&1 |
        Set-Content -Encoding utf8 $dif
    $m = Select-String -Path $log -Pattern '^CONTACT VERDICT: (.*)$' | Select-Object -First 1
    $cv = if ($m) { $m.Matches.Groups[1].Value } else { '(none)' }
    $m = Select-String -Path $dif -Pattern '^VERDICT: (.*)$' | Select-Object -First 1
    $dv = if ($m) { $m.Matches.Groups[1].Value } else { '(none)' }
    $rows += [pscustomobject]@{ capture = $c.n; decision = $dv; contact = $cv }
}
$rows | Format-Table -AutoSize
