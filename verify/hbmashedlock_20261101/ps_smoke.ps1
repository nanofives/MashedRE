param([string]$LockPs)
$dir = Join-Path ([IO.Path]::GetTempPath()) ("mlps_" + [guid]::NewGuid().ToString('N').Substring(0, 8))
New-Item -ItemType Directory -Path $dir | Out-Null
$env:TEMP = $dir
. $LockPs
$n = 0; $fail = 0
function Chk($name, $ok, $detail) { $script:n++; if (-not $ok) { $script:fail++ }; Write-Host "[$script:n/5] $name $(if ($ok) {'PASS'} else {'FAIL'}) $detail" }

# 1. priority handle writes a held record, release turns it into a linger
$h = Acquire-Lock -Name 'mashed_machine' -Label 'ps-prio' -Priority -Quiet
$held = @(Get-PrioLinger 'mashed_machine')
Release-Lock $h
$after = @(Get-PrioLinger 'mashed_machine')
Chk 'ps-priority-hold-then-linger' (($held.Count -eq 1) -and $held[0].held -and ($after.Count -eq 1) -and -not $after[0].held) "(held=$($held.Count) linger=$($after.Count))"

# 2. a new Enter-SoloLock waits through a 3 s linger of another PID
Get-ChildItem $dir -Filter '*_priolinger_*' | Remove-Item -Force
$now = [int][double]::Parse((Get-Date -UFormat %s))
@{ pid = 4; at = 'x'; epoch = $now; expires = $now + 3; held = $false; label = 'other' } | ConvertTo-Json -Compress |
  Set-Content -LiteralPath (Join-Path $dir 'mashed_machine_priolinger_4.lock') -Encoding utf8
$t0 = Get-Date
$tok = Enter-SoloLock -Label 'ps-solo' -PollSec 1
$dt = ((Get-Date) - $t0).TotalSeconds
Chk 'ps-solo-waits-through-linger' ($dt -ge 1.5) "($([math]::Round($dt, 1)) s)"

# 3. default TTL 3600
$o = Get-Content (Get-SoloLockPath 'mashed_machine') -Raw | ConvertFrom-Json
Chk 'ps-solo-default-ttl-le-1h' (([int]$o.expires - [int]$o.epoch) -le 3600) "(ttl=$([int]$o.expires - [int]$o.epoch))"
Exit-SoloLock $tok

# 4. a held record of a dead PID is stale
@{ pid = 99999999; at = 'x'; epoch = $now; expires = $now + 86400; held = $true; label = 'dead' } | ConvertTo-Json -Compress |
  Set-Content -LiteralPath (Join-Path $dir 'mashed_machine_priolinger_99999999.lock') -Encoding utf8
Chk 'ps-dead-hold-is-stale' (@(Get-PrioLinger 'mashed_machine').Count -eq 0) ''

# 5. -Priority solo open ignores a pending marker of a live PID ($PID's parent stands in)
$pp = (Get-CimInstance Win32_Process -Filter "ProcessId=$PID").ParentProcessId
@{ pid = $pp; at = 'x'; epoch = $now; expires = $now + 600; label = 'train' } | ConvertTo-Json -Compress |
  Set-Content -LiteralPath (Join-Path $dir "mashed_machine_prio_$pp.lock") -Encoding utf8
$t0 = Get-Date
$tok = Enter-SoloLock -Label 'ps-train' -Priority -PollSec 1
$dt = ((Get-Date) - $t0).TotalSeconds
Exit-SoloLock $tok
Chk 'ps-priority-solo-ignores-marker' ($dt -lt 1.5) "($([math]::Round($dt, 1)) s)"
Remove-Item -Recurse -Force $dir
if ($fail) { Write-Host 'FAIL'; exit 1 } else { Write-Host 'PASS'; exit 0 }
