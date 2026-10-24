# d3rflame replay arms (PREREG_D3-CF-RFLAME.md section 4). One line per arm as it finishes.
#   pwsh -NoProfile -File verify\d3rflame_20261024\run_replay_arms.ps1
$ErrorActionPreference = 'Stop'
$here = $PSScriptRoot
$repo = Resolve-Path "$here\..\.."
$exe  = Join-Path $repo 're\tools\pu_replay\out\pu_replay.exe'
$out  = Join-Path $here 'arms'
if (-not (Test-Path $out)) { New-Item -ItemType Directory -Force $out | Out-Null }
$g3  = Join-Path $repo 'verify\d3_contact_20260928\g3.msd.puhook.csv'
$g3f = Join-Path $here 'g3f.msd.puhook.csv'
$arms = @(
    @{ n = 'G3_OFF';  in = $g3;  env = @{} },
    @{ n = 'G3_ON';   in = $g3;  env = @{ MASHED_RFLAME_SORT = '1' } },
    @{ n = 'F_OFF';   in = $g3f; env = @{} },
    @{ n = 'F_ON';    in = $g3f; env = @{ MASHED_RFLAME_SORT = '1' } },
    @{ n = 'F_ON_NB'; in = $g3f; env = @{ MASHED_RFLAME_SORT = '1'; MASHED_PU_FLAME_NOBIRTH = '1' } }
)
$i = 0
foreach ($a in $arms) {
    $i++
    foreach ($k in 'MASHED_RFLAME_SORT', 'MASHED_PU_FLAME_NOBIRTH') { Remove-Item "Env:$k" -ErrorAction SilentlyContinue }
    foreach ($k in $a.env.Keys) { Set-Item "Env:$k" $a.env[$k] }
    $env:MASHED_PU_STEPDUMP    = Join-Path $out "$($a.n).port.csv"
    $env:MASHED_PU_CONTACTDUMP = Join-Path $out "$($a.n).port.pucontact.csv"
    Remove-Item -Force $env:MASHED_PU_STEPDUMP, $env:MASHED_PU_CONTACTDUMP -ErrorAction SilentlyContinue
    $log = Join-Path $out "$($a.n).replay.txt"
    & $exe $a.in 0 2>&1 | Set-Content -Encoding utf8 $log
    $q  = (Select-String -Path $log -Pattern 'RFLAME query' | Select-Object -First 1).Line
    $cv = (Select-String -Path $log -Pattern '^CONTACT VERDICT: (\S+)' | Select-Object -First 1)
    $sc = (Select-String -Path $log -Pattern '^SORT CHECK: (.*)$' | Select-Object -First 1)
    $cvs = if ($cv) { $cv.Matches.Groups[1].Value } else { '(none)' }
    $scs = if ($sc) { $sc.Matches.Groups[1].Value } else { '(no puflame)' }
    Write-Host ("[{0}/{1}] {2} contact={3} sort={4} | {5}" -f $i, $arms.Count, $a.n, $cvs, $scs, ($q -replace '\s+', ' ').Trim())
}
foreach ($k in 'MASHED_RFLAME_SORT', 'MASHED_PU_FLAME_NOBIRTH', 'MASHED_PU_STEPDUMP', 'MASHED_PU_CONTACTDUMP') {
    Remove-Item "Env:$k" -ErrorAction SilentlyContinue
}
