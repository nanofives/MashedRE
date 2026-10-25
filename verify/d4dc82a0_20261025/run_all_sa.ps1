# One lock hold for every standalone run of this lane: baseline and consolidated exe on screen 19, the
# consolidated exe on screens 30 and 32 (the arrow_fix lane's three screens).
$V = 'C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\D4-DC-82a0\verify\d4dc82a0_20261025'
$D = 'D:\MashedWT\D4-DC-82a0_data'
foreach ($r in @(@('base', '19'), @('after', '19'), @('after', '30'), @('after', '32'))) {
    $k = $r[0]; $s = $r[1]
    "== $k screen $s"
    & pwsh -NoProfile -File "$V\run_sa.ps1" -Exe "$D\$k\mashed_re.exe" -Screen $s -Out "$V\sa${s}_$k.json" -Log "$V\sa${s}_$k.log"
}
