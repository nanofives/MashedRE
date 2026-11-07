# RESULT MATCH-S20, 2026-11-07

**Verdict: GREEN (match-only, L-93).** Slice 0x004c7860..0x004d41e0: 117 game functions (33 library rows inside
skipped, `slice_rvas.txt`, `tools/mkslice.py`). **79/117 byte-identical** under `re/tools/match/match.py` (XP DDK
cl 13.00.9176 / c2 13.00.9178), and all 79 have `relocheck.py` OK (`relocheck.tsv`): 77 with `unchecked=0`,
0x004c8800 and 0x004d2e30 with one unchecked import each (`GetWindowRect`, `GetClientRect`, imports are unchecked by
name). No matched file uses `__asm` (two sub-100 files, 0x004cb490 and 0x004cf160, name it only in their header).

Scope per L-93: sources under `re/match/s20/`, `results.tsv`, `relocheck.tsv`, this file. **No body swap into
`mashedmod/src`, no tracker edits** (hooks.csv, CHANGELOG, UNCERTAINTIES, STUBS untouched). C4R-MATCHBATCH-2 promotes
the 79 identical rows. Gates: not run (match-only, the merge train runs them).

## Flags: the whole slice is RW core

Every file is `// FLAGS: /O2 /Ob1 /G5` (one, 0x004cb4c0, adds `/Oy-`). The raw converter ran twice, with the game
flags `/O2 /GS /Ob1` and with `/O2 /Ob1 /G5` (the MATCH-S19 RW-core finding): /G5 scored higher on 58 functions,
equal on 43, lower on 16, and raw 100% went 16 -> 20 under /G5. No hand-matched function ended on the game flags.

## Static leg

| | count |
|---|---|
| slice functions | 117 |
| raw converter (g2s, best of both flag sets) at 100% | 20 (19 relocheck OK as converted, 0x004d3d50 after writing RwGlobals first) |
| hand-matched to 100%, round 1 (helpers h1-h4) | 57 |
| hand-matched to 100%, round 2 (h5, h6 on 20 near-misses) | 2 (0x004cb190, 0x004ccf20) |
| **identical** | **79** |
| below 100% | 38 (table below; budget spent) |

Round 2 also moved 0x004c9cd0 90.2 -> 96.5, 0x004cb4c0 83.7 -> 90.2 (`/Oy-`), 0x004d1e30 87.5 -> 87.9.

Faithfulness flag for a later port leg: `re/match/s20/004cbe80.c` (78.6%) has the default case call FUN_004d8480(&err)
before err is filled; the original fills it first. Faithful store orders top out at 71.7%
(`D:/MashedWT/MATCH-S20_data/scratch/h6/cbe80_keep.c`, helper_notes_h6.md). Not identical, so not promoted either way.

| rva | size | best % | regs % |
|---|---|---|---|
| 0x004c7a70 | None | - | - |
| 0x004c8c70 | 472 | 66.2 | 89.7 |
| 0x004c9cd0 | 443 | 96.5 | 96.5 |
| 0x004c9eb0 | 147 | 94.8 | 94.8 |
| 0x004c9fb0 | 424 | 81.3 | 85.6 |
| 0x004caea0 | 300 | 77.5 | 86.2 |
| 0x004cb490 | 42 | 83.3 | 88.9 |
| 0x004cb4c0 | 320 | 90.2 | 90.2 |
| 0x004cba80 | 76 | 90.3 | 90.3 |
| 0x004cbb70 | 82 | 93.3 | 96.7 |
| 0x004cbbd0 | 130 | 75.5 | 77.4 |
| 0x004cbe80 | 445 | 78.6 | 93.1 |
| 0x004cc050 | 250 | 78.5 | 81.7 |
| 0x004cc230 | 440 | 48.0 | 45.4 |
| 0x004cc5e0 | 244 | 59.8 | 60.9 |
| 0x004cc6e0 | 142 | 45.9 | 54.1 |
| 0x004cd500 | 71 | 83.3 | 100.0 |
| 0x004cd550 | 644 | 50.0 | 60.4 |
| 0x004ce480 | 333 | 89.7 | 89.7 |
| 0x004ce970 | 375 | 71.5 | 38.7 |
| 0x004cecb0 | 468 | 79.6 | 79.6 |
| 0x004cf160 | 325 | 45.6 | 46.8 |
| 0x004cf4a0 | 244 | 74.3 | 74.3 |
| 0x004cf7d0 | 514 | 84.2 | 86.7 |
| 0x004cff20 | 842 | 62.5 | 68.2 |
| 0x004d0290 | 632 | 73.4 | 81.5 |
| 0x004d0660 | 251 | 64.9 | 64.9 |
| 0x004d07b0 | 1351 | 77.0 | 74.7 |
| 0x004d0d00 | 695 | 35.5 | 67.8 |
| 0x004d1020 | 1460 | 42.1 | 57.6 |
| 0x004d1640 | 1735 | 92.4 | 92.4 |
| 0x004d1d70 | 162 | 72.1 | 98.4 |
| 0x004d1e30 | 1270 | 99.1 | 98.6 |
| 0x004d2340 | 1548 | 37.3 | 43.5 |
| 0x004d3510 | 914 | 55.8 | 75.3 |
| 0x004d3b00 | 585 | 30.6 | 55.6 |
| 0x004d3e90 | 546 | 82.8 | 86.6 |
| 0x004d40d0 | 159 | 89.1 | 97.8 |

0x004c7a70 (2811 B, raw converter did not compile, no Ghidra size in `pc_funcs.csv`) was not attempted. Residual
class and what was tried per row: `helper_notes_h1.md`..`helper_notes_h6.md` (class (a) compiler codegen: 0x004c8c70,
0x004cb4c0 round 1, 0x004cc5e0, 0x004cc6e0, 0x004d3510, 0x004d07b0 (locals in dead parameter homes, frame layout);
(c) tool limit: 0x004cb490, 0x004cf160 (inline-asm x87 conversion); the rest (b) source shape not found in budget,
mostly callee-saved push shrink-wrap, cross-jumped error tails and register rotation).

## Raw data (not committed), `D:/MashedWT/MATCH-S20_data`

| file | sha256 |
|---|---|
| decomp.json | 545c3617ecbccfc34c6f90c8a206c5c95debff0bbebd538137c6bac29f4ebdbc |
| g2s/results.tsv (best of both) | 727122ed46b3d4063be22b7ac717e7d3e769e6905ed47e7d2bd5674cb41ae81c |
| g2s_game/results.tsv (/O2 /GS /Ob1) | 634391e44a77e38ac387c67d2f7546055c4e6f1de682cb0154435d700296ed8e |
| g2s_g5/results.tsv (/O2 /Ob1 /G5) | 036515e6769978a19b2dc9dc287b75e8bbd671fb6d72df66b685d860088f1d7e |
| notes/h1..h6.md, scratch/ | helper reports (copied here as helper_notes_h*.md), scratch |

## Follow-ups (not filed)

- 0x004c7a70: add to `re/console/pc_funcs.csv` and give it a first pass.
- Port leg for the 79 (body swap per MATCH-S19's SWAP_BRIEF.md) when the port axis picks this range.
