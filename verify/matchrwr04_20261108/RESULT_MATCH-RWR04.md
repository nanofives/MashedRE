# RESULT MATCH-RWR04 (MATCH-RWLIB-2 slice R04, 0x00550670..0x005655a0) — HANDOFF (partial)

Verdict: HANDOFF. Stopped early on a reboot-coordinator request (LIVING-PC Windows token leak, 2026-11-08).
Match-only: no build, no game, no gates, no tracker edits, no promotion.

## Counts

- Rows now at 100% byte match with relocheck OK: **6 / 79** (before 0 / 79).
- Compiler: XP DDK cl 13.00.9176 / c2 13.00.9178 via `re/tools/match/match.py`, flags `/O2 /Ob1 /G5` (each file's
  existing `// FLAGS:` line, unchanged). No `__asm`, no naked function in any of the six.
- `relocheck.tsv`: all six OK.
- Not done yet: full `results.tsv` re-score of the 79 rows (collect.py), C++ check (cxxcheck.bat), per-file
  `MATCH-RWR04` header lines. The other 73 rows are untouched in source, so their round-1 scores in R04.tsv stand.

## The 6 rows at 100% (C4 candidates by route match)

| rva | size | before | after | what moved it |
|---|---|---|---|---|
| 0x0055fe40 | 12 | 25.0 | 100 | returns param 1 (Ghidra typed void) |
| 0x00565570 | 28 | 16.7 | 100 | returns param 1 (Ghidra typed void) |
| 0x0055a140 | 172 | 12.3 | 100 | returns param 1 (Ghidra typed void) |
| 0x00552720 | 43 | 6.7 | 100 | returns param 1 (Ghidra typed void) |
| 0x0055ade0 | 102 | 90.2 | 100 | `return param_2;` duplicated inside the if block: the shared epilogue then needs edi pushed at entry (no shrink-wrap) |
| 0x00551550 | 77 | 89.2 | 100 | returns int: `return cb(...)` on the callback path, `return param_1` at the end (Ghidra typed void); no shrink-wrap follows |

## Findings for the remaining rows and the other slices

1. **Return-value sweep is the cheapest lever.** `tools/retall.py` runs `tools/retv.py` (try `return <param k>` for a
   void-typed function) over every void row; it took 4 rows from <=25% to 100% in one pass.
2. **"Shrink-wrap" residue (original pushes callee-saved regs at entry) is a return-shape problem**, not a compiler
   setting. Two fixes worked: an explicit duplicated `return x;` at the end of the if block (0x0055ade0), and a
   non-void return where the original leaves a value in eax at every ret (0x00551550). Rows with the same note still
   open: 0x00551190 (90.7), 0x00550a20 (82.4), 0x00551c40 (80.5). For 0x00551c40 the original returns the raw call
   result (eax = 0) on all three fail paths with no `xor eax,eax` and no tail merge; 5 nest/flat shapes tried, none.
3. 0x0055c230 (93.4): the first dot product's term order is canonicalised by the compiler; 9 flat spellings all give
   93.4 and 18 parenthesisations give 83.6..93.4. Not solved.
4. 0x00551a50 (96.4): `lea edx,[eax+8]` residue unchanged under index, cast and outer-index forms.
5. 0x0055c380 (85.2): register choice for the 1.0f constant (edx in the original). Float-typed stores and store
   orders tried, no gain. A permuter run was started (log D:/MashedWT/MATCH-RWR04_data/perm_0055c380.log, not
   applied, no `--write`).

## Resume plan

Continue from the near-miss list in R04.tsv: 005584c0 (inline ret on the first fail path), 0055c2d0 (ret4 variant
scored 87.9 regs / 97.0 ops, combine with the x87 order fix), 0055ac50, 0055ad30, 005551d0, 00559c00, 00552890,
005507b0, 00564100, 0055dca0, then the shrink-wrap rows above with finding 2. Finish with collect.py (results.tsv with
before%), cxxcheck, header lines.

## Files

- Sources changed (6): `re/match/rwl04/{00551550,00552720}.c`, `re/match/rwl05/{0055a140,0055ade0,0055fe40,00565570}.c`.
- `tools/`: `retv.py`, `retall.py`, `show.py`, `vtry.py` (paths point at this worktree / D:/MashedWT/MATCH-RWR04_data).
- Raw, not committed (D:/MashedWT/MATCH-RWR04_data):
  - `retv_all.log` sha256 881c7b97ac20ed7e6d04c07680006816dd8c42620c51fda7e90737692975a115
  - `retscan.log` sha256 3a0da477841201bfff53c38181644e34e868c2ff18b2595d99c641003d3c08b1
  - `scratch/` variant sources, `s*.py` variant specs.
