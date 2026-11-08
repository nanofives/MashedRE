# RESULT MATCH-RWL03, 2026-11-08

**Verdict: GREEN (match-only, L-93, partial slice with honest counts).** Slice 0x004f0910..0x005405c0: 125 `third-party-library[renderware]` rows
(list `D:/MashedWT/MATCH-RWLIB_data/rwl03_rows.tsv`; 21 rows 0x004f0910..0x004f46a0 are the RW D3D9 driver, the rest RW toolkit rows from 0x0052d8e0).
Sources in `re/match/rwl03/<va8>.c`: 121 files (4 rows have no compiling source, see below).

**55/125 identical** under `re/tools/match/match.py` (XP DDK cl 13.00.9176 / c2 13.00.9178, line-1 flags), all 55 with `relocheck.py` OK
(`relocheck.tsv`; 49 with unchecked=0, the other 6 list only imports or the CRT unchecked) and all 55 pass the VS2022 /TP C++ check
(`cxx_check.txt`). None is naked asm (`__asm` appears in no file). Scores, sha256, class, reason: `results.tsv`.
No trackers, no build, no gates. Promotion is for a later `c4_reclass.py` batch.

## Flags evidence (g2s raw converter on all 125 rows, `tools/g2s_best.py`)

| flag set | compiled | 100% |
|---|---|---|
| `/O2 /Ob1 /G5` (RW build) | 117 of 125 (best-of table) | 28 |
| `/O2 /GS /Ob1` (game) | | 12 |
| `/O1` | | 4 |

Best-of per row (ties to /G5): 101 rows best under /G5, 12 under game flags, 12 under /O1 (all of those low raw scores). Every one of the 55 identical rows
is at 100% under `/O2 /Ob1 /G5`. The D3D9 driver rows (0x004f0910..0x004f46a0) use the same flags as the toolkit rows: no row there needed other flags.
Rows whose line 1 differs from /G5 are all below 100%: 3 `/O1`, 2 `/O1 /G5` (0x0053fb00, 0x0053fa20: shrink-wrapped push order, `/O1 /G5` is the best form), 3 `/O2 /GS /Ob1`.
`results.tsv` column `flags` has the line-1 flags of every file.

## Counts

| | |
|---|---|
| rows | 125 |
| raw converter 100% (relocheck OK) | 28 (all needed relocheck or C++ edits only, see below) |
| hand-matched to 100% (3 helpers, `notes/`) | 27 (h1 10, h2 7, h3 10; h1 reported 8 in its summary, its list shows 10) |
| **identical** | **55** |
| below 100% | 70: class (a) 12, (a/b) 5, (b) 50, (b/c) 1, (c) 2 |
| no Ghidra size | 0 |

Hand matching was bounded to rows of 700 B or less (79 rows, 3 helpers, about 25 min wall each). The 18 rows above 700 B were not hand-matched
(budget): 0x004f1fb0, 0x004f2570, 0x004f3e90, 0x00530650, 0x00530c00, 0x00532b80, 0x00533ec0, 0x00534d00, 0x00535330, 0x00538d60, 0x00539900,
0x00539ec0, 0x0053a7a0, 0x0053b7b0, 0x0053bdf0, 0x0053c6e0, 0x0053dea0, 0x005405c0 (4 of them, 0x004f1fb0 0x00538d60 0x0053a7a0 0x0053dea0, have no compiling source).
Class (c): 0x00532000 and 0x005320b0 are hand-written packed SSE (movaps/mulps/shufps), no SSE in the DDK cl.

## C++ check
106/125 pass VS2022 /TP. The 19 failures are all below 100% (18 over 700 B, plus 0x0053e830): pointer/int argument types in raw bodies.

## Tool fix (re/tools/match/relocheck.py, one line)
`RE_G` read a global's address from a later call argument: the optional group `(?:[^,]+,\s*)?` crossed `;` and newlines, so
`MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);` plus a later `(param_1,0x30502)` gave `DAT_007d3ff8 = 0x30502` and a false BAD on 0x004f0970.
Changed to `(?:[^,;\n]+,\s*)?`. All 55 rows re-checked after the change.

## Notes and follow-ups
- RwGlobals rule: `(DAT_007d3ff8 + DAT_00911ae0 + k)` pointer first (0x004f4340, 0x004f43f0 failed relocheck with the raw operand order).
- Permuter variants changed semantics in 0x004f2e70, 0x0053d5a0, 0x00533d00, 0x0052e0e0, 0x0052eb40 (stores moved, aligned-pointer logic, uninitialized locals): rejected.
- Idioms found: byte loop index via `*(byte*)&idx` or the param slot; float-typed globals give immediate stores; callbacks passed as real function decls cast to `(void*)`; Ghidra `code` loses `__stdcall`.
- Class (a) early-return layout (`jne;ret` not tail-merged: 0x0053e760, 0x0052fe90, 0x00538600, 0x0053eaa0, 0x00530160) is the S19/S24 kind.
- Follow-up: 0x0053e830 and 0x0053dba0 (raw, near 66%), the 18 rows over 700 B, and rows at 87-96% (0x005327d0, 0x004f2e70, 0x004f3be0, 0x0052fdb0, 0x00538310, 0x00540340).
- Static callees without rows: no file holds a second MATCH function. Helper notes: `notes/h1.md`..`h3.md` (copied from the data dir).

## Raw data (not committed), `D:/MashedWT/MATCH-RWL03_data`
decomp_raw.json, g2s_g5 / g2s_game / g2s_o1 results. decomp_pc.py ran from the main checkout (pool slot Mashed_pool0).

## Reproduce
`py -3.12 verify/matchrwl03_20261108/tools/collect.py`; `relocheck.py re/match/rwl03/<va8>.c` per identical row.

## Merge note
After merging orch/integration (which carried MATCH-RWL01's `(?:[^,;]+,\s*)?`), the relocheck.py conflict was resolved to the stricter `(?:[^,;\n]+,\s*)?` line (a superset of the RWL01 fix); re-running relocheck.py on every row of the RWL01 (70), RWL02 (74), RWL05 (68) and RWL03 (55) relocheck.tsv files printed OK for all, unchanged.
