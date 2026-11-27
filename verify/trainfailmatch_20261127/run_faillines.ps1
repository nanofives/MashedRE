param([string]$Src, [string]$Out)
$t = Get-Content $Src -Raw
$m = [regex]::Match($t, '(?s)function Get-BinderFailLines\(\$out\) \{.*?\r?\n\}')
. ([scriptblock]::Create($m.Value))
$o = Get-Content $Out
$r = @(Get-BinderFailLines $o)
"lines: $(@($o).Count), fail lines returned: $($r.Count)"
$r
$old = Get-Content $Out | Where-Object { $_ -match '^\[selftest\]' -and $_ -match '\bFAILS?\b' -and $_ -cnotmatch '\bFAILS?\b' }
"case-insensitive-only matches in this output: $(@($old).Count)"
$old
