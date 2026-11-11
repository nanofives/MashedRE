$DCFG = @{ flip_id_rx = '(^|-)FLIP(-|$)'; flip_title_rx = '(?i)\bdecided[ -]flips?\b'; flip_decision_rx = '^L-\d+$'; flip_decision_title_rx = '(?i)\bdefault[ -]ON\b' }
$src = Get-Content -Raw 'C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/HB-TRAIN-FLIP-DECISION/re/orchestrator/merge_train.ps1'
$m = [regex]::Match($src, '(?s)function Test-FlipRow\(\$row\) \{.*?\n\}')
Invoke-Expression $m.Value
$rows = (Get-Content -Raw 'C:/Users/maria/Desktop/Proyectos/Mashed/log/orch/roadmap.json' | ConvertFrom-Json -DateKind String).rows
$old = @($rows | Where-Object { ($_.id -match $DCFG.flip_id_rx) -or ($_.title -match $DCFG.flip_title_rx) })
$new = @($rows | Where-Object { Test-FlipRow $_ })
"rows $($rows.Count) old $($old.Count) new $($new.Count) added: $((@($new | Where-Object { $old.id -notcontains $_.id }) | ForEach-Object { "$($_.id)($($_.stage))" }) -join ', ')"
$q = @($rows | Where-Object { $_.stage -notin 'merged','done' -and (Test-FlipRow $_) }) ; "not merged/done flagged: $(($q | ForEach-Object { "$($_.id)($($_.stage))" }) -join ', ')"
