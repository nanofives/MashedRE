# MATCH-SCOPE: cut of the game functions into match slices (2026-10-31)

Text-only. No matching, no Ghidra, no tracker write, no ledger write other than the new rows below.

## Verdict

28 slices, MATCH-S01..S28, 3,403 game functions in total (range 54 to 157 per slice, 24 of 28 within 96-137).
Table: `slices.tsv` (id, first and last RVA, game functions, library rows skipped inside the range, hooked-status count, C2+ count, notes).
Reproduce: `py -3.12 verify/matchscope_20261031/cut_slices.py` from the repo root.

## Method

- Function list: `hooks.csv`, one row per RVA (5,920 rows, 5,907 unique). 13 rows were duplicates (first kept).
  Mixed RVA notation: rows written as an RVA (below 0x400000, 0x00095280 style) were normalized by adding 0x400000
  to the VA form every other row uses.
- Library = every row whose subsystem starts `third-party-library` (2,407 rows: Lua 4.0, qhull, CRT, d3dx9, DirectShow
  BaseClasses, libpng, RenderWare) plus every address LIB-IDENTITY (`orch/LIB-IDENTITY` @ 8c18faf96,
  `libidentity_functions.tsv`, copied here) lists as `identical`, `identical-ambiguous` or `identical-unlisted`
  (97 more addresses under game subsystems or without a row). Total library 2,504, game 3,403.
- Cut: the sorted game list is split at library runs of 40 or more consecutive library rows (DirectShow, Lua, qhull,
  d3dx9, the large RenderWare blocks), so no slice crosses a large library block. Each block is cut into
  round(n/125) balanced slices. Slices under 30 functions merge into a neighbour (cap 150).
- Smaller library fragments (CRT, RenderWare pieces of under 40 rows) stay inside a slice and are not counted as game
  functions. The `lib_fns_inside` column gives the number per slice. Slices S17, S21, S24, S28 carry 123 to 269.
- Slices S17, S18, S22 and S24 hold fewer than 100 functions because the game code there is fragmented between library
  code. S23 has 157. These are the shortfalls against the 100-150 target.

## Notes for the slice workers

- `hooked_status` counts `status == hooked` only. hooks.csv `status` is mostly `mapped` / `impl`, so it is not a
  coverage figure. `c2plus` is the number of rows at C2 or higher, almost all of them.
- 122 hooks.csv rows are not in `re/console/pc_funcs.csv` (5,798 of 5,800 pc_funcs entries are in hooks.csv). Not
  investigated here. A worker whose slice lists an address Ghidra has no function at reports it, does not guess.
- Each MATCH-Snn row can start once MATCH-MERGE is merged. A slice's re-classify step needs MATCH-RUBRIC merged first.

## Ledger rows added

MATCH-S01 .. MATCH-S28, section MATCH, wave 4, deps none, lowest RVA first. Titles carry the RVA range and function count.
