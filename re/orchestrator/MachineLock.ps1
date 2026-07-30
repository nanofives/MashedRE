# MachineLock.ps1 — dot-source for cross-process locks that interoperate with
# re/orchestrator/mashed_lock.py (SAME file path in $env:TEMP, SAME JSON schema:
# pid/at/epoch/label/host). A PowerShell spawner and a Python spawner queue
# against each other on the same named lock.
#
#   . "$PSScriptRoot\MachineLock.ps1"
#   $h = Acquire-Lock -Name mashed_build -Label 'exec-build'   # blocks if held
#   try { ... } finally { Release-Lock $h }
#
# Two standard names:
#   mashed_machine — the GAME lock (any process that boots MASHED).
#   mashed_build   — the build-tree lock (only build.bat writers).

$script:MaxAgeSec = 30 * 60

function Get-LockPath([string]$Name) { Join-Path $env:TEMP "$Name.lock" }

function Test-PidAlive([int]$ProcId) {
  if ($ProcId -le 0) { return $false }
  return $null -ne (Get-Process -Id $ProcId -ErrorAction SilentlyContinue)
}

function Acquire-Lock {
  param(
    [string]$Name = 'mashed_machine',
    [string]$Label = '',
    [int]$TimeoutSec = 1800,
    [int]$PollSec = 3,
    [switch]$Force,
    [switch]$Quiet
  )
  $path = Get-LockPath $Name
  $start = Get-Date
  $notedWait = $false
  while ($true) {
    try {
      $fs = [System.IO.File]::Open($path, [System.IO.FileMode]::CreateNew, [System.IO.FileAccess]::Write)
      $json = @{ pid = $PID; at = (Get-Date).ToString('o'); epoch = [int][double]::Parse((Get-Date -UFormat %s)); label = $Label; host = $env:COMPUTERNAME } | ConvertTo-Json -Compress
      $bytes = [Text.Encoding]::UTF8.GetBytes($json)
      $fs.Write($bytes, 0, $bytes.Length); $fs.Close()
      if (-not $Quiet) { Write-Host "[machine-lock] HELD '$Name' pid=$PID ($Label)" -ForegroundColor DarkGray }
      return [pscustomobject]@{ Name = $Name; Path = $path; Pid = $PID }
    } catch [System.IO.IOException] {
      $owner = $null; try { $owner = Get-Content $path -Raw | ConvertFrom-Json } catch {}
      $opid = if ($owner) { [int]$owner.pid } else { 0 }
      $age = if ($owner.epoch) { (Get-Date -UFormat %s) - $owner.epoch } else { 9999999 }
      $stale = $Force -or (-not (Test-PidAlive $opid)) -or ($age -gt $script:MaxAgeSec)
      if ($stale) {
        if (-not $Quiet) { Write-Host "[machine-lock] breaking STALE '$Name' (pid=$opid)" -ForegroundColor Yellow }
        Remove-Item $path -Force -ErrorAction SilentlyContinue; continue
      }
      if (((Get-Date) - $start).TotalSeconds -gt $TimeoutSec) {
        throw "waited ${TimeoutSec}s for lock '$Name' held by pid=$opid ($($owner.label))"
      }
      if (-not $notedWait -and -not $Quiet) {
        Write-Host "[machine-lock] WAITING — '$Name' held by pid=$opid ($($owner.label)); queueing..." -ForegroundColor Yellow
        $notedWait = $true
      }
      Start-Sleep -Seconds $PollSec
    }
  }
}

function Release-Lock($handle) {
  if (-not $handle) { return }
  if (Test-Path $handle.Path) {
    $o = $null; try { $o = Get-Content $handle.Path -Raw | ConvertFrom-Json } catch {}
    if (-not $o -or [int]$o.pid -eq $handle.Pid) {
      Remove-Item $handle.Path -Force -ErrorAction SilentlyContinue
    }
  }
}
