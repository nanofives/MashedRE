# PowerShell wrapper around frida_pool.sh — same surface, native shell.
# Bash is the authoritative implementation; this just dispatches.

param(
    [Parameter(Position = 0)] [string] $Action = 'status',
    [Parameter(Position = 1, ValueFromRemainingArguments = $true)] [string[]] $Rest
)

$bash = Join-Path $PSScriptRoot 'frida_pool.sh'
if (-not (Test-Path $bash)) { Write-Error "frida_pool.sh not found at $bash"; exit 1 }

$args = @($bash, $Action) + ($Rest | Where-Object { $_ })
& bash @args
exit $LASTEXITCODE
