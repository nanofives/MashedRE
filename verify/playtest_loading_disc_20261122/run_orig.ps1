param([int]$RunS = 30, [string]$OutDir, [double]$Every = 1.0, [double]$StartS = 2.0)
$root = "C:\Users\maria\Desktop\Proyectos\Mashed"
$req = "$OutDir\bbreq.txt"
Remove-Item $req -ErrorAction SilentlyContinue
$env:MASHED_ORIG_BBDUMP_REQ = $req
$p = Start-Process -FilePath "$root\original\MASHED.exe" -WorkingDirectory "$root\original" -PassThru
Write-Output "spawned pid $($p.Id)"
$t0 = Get-Date
$n = 0
$next = $StartS
while (-not $p.HasExited -and ((Get-Date) - $t0).TotalSeconds -lt $RunS) {
    $el = ((Get-Date) - $t0).TotalSeconds
    if ($el -ge $next -and -not (Test-Path $req)) {
        $tgt = "$OutDir\orig_t{0:D3}.bmp" -f [int]($el * 10)
        Set-Content -Path $req -Value $tgt -NoNewline
        $n++
        $next = $el + $Every
    }
    Start-Sleep -Milliseconds 100
}
if (-not $p.HasExited) { Stop-Process -Id $p.Id -Force; Write-Output "stopped pid $($p.Id)" } else { Write-Output "exited $($p.ExitCode)" }
Write-Output "requests $n"
Get-ChildItem "$OutDir\orig_t*.bmp" | ForEach-Object { $_.Name }
