# RESULT C4-REPLAY-RULE (L-128, 2026-11-11)

Text step. Replay form of the frida C4 route written into: re/CONFIDENCE.md (C4 routes, five legs, tag `c4=frida-replay`),
.claude/skills/re-classify/SKILL.md (evidence type `frida-replay`, RESULT line format), re/tools/c4_reclass.py (batch route
`frida-replay`, per-route counts in the summary), ROADMAP.md (L-126 admitted/provisional not C4 by status alone, decisions
table row L-128), re/orchestrator/WORKER_RULES.md, CLAUDE.md. hooks.csv untouched (no row promoted; the tag applies at the first
PROV-INSITU promotion).

Checks run: `py_compile` OK; c4_reclass.py batch dry run on 0x004c9eb0 (C3 row) with a scratch RESULT: PASS with all lines,
REFUSED with `write-set: not-covered`; unknown RVA row refused; already-C4 row skipped; nothing written (dry run).
No build, no gates (text and tool-only row).
