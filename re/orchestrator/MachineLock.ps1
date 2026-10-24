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

#
# Exclusive (solo) mode, same files and token as mashed_lock.py (2026-10-24):
#   $tok = Enter-SoloLock -Label 'd2arms' [-TtlSec 14400] [-BindPid $PID]
#   try { ... boots take mashed_machine as usual ... } finally { Exit-SoloLock $tok }
# While a solo window is open, Acquire-Lock waits unless $env:MASHED_LOCK_SOLO_TOKEN
# equals the window's token (Enter-SoloLock sets it for this process and children).

$script:MaxAgeSec = 30 * 60
$script:SoloEnv = 'MASHED_LOCK_SOLO_TOKEN'

function Get-LockPath([string]$Name) { Join-Path $env:TEMP "$Name.lock" }
function Get-SoloLockPath([string]$Name) { Join-Path $env:TEMP "${Name}_solo.lock" }

function Get-SoloOwner([string]$Name = 'mashed_machine') {
  $p = Get-SoloLockPath $Name
  if (-not (Test-Path $p)) { return $null }
  $o = $null; try { $o = Get-Content $p -Raw | ConvertFrom-Json } catch {}
  $now = [int][double]::Parse((Get-Date -UFormat %s))
  $dead = $o -and $o.pid -and -not (Test-PidAlive ([int]$o.pid))
  if (-not $o -or $dead -or ([int]$o.expires -lt $now)) {
    Remove-Item $p -Force -ErrorAction SilentlyContinue; return $null
  }
  return $o
}

function Enter-SoloLock {
  param([string]$Name = 'mashed_machine', [string]$Label = '', [int]$TtlSec = 14400,
        [int]$BindPid = 0, [int]$TimeoutSec = 1800, [int]$PollSec = 3)
  $p = Get-SoloLockPath $Name
  $tok = [guid]::NewGuid().ToString('N')
  $start = Get-Date
  while ($true) {
    try {
      $now = [int][double]::Parse((Get-Date -UFormat %s))
      $fs = [System.IO.File]::Open($p, [System.IO.FileMode]::CreateNew, [System.IO.FileAccess]::Write)
      $json = @{ token = $tok; pid = $(if ($BindPid) { $BindPid } else { $null }); at = (Get-Date).ToString('o');
                 epoch = $now; expires = $now + $TtlSec; label = $Label; host = $env:COMPUTERNAME } | ConvertTo-Json -Compress
      $b = [Text.Encoding]::UTF8.GetBytes($json); $fs.Write($b, 0, $b.Length); $fs.Close()
      break
    } catch [System.IO.IOException] {
      if ($null -eq (Get-SoloOwner $Name)) { continue }
      if (((Get-Date) - $start).TotalSeconds -gt $TimeoutSec) { throw "waited ${TimeoutSec}s for solo '$Name'" }
      Start-Sleep -Seconds $PollSec
    }
  }
  # drain the current game holder
  $g = Get-LockPath $Name
  while (Test-Path $g) {
    $o = $null; try { $o = Get-Content $g -Raw | ConvertFrom-Json } catch {}
    $age = if ($o.epoch) { (Get-Date -UFormat %s) - $o.epoch } else { 9999999 }
    if (-not $o -or -not (Test-PidAlive ([int]$o.pid)) -or $age -gt $script:MaxAgeSec) {
      Remove-Item $g -Force -ErrorAction SilentlyContinue; break
    }
    Start-Sleep -Seconds $PollSec
  }
  Set-Item -Path "env:$($script:SoloEnv)" -Value $tok
  return [pscustomobject]@{ Name = $Name; Token = $tok }
}

function Exit-SoloLock($handle) {
  if (-not $handle) { return }
  $p = Get-SoloLockPath $handle.Name
  $o = $null; try { $o = Get-Content $p -Raw | ConvertFrom-Json } catch {}
  if ($o -and $o.token -eq $handle.Token) { Remove-Item $p -Force -ErrorAction SilentlyContinue }
  Remove-Item -Path "env:$($script:SoloEnv)" -ErrorAction SilentlyContinue
}

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
  $notedSolo = $false
  while ($true) {
    $so = Get-SoloOwner $Name
    if ($so -and ((Get-Item -Path "env:$($script:SoloEnv)" -ErrorAction SilentlyContinue).Value -ne $so.token)) {
      if (-not $notedSolo -and -not $Quiet) {
        Write-Host "[machine-lock] WAITING — solo window '$Name' open ($($so.label))" -ForegroundColor Yellow
        $notedSolo = $true
      }
      Start-Sleep -Seconds $PollSec; $start = Get-Date; continue   # bounded by the solo expiry
    }
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
