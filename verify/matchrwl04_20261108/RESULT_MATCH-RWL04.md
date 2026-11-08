# RESULT MATCH-RWL04, 2026-11-08

**Verdict: GREEN (match-only, L-93, partial slice with honest counts).** Slice 0x005412d0..0x005540d0: 126 rows, all
`third-party-library[renderware]` (list `D:/MashedWT/MATCH-RWLIB_data/rwl04_rows.tsv`, copied to
`D:/MashedWT/MATCH-RWL04_data/slice_rows.tsv` by `tools/mkslice.py`). 125 rows have a source in `re/match/rwl04/<va8>.c`.
0x005422c0 (4666 B) has none, see below.

**60/126 identical** under `re/tools/match/match.py` (XP DDK cl 13.00.9176 / c2 13.00.9178, line-1 flags). All 60 pass
`relocheck.py` with OK (`relocheck.tsv`). 59 have unchecked=0. 0x00550350 has only the import
`InitializeCriticalSection` unchecked. All 60 pass the VS2022 C++ check (`cxx_check.txt`). Scores, sha256, class and
reason per row: `results.tsv`. No trackers, no build, no gates. Promotion is left to a later `c4_reclass.py` batch.

## Flags evidence (g2s raw converter on all 126 rows, 111 compiled under each set)

| flag set | 100% |
|---|---|
| `/O2 /Ob1 /G5` (RW build) | 28 |
| `/O2 /GS /Ob1` (game) | 19 |
| `/O1` | 9 |

8 rows are 100% only under `/G5`. No row is 100% only under the game flags or only under `/O1`. The game flags beat
`/G5` on 14 low raw rows, the best being 0x00543710 at 76.7 vs 75.4. That row later matched 100% under `/G5` (helper h3).
`/O1` beat both on 7 low raw rows, best 0x0054dbd0 at 78.9 vs 78.2. All 60 identical rows use `/O2 /Ob1 /G5`. Files below
100% whose line 1 differs: `/O2 /GS /Ob1` on 3 rows (incl. 0x0054a120), `/O1` on 2 rows (0x0054d4d0, 0x0054dbd0), and
`/O2 /Os /Ob1 /G5` on 0x00551460. Each one is the best its helper found, so these rows give no evidence for any flag
set. `tools/g2s_best.py` merges the runs (best per row, ties go to `/G5`).

## Counts

| | |
|---|---|
| rows | 126 |
| raw converter 100% with relocheck OK | 28 (9 of them needed C++-check edits, listed below) |
| hand-matched to 100% (helpers h1 7, h2 10, h3 15; `notes/h*.md`) | 32 |
| **identical** | **60** |
| below 100% | 66: class (a) 19, (b) 46, (c) 1 |
| no Ghidra size | 0 (every row is in `re/console/pc_funcs.csv`) |

These 9 raw rows got C++-check edits: 0x00543a40, 0x00546320, 0x00549620, 0x005499f0, 0x00550750, 0x00551410, 0x005522e0,
0x00553ef0 and 0x005540d0. The edits are casts plus `typedef int code(...)` under `__cplusplus`. The DDK score stayed
100%, relocheck was re-run OK, and each file's line 2 says so. 0x00546bf0 (h3) declared its globals as `_DAT_005cc320` /
`_DAT_005cc32c`. Relocheck could not resolve that spelling and left both unchecked, so they were renamed to `DAT_...`.
The score is still 100% and relocheck now checks 4 references with 0 unchecked.

## Below 100%

Classes: (a) compiler-version codegen of the S19/S24 kind: shrink-wrapped callee-saved pushes, unmerged
`xor eax,eax; ret` epilogues, x87 operand order, `add` operand order. (b) source shape not found within the budget. 26
rows are still raw or structure-only, which is the cheapest next target. (c) tool limit. The per-row residual is in
`results.tsv` and `notes/h*.md`. Closest rows:

| rva | size | best % | regs % | class | residual |
|---|---|---|---|---|---|
| 0x00551a50 | 121 | 96.4 | 98.2 | b | eax reload + `lea edx,[eax+8]` vs `mov edx; add edx,8`; permuter no gain |
| 0x00549900 | 112 | 94.9 | 94.9 | a | `fisub dword` vs `fild qword; fsubp` |
| 0x00550580 | 237 | 93.5 | 93.5 | a | `xor esi,esi; mov eax,esi` on the fail return |
| 0x0054b630 | 150 | 93.0 | 96.5 | b | memcpy intrinsic, spill slot numbering |
| 0x00551190 | 251 | 90.7 | 90.7 | a | shrink-wrapped `push esi` (same row as S24's) |
| 0x0054d4d0 | 1785 | 90.5 | 90.5 | a | `/O1` wins; orig reloads stored floats from memory |
| 0x00551550 | 77 | 89.2 | 89.2 | a | orig pushes esi/edi at entry, ours shrink-wraps |
| 0x0054e520 | 1646 | 88.4 | 88.4 | b | ops 95.7%; see the 6.0f note below |
| 0x00552720 | 43 | 6.7 | 100.0 | a | ops identical, the two pointer params swap registers |
| 0x00543b90 / 0x00543da0 | 21 | 60.0 | 100.0 | a / b | `add ecx,eax` vs `add eax,ecx` only |
| 0x005422c0 | 4666 | - | - | c | raw converter refused it (`NAN` operand with no address), no source written |

**Not eligible as written:** 0x0054e520 and 0x0054eb90 (both below 100%) use a `6.0f` literal in place of the global at
0x005cd0a0. That raised their scores, but a literal instead of the `MGLOBAL` would fail relocheck at 100%. A follow-up
must restore the global form.

C++ check: 115/125 files pass. The 10 failures are all below 100%: 0x005412d0, 0x00541d40, 0x005493e0, 0x0054b340,
0x0054ba20, 0x0054bda0, 0x0054f8d0, 0x00550130, 0x005531f0 and 0x00553620. They are raw bodies with callee
arity/pointer-type mismatches, to be fixed when those rows are matched.

## Static callees and no-size rows
No file holds a second `// MATCH:` function. No no-size rows.

## Tool change
`re/tools/match/relocheck.py`: the `RE_G` regex optional middle group changed from `(?:[^,]+,\s*)?` to `(?:[^,;]+,\s*)?`.
This is the same one-line fix as orch/MATCH-RWL01's, which is not yet on orch/integration. The old group let an
`MGLOBAL(...)` line match across the `;` into the next line, for example a call with a hex argument such as
`(DAT_00912a24,0x301a1)`, so relocheck took `0x301a1` as the global's address. 0x00553ef0 reported
`BAD +0x7 _DAT_007d3ff8: ours 0x000301a1`, and after the fix it reports OK. A merge with RWL01 gives the identical line.

## Follow-ups
- The 26 raw, structure-only or compile-fix-only (b) rows (listed in `results.tsv`), first the ones with ops over 85%: 0x0054ceb0,
  0x00551ad0, 0x0054e520.
- 0x005422c0: write the source by hand from `odis.py` (4.6 KB, structure pass).
- 0x005459c0 and 0x00545d30: the raw converter's `ROUND()` stub needs the real float-to-int idiom. It may be the
  inline `fistp` macro of S24 class (c).

## Raw data (not committed), `D:/MashedWT/MATCH-RWL04_data`
- decomp_raw.json (`re/tools/decomp_pc.py --port --callees --create`, pool slot Mashed_pool0, run from the main checkout): sha256 29c697a05ee75feea32e8b61987f5f3abe60491963605fa7db80462794fb9e74. decomp.json is the same with non-ASCII replaced (g2s writes cp1252).
- g2s_g5/results.tsv 4132d48cd4734ca7ed73c38eb0e114684e399df986218e40be51847fb1c343d8
- g2s_game/results.tsv 2f21b04abfa92fe18e245a6ed11f7883fcf97677a5d00b6185350dfa410f5fc4
- g2s_o1/results.tsv 9e143beaa1bef6cf7653f7cd1ee485c06f3784468b14a4d5a71f5e7978c31259

## Reproduce
From the worktree: `py -3.12 verify/matchrwl04_20261108/tools/collect.py > verify/matchrwl04_20261108/results1.tsv`,
then `tools/mkresults.py`. Run `relocheck.py re/match/rwl04/<va8>.c` for each identical row. Helper brief: `BRIEF.md`.
Assignment: `helper_assign.txt` (3 helpers, round-robin by size).
