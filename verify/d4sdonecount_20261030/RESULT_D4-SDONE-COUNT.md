# RESULT D4-SDONE-COUNT (2026-10-30)

Verdict: GREEN. Text-only step. Edited `ROADMAP.md` (D4 dual-copy item and D4 gate), nothing else.

## rva-lint output (verbatim, `py -3.12 scripts/lint_rva_bodies.py`, worktree off orch/integration 1611794e7)

```
[rva-lint] 16 finding(s): CROSS-TARGET=16  |  allowlisted=16 NEW=0
[rva-lint] OK -- no NEW duplicate bodies (16 known, tracked in re\tools\dual_copy_allowlist.txt)
```

Saved as `verify/d4sdonecount_20261030/lint.txt`. `dual_copy_allowlist.txt` has 16 live lines: `:38` (`004177b0`) and `:63-77` (15 RFX CROSS-TARGET).

## Edits
- Dual-copy item: "currently holds 110 entries" now says 110 was the count when added and 16 live lines now, all CROSS-TARGET, quoting the lint line.
- D4 gate: added the line `allowlisted=16 NEW=0` and named `D4-DC-77b0` (`0x004177b0`) and `D4-DC-RFX-CORE` (`:63-77`) as the only blockers.
- Line numbers in the task (2900, 2963-2964) were stale in this base: the text sits at ROADMAP.md:2927 and :2991.
- The older `allowlisted=110/111/122` mentions (lines 1151, 1608, 1653, 1671, 1747) are dated history and were left alone.
