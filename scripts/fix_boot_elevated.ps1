# Auto-try elevated fixes for the MASHED.exe boot heap-AV (LFH metadata corruption
# in ntdll!RtlpHeap; the documented EMULATEHEAP shim no longer engages after the
# ~2026-06-10 Windows AppCompat update). RUN THIS IN AN ELEVATED (Admin) POWERSHELL.
#
# Each candidate is applied, MASHED is launched and watched for survival past the
# ~4s heap-AV, and the FIRST one that boots is KEPT; failures are reverted. Nothing
# here is destructive - only HKLM IFEO / HKLM Layers values for MASHED.exe, all
# reverted if they don't help.
#
#   Right-click PowerShell -> Run as administrator, then:
#   cd C:\Users\maria\Desktop\Proyectos\Mashed
#   powershell -ExecutionPolicy Bypass -File scripts\fix_boot_elevated.ps1

$ErrorActionPreference = 'Stop'
$mashed = 'C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe'
$dir    = Split-Path $mashed
$ifeo   = 'HKLM:\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\MASHED.exe'
$hklmL  = 'HKLM:\SOFTWARE\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Layers'

# admin check
$admin = ([Security.Principal.WindowsPrincipal][Security.Principal.WindowsIdentity]::GetCurrent()
         ).IsInRole([Security.Principal.WindowsBuiltinRole]::Administrator)
if (-not $admin) { Write-Host "NOT ELEVATED - re-run in an Admin PowerShell." -ForegroundColor Red; exit 1 }

function Test-Boot([int]$secs = 11) {
    $p = Start-Process -FilePath $mashed -WorkingDirectory $dir -PassThru
    Start-Sleep -Seconds $secs
    $p.Refresh()
    if ($p.HasExited) { return @{ ok = $false; code = ('0x{0:X8}' -f $p.ExitCode) } }
    Stop-Process -Id $p.Id -Force
    return @{ ok = $true; code = 'ALIVE' }
}

function Clear-IFEO { if (Test-Path $ifeo) { Remove-Item $ifeo -Recurse -Force } }
function Clear-HklmLayer { if ((Test-Path $hklmL) -and (Get-Item $hklmL).Property -contains $mashed) { Remove-ItemProperty $hklmL -Name $mashed -Force } }

# (name, apply scriptblock, cleanup scriptblock)
$candidates = @(
  @{ name = 'Full Page Heap (replaces LFH allocator)';
     apply = { New-Item $ifeo -Force | Out-Null
               New-ItemProperty $ifeo -Name GlobalFlag    -Value 0x02000000 -PropertyType DWord -Force | Out-Null
               New-ItemProperty $ifeo -Name PageHeapFlags -Value 0x3        -PropertyType DWord -Force | Out-Null }
     cleanup = { Clear-IFEO } },

  @{ name = 'Light Page Heap';
     apply = { New-Item $ifeo -Force | Out-Null
               New-ItemProperty $ifeo -Name GlobalFlag    -Value 0x02000000 -PropertyType DWord -Force | Out-Null
               New-ItemProperty $ifeo -Name PageHeapFlags -Value 0x1        -PropertyType DWord -Force | Out-Null }
     cleanup = { Clear-IFEO } },

  @{ name = 'EMULATEHEAP via system-wide (HKLM) Layers';
     apply = { if (-not (Test-Path $hklmL)) { New-Item $hklmL -Force | Out-Null }
               Set-ItemProperty $hklmL -Name $mashed -Value '~ RUNASINVOKER WIN98RTM HIGHDPIAWARE EMULATEHEAP' -Type String }
     cleanup = { Clear-HklmLayer } },

  @{ name = 'Disable segment-heap front-end (FrontEndHeapDebugOptions=0x08)';
     apply = { New-Item $ifeo -Force | Out-Null
               New-ItemProperty $ifeo -Name FrontEndHeapDebugOptions -Value 0x08 -PropertyType DWord -Force | Out-Null }
     cleanup = { Clear-IFEO } }
)

Write-Host "=== MASHED.exe boot-fix sweep (elevated) ===`n"
$winner = $null
foreach ($c in $candidates) {
    Write-Host ("Trying: {0}" -f $c.name)
    & $c.apply
    $r = Test-Boot 11
    if ($r.ok) {
        Write-Host ("  >>> BOOTS - keeping this fix: {0}" -f $c.name) -ForegroundColor Green
        $winner = $c.name
        break
    } else {
        Write-Host ("  crashed ({0}); reverting" -f $r.code) -ForegroundColor DarkYellow
        & $c.cleanup
    }
}

Write-Host ""
if ($winner) {
    Write-Host "RESULT: boot fixed by -> $winner" -ForegroundColor Green
    Write-Host "(left applied. To undo later: delete the MASHED.exe IFEO key / HKLM Layers entry.)"
} else {
    Write-Host "RESULT: none of the elevated candidates booted MASHED. All reverted." -ForegroundColor Red
    Write-Host "Next: a reboot (re-rolls heap/shim state) or a custom EmulateHeap .sdb."
}
