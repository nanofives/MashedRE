# RESULT MATCH-S27, 2026-11-07

**Verdict: GREEN (match-only, L-93).** Slice 0x005b1160..0x005bd6f0: 136 functions (`slice_rvas.txt`, `slice_rows.tsv`,
`tools/mkslice.py`), all `audio` rows in hooks.csv. The 2 library rows inside the range are skipped
(0x005b8200 and 0x005b8680, `third-party-library[d3dx9-fx]`). Every one of the 136 has a Ghidra size in the regenerated `pc_funcs.csv`, so no
row needed the no-size path.

**62/136 identical** under `re/tools/match/match.py` (XP DDK cl 13.00.9176 / c2 13.00.9178, each file's line-1 flags).
All 62 pass `relocheck.py` with OK (`relocheck.tsv`, 62 lines). 51 have `unchecked=0`. The other 11 have only Win32/COM imports
or a CRT helper unchecked (e.g. `CloseHandle`, `CoTaskMemFree`, `ReleaseSemaphore`, `CoUninitialize`), which the brief allows.
None of the 62 holds an `__asm`. Two files hold one (0x005b4060 at 88.4%, 0x005b4150 at 90.0%, an inline `fnstcw/fldcw/fistp` macro inside a compiled C body)
and both are below 100%. All 136 files pass the VS2022 C++ check (`cxx_all.txt`: 136 CXXOK).
Sources: `re/match/s27/<va8>.c`. Scores, source sha256 and how each row was scored: `results.tsv`.

This step does no body swap, edits no trackers (hooks.csv, CHANGELOG, UNCERTAINTIES, STUBS, pc_funcs.csv) and runs no gates.
A later C4R batch promotes the 62 identical rows.

## Tool overlays (uncommitted in the worktree, restored with `git checkout` before the commit)

- `re/tools/match/match.py` from `orch/C4R-MATCHPY-REL32` (false 1-byte diff on self-recursive calls)
- `re/tools/match/relocheck.py` from `orch/MATCH-S17` (SEH scope tables)
- `re/console/pc_funcs.csv` from `orch/MATCH-PCFUNCS-REGEN` (regenerated sizes)

## Flags

The raw converter ran on all 136 rows under three flag sets: `/O2 /Ob1 /G5` (30 rows at 100%), `/O2 /GS /Ob1` (21 at 100%, all of
them also at 100% under `/G5`) and `/O1` (2 at 100%, also under `/G5`). `tools/g2s_best.py` merges the runs (best per row, ties to `/G5`).
18 rows did not compile in the raw converter (`compile` stage): the cause was a bogus `extern int LPLONG;` / `param_` global (dropped by a one-line edit to `to_s24.py`),
missing typedefs and prototype arity. Every identical file has `// FLAGS: /O2 /Ob1 /G5` on line 1 unless its own header says otherwise
(helpers tried `/O2 /GS /Ob1`, `/O1` and `/O1 /Oy-` per the brief on stalled rows and recorded the winner on line 1).

## Counts

| | count |
|---|---|
| slice functions | 136 |
| raw converter (g2s, best flags) at 100% | 30 |
| hand-matched to 100% (helpers h1-h3, `notes/h*.md`) | 32 |
| **identical total** | **62** |
| below 100% | 74: (a) 16, (b) 57, (c) 1 |
| scored by whole-symbol bytes | 1 (0x005baf90: Ghidra size stops before a jump table, the whole 104 B symbol is byte-identical; `by = bytes` in `results.tsv`) |

The 26 files that had C++ errors (raw-converter files and a few hand files) were fixed with casts, `#ifdef __cplusplus` typedefs and
prototype-arity guards only: the scores and relocheck results were re-measured after the fix and did not move.

## Below 100% (budget spent)

Classes: (a) compiler-version codegen, (b) source shape not found within the budget (includes "not iterated"), (c) tool limit.
Per-row reasons are in `notes/h*.md`. The full table is `below100.md`. Rows the helpers did not iterate (raw Ghidra shape kept, C++ fixes only):
0x005bc0f0, 0x005bbdb0, 0x005b5e00, 0x005b6be0, 0x005bc190, 0x005ba7f0, 0x005b41f0, 0x005b4850. The single (c) row is 0x005bad30
(Ghidra `unaff_EBP` / stack pseudo-variables, boundary or convention wrong).

## Gates

Not run: match-only, the merge train runs them. No build input changed (`re/match/s27/` and `verify/` only).

## Raw data (not committed), `D:/MashedWT/MATCH-S27_data`

- decomp_raw.json (`re/tools/decomp_pc.py --port --callees --create`, pool slot Mashed_pool10) sha256 3cad9a1a362760d65bfd0f28fdd14c12476d24b60618f229677a273c06f72bcd
- decomp.json (non-ASCII comment characters replaced by `?` because g2s.py writes cp1252) sha256 bfe3f97bb5742d99b8b9ad2e07b47dc74bad3be566a47f0fc2986faca2ecbdda
- g2s/results.tsv (merged best) sha256 856e582ef50395a467bfc7d140113441d68c32820379121f4a4173f348f47995
- g2s_g5/results.tsv sha256 0b93dd6e4bce6fc5156511f39f4393cef8adc4af0af05befc687d86742d1f7eb
- g2s_game/results.tsv sha256 6573963157516e85cc6992f1e04cfc063b5be13683c14cba8ccf1fec0dcbf95e
- g2s_o1/results.tsv sha256 6bfec216f160c6d83b4525b60206c9b5a79104c2952bc7533fd2c3af262ab56a

## Reproduce

From the worktree with the three overlays applied: `py -3.12 verify/matchs27_20261107/tools/collect.py > results.tsv`, and
`py -3.12 re/tools/match/relocheck.py re/match/s27/<va8>.c` per identical row. Helper brief: `BRIEF.md`. Assignment: `helper_assign.txt`.
Pool note: the worktree has a junction `mashed_pool` to the main checkout's pool (not committed, not tracked).
