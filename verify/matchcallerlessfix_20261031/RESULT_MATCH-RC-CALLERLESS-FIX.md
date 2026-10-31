# RESULT MATCH-RC-CALLERLESS-FIX (GREEN, text only)

Ruling L-68 (Mariano 2026-10-31): a match row is C4 only when mashed_re.exe calls the swapped-in body.

Changes:
- re/CONFIDENCE.md: "Callerless bodies" clause rewritten (exe-callers=0 => not C4, matched + swapped, pending a caller,
  required note, demote any merged match C4 with exe-callers=0). Write-set paragraph no longer says a callerless body is C4.
- .claude/skills/re-classify/SKILL.md: callerless bullet rewritten to match.
- re/analysis/CHANGELOG.md: correction entry.

Demotions: none. `hooks.csv` on orch/integration @ 0d421534c has 0 rows with `c4=match`, `c4=libid`, `route match` or `exe-callers`.

Gates: not run (text only).
