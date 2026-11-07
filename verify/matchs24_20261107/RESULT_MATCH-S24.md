# RESULT MATCH-S24, 2026-11-07

**Verdict: GREEN (match-only, L-93).** Slice 0x00545340..0x0055dec0: 77 functions (`slice_rvas.txt`,
`slice_rows.tsv`, `tools/mkslice.py`). The 189 library rows inside it are skipped (118 `third-party-library[renderware]`,
71 `third-party-library[RenderWare-Physics-3.7]`). slices.tsv says 185. The 77 rows are tagged game code in hooks.csv
but are RenderWare toolkit code: the RtFS file-system handler, a 2D font system with `.met` metrics loading, RW Physics
3.7 body setters, and spline/collision helpers.

**56/77 identical** under `re/tools/match/match.py` (XP DDK cl 13.00.9176 / c2 13.00.9178, each file's line-1 flags).
All 56 pass `relocheck.py` with OK (`relocheck.tsv`, 56 lines). 49 have `unchecked=0`. The other 7 have only Win32
imports or a CRT helper unchecked (`DeleteCriticalSection`, `Enter/LeaveCriticalSection`, `CloseHandle`,
`SetFilePointer`, `GetLastError`, `GetOverlappedResult`, `GetFileAttributesA`, `_strncpy`), which the brief allows.
All 77 files pass the VS2022 C++ check (`cxxmany.bat`, 77 CXXOK). Sources: `re/match/s24/<va8>.c`. Scores, source
sha256 and how each row was scored: `results.tsv`.

This step does no body swap, edits no trackers (hooks.csv, CHANGELOG, UNCERTAINTIES, STUBS) and runs no gates.
Step C4R-MATCHBATCH-2 promotes the 56 identical rows.

## Flags

Every row uses `/O2 /Ob1 /G5`, the RW build with no `/GS` (77 of 77 files on line 1). The raw converter ran on all 77 rows
under three flag sets (`D:/MashedWT/MATCH-S24_data/g2s_g5`, `g2s_game` = `/O2 /GS /Ob1`, `g2s_o1` = `/O1`). 17 rows
are at 100% under `/O2 /Ob1 /G5`. 6 of them reach 100% only under `/G5` (0x00550be0, 0x00552750, 0x00554010, 0x00555280,
0x00557250, 0x0055c810). No row scores higher under the game flags except two low raw forms (0x00556e90 26.8 vs 15.7,
0x00552e40 39.4 vs 35.1). Both later matched or nearly matched under `/G5`: 0x00552e40 is identical and 0x00556e90 is
at 97.5. `/O1` never won. `tools/g2s_best.py` merges the three runs (best per row, ties to `/G5`).

## Counts

| | count |
|---|---|
| slice functions | 77 |
| raw converter (g2s, `/O2 /Ob1 /G5`) at 100%, relocheck OK | 17 |
| hand-matched to 100% (helpers h1-h4, `notes/h*.md`) | 39 |
| of the 56, no Ghidra size (scored with the size forced to the compiled symbol length) | 8 |
| **identical total** | **56** |
| below 100% | 21: (a) 8, (b) 11, (c) 2 |

8 of the raw-converter files needed C++-check edits: casts, a `code(...)` typedef under `__cplusplus`, and callee
prototypes. The DDK bytes are unchanged and relocheck was re-run OK after the edits. Each of those files says so in its
header comment.

**No-size rows.** 12 rows are not in `re/console/pc_funcs.csv`. 8 of them are identical, with `by = nosize` in
`results.tsv` and `bytes_bad` empty. `nosize_ends.tsv` lists where each one ends: on `90` padding up to the next
16-byte boundary, read from the original image. These are 0x00550c30, 0x00550d80, 0x00551050, 0x00551180,
0x00551290, 0x005512d0, 0x005514e0 and 0x00555910. The other 4 (0x00550c50, 0x00550da0, 0x00550f20, 0x00554940) are
below 100%. `pc_funcs.csv` is not changed here. Follow-up: add the 8 rows, as MATCH-S19-FU-PCFUNCS did for S19.

**Static callees.** 00554200.c holds 0x00553e80 (Destroy): 100% (37/37 instructions) and relocheck OK in that file.
0x00553e80 has a pc_funcs size (100 B) but **no hooks.csv row**, so it is not counted in the 56. Follow-up: give it a
row so a later promotion can take it. 00550430.c, 00550b00.c, 00554200.c and 00547bf0.c use file-local
`static __forceinline` helpers that have no address of their own (the lookup inlined into the caller). They are not
separate functions.

## Below 100% (budget spent)

Classes: (a) compiler-version codegen, (b) source shape not found within the budget, (c) tool limit. The per-row
reason is in `notes/h*.md`.

| rva | size | best % | regs % | class | residual |
|---|---|---|---|---|---|
| 0x00555af0 | 1072 | 99.3 | 99.3 | b | int temp for the final `(float)(y1-y0)` lands in a different dead parameter slot (2 instructions) |
| 0x00556e90 | 631 | 97.5 | 97.5 | a | x87/integer spill-copy scheduling (colour twin of 0x00557110); permuter 307 compiles, no gain |
| 0x00557110 | 169 | 96.5 | 96.5 | a | first `fcomp` scheduled after the spilled +0x5c integer copy; rewrites, flags, permuter |
| 0x00555f20 | 204 | 96.1 | 96.1 | a | `n = max-min+1` as inc/mov vs lea; spellings, flags, permuter |
| 0x00550da0 | 366 | 94.9 | 94.9 | b | `state = 2` store hoisted above the position compare (no size) |
| 0x00555360 | 353 | 94.4 | 94.4 | b | original ends three failed checks in separate `jne; ret`, ours tail-merges into one `xor eax,eax; ret` |
| 0x00551190 | 251 | 90.7 | 90.7 | a | `push esi` before the early return in the original, shrink-wrapped in ours (S19 class) |
| 0x00550f20 | 296 | 90.6 | 97.2 | b | WriteFile argument register choice and position load order (no size) |
| 0x00546e70 | 949 | 87.0 | 87.0 | a | slerp-cache setup with inline acos: x87 operand order in 9 products/sums, `fst` vs `fld+fstp` |
| 0x00546b10 | 214 | 86.0 | 89.5 | a | `push ebx` shrink-wrapped past the NULL-check return (S19 class); all other instructions equal |
| 0x00558b40 | 258 | 85.1 | 85.1 | a | x87 operand order in two dot products (S19 class) |
| 0x00555ff0 | 1932 | 81.9 | 84.8 | b | 558/558 instructions in place; stack-slot/register assignment and one byte-store schedule. The permuter's best variant changed behaviour and was rejected |
| 0x00550c50 | 297 | 79.8 | 81.8 | a | ebx/esi pushes shrink-wrapped past the early return (S19 class; no size) |
| 0x00556780 | 1300 | 75.5 | 80.1 | b | font `.met` loader, ops 94.6%; frame-slot and register assignment (ebp/ebx swapped) |
| 0x00554200 | 397 | 69.5 | 75.9 | b | result join threaded in ours, one join block with result in ebp in the original |
| 0x00547450 | 1951 | 66.8 | 66.8 | b | sphere-triangle test, ops 87.4%; frame layout swaps two V3d slots, plus x87 operand order |
| 0x00556ca0 | 22 | 60.0 | 60.0 | b | tail call re-stores an unchanged arg 3; no source form found |
| 0x00547bf0 | 5807 | 46.0 | 49.9 | b | structure pass only, at the 60-min cap |
| 0x00545340 | 1663 | 35.8 | 45.8 | b | structure right; frame and register allocation differ |
| 0x005572c0 | 1494 | 17.9 | 38.4 | c | 8 colour float-to-int conversions are inline `fnstcw/fldcw/fistp` (an `__asm` macro in the original). C gives `call __ftol` and the brief bars `__asm` |
| 0x00554940 | 1417 (sym) | 12.8 | 30.0 | c | 16 inline `fnstcw/fldcw/fistp` conversions, same as 0x005572c0 (no size; original runs to 0x005551cb) |

The two (c) rows match only with an inline-asm float-to-int macro inside a compiled C body. The MATCH section counts
that form as match evidence (an `__asm` block inside a compiled C body, not a naked transcription), but this slice's
brief barred `__asm`. A follow-up could allow one `__asm` macro for these two rows.

## Gates

Not run: match-only, the merge train runs them. No build input changed (`re/match/s24/` and `verify/` only).

## Raw data (not committed), `D:/MashedWT/MATCH-S24_data`

- decomp_raw.json (`re/tools/decomp_pc.py --port --callees --create`, pool slot Mashed_pool0) sha256 0ceb25e6022926efedcec0d2e2a2e05d57b57b743a2e8040aaa78495a0ef13e8
- decomp.json: the same JSON with non-ASCII comment characters (arrows and dashes from master-project plate comments)
  replaced by ASCII, because g2s.py writes cp1252 and crashed on them. sha256 3cbe601a4050c7840814e3025a01595f4321d9db080e6a831c369d8a345fbb8c
- g2s/results.tsv (merged best) sha256 32c02763ec186142be759d18c57795460e774f74ae4d15afe22810e508c527cf
- g2s_g5/results.tsv sha256 bbce4567fc5370361d1bfd9fb4570d2310b9c84fecd39476e4828e2c6c5e61e5
- g2s_game/results.tsv sha256 3968b1d48666aef48c81bb25a6eef7e8879b00496d081c8dee240910b3387655
- g2s_o1/results.tsv sha256 de6c8b81e4b68ad2c5f211eb330b85dd0809b99e578c4554f06dc36cbf40e585

## Reproduce

From the worktree: `py -3.12 verify/matchs24_20261107/tools/collect.py > results.tsv` (reads `slice_rows.tsv` and
`g2s/results.tsv` from the data dir), and `py -3.12 re/tools/match/relocheck.py re/match/s24/<va8>.c` per identical
row. Helper brief: `BRIEF.md`. Assignment: `helper_assign.txt`.
