# HB-SOLOSTALE: Test-TrainDead (merge_train.ps1) on fake pid files. Touches only a temp dir.
$src = Get-Content (Join-Path $PSScriptRoot '..\..\re\orchestrator\merge_train.ps1') -Raw
$null = $src -match '(?s)(function Test-TrainDead.*?\n\})'
Invoke-Expression $Matches[1]
$root = Join-Path ([IO.Path]::GetTempPath()) "traindead_$([guid]::NewGuid().ToString('N').Substring(0,6))"
$DATA = Join-Path $root 'cur'; New-Item -ItemType Directory -Force $DATA | Out-Null
function Put($ts, $o) { $d = Join-Path $root $ts; New-Item -ItemType Directory -Force $d | Out-Null; if ($o) { [IO.File]::WriteAllText((Join-Path $d 'merge_train.pid'), ($o | ConvertTo-Json -Compress)) } }
$sl = Start-Process pwsh -ArgumentList '-NoProfile','-Command','Start-Sleep 60' -PassThru -WindowStyle Hidden
$gone = Start-Process pwsh -ArgumentList '-NoProfile','-Command','exit' -PassThru -WindowStyle Hidden; $gone.WaitForExit()
Start-Sleep -Milliseconds 500
Put 'live' @{ pid = $sl.Id; start = [DateTimeOffset]::new($sl.StartTime).ToUnixTimeSeconds() }
Put 'dead' @{ pid = $gone.Id; start = [DateTimeOffset]::UtcNow.ToUnixTimeSeconds() }
Put 'reuse' @{ pid = $sl.Id; start = [DateTimeOffset]::new($sl.StartTime.AddHours(-2)).ToUnixTimeSeconds() }
Put 'nopid' $null
$r = @{}; foreach ($k in 'live','dead','reuse','nopid') { $r[$k] = Test-TrainDead $k }
$sl.Kill()
$ok = (-not $r.live) -and $r.dead -and $r.reuse -and $r.nopid
"live-train dead=$($r.live)  killed-train dead=$($r.dead)  reused-pid dead=$($r.reuse)  no-pid-file dead=$($r.nopid)"
Remove-Item -Recurse -Force $root
"TRAINDEAD $(if ($ok) {'PASS'} else {'FAIL'})"
