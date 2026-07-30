# preflight.ps1 — read-only classifier for read-fleet queue units.
#
# account2 auto-allows Read/Grep/Glob but PROMPTS on any write/build/install/
# network/MCP action — and a HEADLESS claude2 child cannot answer a prompt
# (bypass-permissions is disabled by managed policy), so it HANGS. This flags
# such units so the fleet rejects them up front instead of stalling.
#
# Heuristic, tuned for FEW FALSE POSITIVES so it stays trusted:
#   1) strip NEGATED action clauses first, so "Do NOT write any file" — the
#      standard read-only affirmation — is a GOOD signal, never a match;
#   2) flag only HIGH-CONFIDENCE execution/mutation signals (command
#      invocations, git mutations, installs, decompile, explicit writes to a
#      source/tracker file). Mentioning a file NAMED run_diff_*.py is fine;
#      RUNNING `py -3.12 ...` is not.
# Returns $null (read-only) or a one-line reason string (flagged).

function Test-ReadOnlyPrompt([string]$Prompt) {
  if ([string]::IsNullOrWhiteSpace($Prompt)) { return $null }
  $t = $Prompt.ToLower()
  $t = [regex]::Replace($t,
    '(?:do not|don''t|do n''t|never|without|no)\s+(?:\w+\s+){0,3}?(?:writ\w*|edit\w*|creat\w*|modif\w*|sav\w*|delet\w*|append\w*|overwrit\w*|run\b|execut\w*|build\w*|install\w*|commit\w*|push\w*|patch\w*|decompil\w*|add\b|author\w*)',
    ' ')
  $danger = @(
    @{ re = '\bpy(thon3?)?\s+-3\.12\b|\bpwsh\s+-|\bpowershell\s+-'; why = 'runs a script (py/pwsh)' },
    @{ re = '\b(pip3?|poetry|npm|yarn|pnpm|cargo|gem|bundle|go)\s+(install|i|ci|add|get)\b'; why = 'package install' },
    @{ re = '\bgit\s+(commit|push|pull|merge|rebase|clone|add|checkout|reset|tag)\b'; why = 'git mutation' },
    @{ re = '\b(curl|wget)\b'; why = 'network fetch' },
    @{ re = '\b(run|execute|invoke|launch|boot|spawn|apply)\s+[\w .\\/''-]{0,30}?(\.(bat|ps1|sh|py)|\bmashed\b|\bgame\b|\bbatch\b|\bsweep\b|\bdiff\b|\bpipeline\b|\bstalker\b|\bscenario\b)'; why = 'runs a command/game' },
    @{ re = '\b(decompile|disassemble)\b'; why = 'MCP decompile (Ghidra — unavailable on account2)' },
    @{ re = '\b(author|writ\w+|sav\w+|edit\w+|modif\w+|append\w+|overwrit\w+|creat\w+|add|register|wire)\b\s+[\w .\\/''-]{0,40}?(build\.bat|asi_sources|hooks_registry|hooks\.csv|changelog|\.cpp|\.rsp|the registry|a new (file|row|entry|hook))'; why = 'writes a source/tracker file' }
  )
  foreach ($d in $danger) {
    $m = [regex]::Match($t, $d.re)
    if ($m.Success) { return "$($d.why) — matched '$($m.Value.Trim())'" }
  }
  return $null
}
