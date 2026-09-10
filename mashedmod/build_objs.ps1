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
# -X87List (added 2026-09-10): a file of TU basenames, one per line, that must be
# compiled with /arch:IA32 appended -- i.e. x87 codegen instead of MSVC's default
# /arch:SSE2. The physics/math ports are verbatim transcriptions of x87 instruction
# streams, and under SSE2 a `float` expression rounds to 32 bits after EVERY operation
# while the original holds intermediates at 80 bits and rounds once at the FSTP.
# MEASURED on RwpSolverBroadphase3.cpp: 0x0055b750 went DIVERGENT 13/48 -> CLEAN 48/48
# and 0x0055c2d0 DIVERGENT 5/24 -> CLEAN 24/24, with the TU's two already-CLEAN rows
# (0x0055a1f0, 0x0055bae0) still CLEAN 48/48 -- 2 fixed, 0 regressions.
# Per-TU rather than global so librw (the shipping renderer) and every non-physics TU
# keep SSE2. See re/analysis/float_model_is_sse2_not_x87_20260910.md and D-11070.
# The list's CONTENT is part of the flag stamp, so editing it forces a full rebuild.
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
    [string]$X87List = '',                            # optional: TUs to compile /arch:IA32
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
# TUs that need x87 codegen. Basenames (no extension), case-insensitive.
$x87 = @{}
$x87Text = ''
if ($X87List -ne '' -and (Test-Path $X87List)) {
    foreach ($line in Get-Content $X87List) {
        $t = $line.Trim()
        if ($t -eq '' -or $t.StartsWith('#')) { continue }
        $x87[[System.IO.Path]::GetFileNameWithoutExtension($t).ToLowerInvariant()] = $true
    }
    $x87Text = (($x87.Keys | Sort-Object) -join ',')
}
# The list content is IN the stamp: moving a TU in or out of the x87 set changes its
# codegen, so it must invalidate the cache exactly like a flag change does.
$stampText = "cl=$clPath`nflags=$ClFlags`nsrcroot=$SrcRoot`nx87=$x87Text"
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
    $flagList = $ClFlags -split ' +' | Where-Object { $_ -ne '' }
    # Two flag sets: the default one, and /arch:IA32 for the x87 TUs. Compiled as two cl
    # invocations because /MP shares one flag set across its whole response file.
    $groups = @(
        @{ Name = 'sse2'; Extra = @();             Rsp = 'compile.rsp';
           Items = @($stale | Where-Object { -not $x87[[System.IO.Path]::GetFileNameWithoutExtension($_.Src).ToLowerInvariant()] }) },
        @{ Name = 'x87';  Extra = @('/arch:IA32'); Rsp = 'compile_x87.rsp';
           Items = @($stale | Where-Object {       $x87[[System.IO.Path]::GetFileNameWithoutExtension($_.Src).ToLowerInvariant()] }) }
    )
    $rc = 0
    foreach ($g in $groups) {
        if ($g.Items.Count -eq 0) { continue }
        if ($g.Name -eq 'x87') {
            Write-Host ("    -> {0} TU(s) with /arch:IA32 (x87)" -f $g.Items.Count)
        }
        $compileRsp = Join-Path $ObjDir $g.Rsp
        Set-Content -Path $compileRsp -Value ($g.Items | ForEach-Object { '"' + $_.Src + '"' })
        Push-Location $SrcRoot
        try {
            # NB: "@$path" not @"$path" -- the latter opens a PowerShell here-string.
            & cl /nologo @flagList @($g.Extra) /c /MP "/Fo$ObjDir\" "/sourceDependencies$ObjDir\" "@$compileRsp"
            $rc = $LASTEXITCODE
        } finally { Pop-Location }
        if ($rc -ne 0) { Write-Host "[$Target] compile FAILED (cl exit $rc, group $($g.Name))"; exit $rc }
    }
} else {
    Write-Host ("[{0}] all {1} objects up to date" -f $Target, $sources.Count)
}

# --- link response file: every object, source-list order -------------------
Set-Content -Path (Join-Path $ObjDir 'link.rsp') -Value ($objs | ForEach-Object { '"' + $_ + '"' })
exit 0
