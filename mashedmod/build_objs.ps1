# build_objs.ps1 -- per-TU object cache for build.bat (added 2026-09-09).
#
# build.bat used to hand every .cpp of a target to ONE `cl` line, which recompiled
# all ~211 (exe) / ~384 (asi) TUs on every run (~2 min for a one-line edit). This
# script compiles only the STALE subset into a per-target object dir and writes a
# link response file listing every object in source-list order, so the link step
# in build.bat is unchanged in effect (same objects, same order, same flags).
#
# Staleness (any one => recompile):
#   - the .obj is missing, or the .cpp is newer than it;
#   - the cl /sourceDependencies JSON is missing, or any include it lists that
#     lives under -RepoRoot is newer than the .obj (system headers are ignored);
#   - the flag stamp (cl path + flags) differs from the last run => whole dir.
#
# Escape hatches: `build.bat clean` (deletes build\obj) or MASHED_BUILD_FULL=1.
# Windows PowerShell 5.1 compatible (no ternary / ?? / pwsh-only syntax).

param(
    [Parameter(Mandatory = $true)][string]$Target,    # exe | asi (label only)
    [Parameter(Mandatory = $true)][string]$SrcRoot,   # ...\src\mashed_re (cwd for cl)
    [Parameter(Mandatory = $true)][string]$ObjDir,    # per-target object dir
    [Parameter(Mandatory = $true)][string]$Rsp,       # source list: one quoted path per line
    [Parameter(Mandatory = $true)][string]$ClFlags,   # e.g. "/EHa /W3 /O2 /DMASHED_STANDALONE"
    [Parameter(Mandatory = $true)][string]$RepoRoot,  # includes under here count as deps
    [switch]$Force
)

$ErrorActionPreference = 'Stop'

function Norm([string]$p) { return [System.IO.Path]::GetFullPath($p).TrimEnd('\').ToLowerInvariant() }

$SrcRoot  = Norm $SrcRoot
$RepoRoot = Norm $RepoRoot
$ObjDir   = [System.IO.Path]::GetFullPath($ObjDir).TrimEnd('\')
if (-not (Test-Path $ObjDir)) { New-Item -ItemType Directory -Force $ObjDir | Out-Null }

# --- source list -----------------------------------------------------------
$sources = @()
foreach ($line in Get-Content $Rsp) {
    $t = $line.Trim()
    if ($t -eq '' -or $t.StartsWith('#') -or $t.StartsWith('REM ')) { continue }
    $t = $t.Trim('"')
    if ($t -notmatch '\.cpp$') { throw "build_objs: non-.cpp entry in ${Rsp}: $t" }
    $sources += $t
}
if ($sources.Count -eq 0) { throw "build_objs: empty source list $Rsp" }

# Flat object dir => basenames must be unique (they are today; guard it).
$seen = @{}
foreach ($s in $sources) {
    $b = [System.IO.Path]::GetFileNameWithoutExtension($s).ToLowerInvariant()
    if ($seen.ContainsKey($b)) { throw "build_objs: duplicate basename '$b' ($s vs $($seen[$b])) -- flat obj dir cannot hold both" }
    $seen[$b] = $s
}

# --- flag stamp ------------------------------------------------------------
$clPath = (Get-Command cl -ErrorAction SilentlyContinue).Source
if (-not $clPath) { throw "build_objs: cl.exe not on PATH (vcvars32 not applied?)" }
$stampText = "cl=$clPath`nflags=$ClFlags`nsrcroot=$SrcRoot"
$stampFile = Join-Path $ObjDir 'flags.txt'
$wipe = $Force -or ($env:MASHED_BUILD_FULL -eq '1')
if (-not $wipe) {
    if (-not (Test-Path $stampFile)) { $wipe = $true }
    elseif ((Get-Content $stampFile -Raw) -ne $stampText) { $wipe = $true; Write-Host "[$Target] flags changed -> full rebuild" }
}
if ($wipe) {
    Get-ChildItem $ObjDir -File | Where-Object { $_.Extension -in '.obj', '.json' } | Remove-Item -Force
}
Set-Content -Path $stampFile -Value $stampText -NoNewline

# --- staleness -------------------------------------------------------------
$mtimeCache = @{}
function MTime([string]$path) {
    $k = $path.ToLowerInvariant()
    if (-not $mtimeCache.ContainsKey($k)) {
        if (Test-Path -LiteralPath $path -PathType Leaf) { $mtimeCache[$k] = (Get-Item -LiteralPath $path).LastWriteTimeUtc }
        else { $mtimeCache[$k] = $null }
    }
    return $mtimeCache[$k]
}

$stale = @()
$objs  = @()
foreach ($s in $sources) {
    $base = [System.IO.Path]::GetFileNameWithoutExtension($s)
    $obj  = Join-Path $ObjDir ($base + '.obj')
    $dep  = Join-Path $ObjDir ($base + '.cpp.json')   # cl names it <basename>.cpp.json
    $objs += $obj
    $srcAbs = Join-Path $SrcRoot $s
    $objT = MTime $obj
    $reason = $null
    if ($null -eq $objT) { $reason = 'no obj' }
    elseif (-not (Test-Path -LiteralPath $dep)) { $reason = 'no deps' }
    else {
        $srcT = MTime $srcAbs
        if ($null -eq $srcT) { throw "build_objs: source missing: $srcAbs" }
        if ($srcT -gt $objT) { $reason = 'source newer' }
        else {
            try { $j = Get-Content -LiteralPath $dep -Raw | ConvertFrom-Json } catch { $reason = 'bad deps json' }
            if (-not $reason) {
                foreach ($inc in $j.Data.Includes) {
                    $il = $inc.ToLowerInvariant()
                    if (-not $il.StartsWith($RepoRoot)) { continue }   # system header
                    $it = MTime $inc
                    if ($null -eq $it) { $reason = "include missing: $inc"; break }
                    if ($it -gt $objT) { $reason = "include newer: $inc"; break }
                }
            }
        }
    }
    if ($reason) { $stale += [pscustomobject]@{ Src = $s; Why = $reason } }
}

# --- compile the stale subset ---------------------------------------------
if ($stale.Count -gt 0) {
    Write-Host ("[{0}] compiling {1} of {2} TUs" -f $Target, $stale.Count, $sources.Count)
    if ($stale.Count -le 12) { foreach ($x in $stale) { Write-Host ("    {0}  ({1})" -f $x.Src, $x.Why) } }
    $compileRsp = Join-Path $ObjDir 'compile.rsp'
    Set-Content -Path $compileRsp -Value ($stale | ForEach-Object { '"' + $_.Src + '"' })
    $flagList = $ClFlags -split ' +' | Where-Object { $_ -ne '' }
    Push-Location $SrcRoot
    try {
        # NB: "@$path" not @"$path" -- the latter opens a PowerShell here-string.
        & cl /nologo @flagList /c /MP "/Fo$ObjDir\" "/sourceDependencies$ObjDir\" "@$compileRsp"
        $rc = $LASTEXITCODE
    } finally { Pop-Location }
    if ($rc -ne 0) { Write-Host "[$Target] compile FAILED (cl exit $rc)"; exit $rc }
} else {
    Write-Host ("[{0}] all {1} objects up to date" -f $Target, $sources.Count)
}

# --- link response file: every object, source-list order -------------------
Set-Content -Path (Join-Path $ObjDir 'link.rsp') -Value ($objs | ForEach-Object { '"' + $_ + '"' })
exit 0
