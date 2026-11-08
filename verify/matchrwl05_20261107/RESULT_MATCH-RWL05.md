# RESULT MATCH-RWL05, 2026-11-07

**Verdict: GREEN (match-only, L-93, partial slice with honest counts).** Slice 0x00554390..0x00565570: 126 library rows
(109 `third-party-library[RenderWare-Physics-3.7]`, 17 `third-party-library[renderware]`; list `D:/MashedWT/MATCH-RWLIB_data/rwl05_rows.tsv`).
Every row has a source in `re/match/rwl05/<va8>.c` (126 files).

**68/126 identical** under `re/tools/match/match.py` (XP DDK cl 13.00.9176 / c2 13.00.9178, line-1 flags), all 68 with
`relocheck.py` OK (`relocheck.tsv`; 67 with unchecked=0, 0x00562460 has the import `_RpWorldForAllWorldSectors` unchecked).
All 68 pass the VS2022 C++ check. Scores, sha256, class, reason: `results.tsv`.
No trackers, no build, no gates. Promotion is for a later `c4_reclass.py` batch.

## Flags evidence (g2s raw converter on all 126 rows)

| flag set | compiled | 100% |
|---|---|---|
| `/O2 /Ob1 /G5` (RW build) | 123 | 36 |
| `/O2 /GS /Ob1` (game) | 123 | 18 |
| `/O1` | 123 | 6 |

18 rows are 100% only under `/G5`. The game flags beat /G5 on 10 low raw rows (best 84.2 vs 80.0 at 0x00562600, which later
matched 100% under /G5; the others 7.5-75%), `/O1` beat both on 7 low rows. Physics 3.7 therefore uses the same flags as the RW core
(`/O2 /Ob1 /G5`). Files whose line 1 differs: 0x0055b080, 0x00562010, 0x005645b0 (`/O2 /GS /Ob1`), 0x0055b750 (`/O1 /Ob1 /G5`),
0x0055bde0 (`/O1`): all below 100%, the raw-run best, no evidence either way for them. `tools/g2s_best.py` merges the runs.

## Counts

| | |
|---|---|
| rows | 126 |
| raw converter 100% (relocheck OK) | 36 (19 needed C++-check edits only: `code(...)` typedef, casts, callee arity) |
| hand-matched to 100% (3 helpers, `notes/`) | 32 |
| **identical** | **68** |
| below 100% | 58: class (a) 12, (b) 45, (c) 1 |
| no Ghidra size | 0 |

58 rows were not matched; at least 14 of them (0x00554390, 0x005592f0, 0x0055b650, 0x005645b0, 0x0055f670, 0x00558240, 0x00558df0, 0x00564310,
0x0055b080, 0x0055f800, 0x0055b940, 0x00563b00, 0x0055f520, 0x00562010) are still raw converter output with only
compile/C++ fixes, class (b) "ran out of budget". Per-row class and residual: `results.tsv`, `notes/h*.md`.
Class (c): 0x0055c3e0 needs the C++ bool conversion `neg/sbb/neg`.

## C++ check
113/126 pass VS2022 /TP (`cxx_check.txt`). The 13 failures are all below 100% (0x00558240, 0x005586f0, 0x00558860, 0x00558df0, 0x005595d0,
0x00559c40, 0x0055b080, 0x0055b4a0, 0x0055bde0, 0x0055f520, 0x0055f670, 0x0055f800, 0x00562010): pointer/int argument
types and `code(...)` arity in raw bodies, to be fixed when those rows are matched.

## Notes and follow-ups
- 0x00558860: the raw `-NAN` operand at pfVar14[2] has no address, so the source holds `0.0f` as a placeholder (header says so); score 24.8.
- 0x005578a0: the call Ghidra labelled `RpAtomicRegisterPlugin` is FUN_004e7d40 with 5 args (helper h1). FUN_00562ac0 and
  0x00554940 are function addresses, not byte globals.
- Static callees without rows: no file holds a second MATCH function.
- The permuter changed semantics in 0x00563df0 (p2/p3 swap) and 0x00557ec0 (store reorder): those variants were rejected.
- Follow-up slice: the 14 raw rows are the cheap next targets; x87-order rows 0x0055bae0/0x0055c000/0x0055bbf0/0x00559040 are class (a)
  of the S19/S24 kind.
- Tool changes: none under `re/tools/match/`. `tools/to_s24.py` now skips a failing row instead of aborting.

## Raw data (not committed), `D:/MashedWT/MATCH-RWL05_data`
decomp_raw.json sha256 358b7ca8c218225b05514521ff4ec0c0b967d8d5b24808e23124a1b745396a09; g2s_g5/game/o1 results.tsv sha256 7abc3afd504245b6acc27a7a92e9e1a833c2c991b537a433f6f3507f2d201da8 / 3c805220a695dbd4b6c1544aede8f94078807184690f71214a19e42c98c0d56b / 0afc62823cce69702cbe885ebc01eb7bdb3647aaaf1197115ad93d72a119bca4.
decomp_pc.py ran from the main checkout (pool slot Mashed_pool0): a worktree has no `mashed_pool`.

## Reproduce
`py -3.12 verify/matchrwl05_20261107/tools/collect.py` then `tools/mkresults.py`; `relocheck.py re/match/rwl05/<va8>.c` per identical row.
Helper brief: `BRIEF.md`, assignment `helper_assign.txt`.
