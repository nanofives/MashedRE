# RESULT MATCH-S21, 2026-11-07

**Verdict: GREEN (match-only, L-93).** Slice 0x004d4250..0x004ecc6f: 131 functions (`slice_rvas.txt`,
`tools/mkslice.py`), with the 216 library rows inside it skipped.
**89/131 identical** under `re/tools/match/match.py` (XP DDK cl 13.00.9176 / c2 13.00.9178, each file's line-1
flags). All 89 pass `relocheck.py` with OK and `unchecked=0` (`relocheck.tsv`, 89 lines). Every hand-matched file
also passes the VS2022 C++ check (`cxx:ok` in the helper notes). Sources: `re/match/s21/<va8>.c`. Scores, source
sha256 and how each row was scored: `results.tsv`.

This step does no body swap, edits no trackers (hooks.csv, CHANGELOG, UNCERTAINTIES, STUBS) and runs no gates.
Step C4R-MATCHBATCH-2 promotes the 89 identical rows.

## Flags

| range | flags | evidence |
|---|---|---|
| 0x004d4250..0x004ec750 (RW core, 124 rows) | `/O2 /Ob1 /G5` | flags brief of MATCH-RWCORE-G5. Raw converter: 40 rows at 100% under `/O2 /Ob1 /G5` against 7 under `/O2 /GS /Ob1`, and none at 100% only under the game flags (`D:/MashedWT/MATCH-S21_data/g2s_g5`, `g2s_game`) |
| 0x004ec839..0x004ecc6f (D3DX, 7 rows, odd entry addresses) | `/O1` | `d3dx_flagsweep.txt`: 0x004ec839 scores 82.3% under `/O1` against 38.7% under the game flags and 27.4% under `/O2 /Ob1 /G5`. `/O1 /Oy-`, `/O1 /GS` and `/O1 /Ob1` give the same score. All 7 D3DX rows are identical under `/O1` (helper h4) |

New finding: these D3DX rows (Vec2/Vec3 Hermite, Catmull-Rom, barycentric, float16 conversion, one `jmp [0x006187c4]`
thunk) come from a `/O1` build. They are game-tagged rows in hooks.csv. The `third-party-library[d3dx9-psgp]` rows
skipped inside this range were not scored here.

## Counts

| | count |
|---|---|
| slice functions | 131 |
| raw converter (g2s, `/O2 /Ob1 /G5`) at 100%, relocheck OK | 40 |
| no Ghidra size, raw form identical (scored with the size forced to the compiled symbol length) | 4 |
| hand-matched to 100% (helpers h1-h4, `notes/h*.md`) | 45 |
| **identical total** | **89** |
| below 100% | 42: (a) 14, (b) 20, (c) 8 |

**No-size rows.** 10 rows are not in `re/console/pc_funcs.csv`: 0x004d8430, 0x004d8470, 0x004d8530, 0x004d8550,
0x004d86d0, 0x004d8700, 0x004d8730, 0x004d8750, 0x004d8b70, 0x004d8fa0. `tools/collect.py` and `tools/nosize.py`
force each one's size to the compiled symbol's length, and only when that symbol ends at or before the next known
entry. All 10 are identical with `bytes_bad` empty (`by = nosize` in `results.tsv`). Each ends at the next entry
(0x004d8470, 0x004d8fa0 at 0x004d9000) or on `90` padding before a 16-byte boundary (helper notes give the end
addresses). `pc_funcs.csv` is not changed here. Follow-up: add the 10 rows, as MATCH-S19-FU-PCFUNCS did for S19.

**Static callees held in files:** 004d5bc0.c and 004d7480.c hold 0x004d5480, 004d9050.c holds 0x004d94a0
(`__forceinline`, so one level inlines), and 004d9530.c holds 0x004da430 and 0x004da480. Each held callee also has
its own file. 0x004d5480, 0x004da430 and 0x004da480 are identical in their own files. 0x004d94a0 is at 59.6% in its
own file.

## Below 100% (budget spent)

Classes: (a) compiler-version codegen, (b) source shape not found within the budget, (c) tool limit. The per-row
reason is in `notes/h*.md`.

| class | rows |
|---|---|
| (c) inline asm in the original (cpuid / EFLAGS.ID tests, SSE point and vector transforms, `fnstcw/fistp` float-to-int macro). The brief forbids `__asm`, so a C stand-in stays | 0x004dcf90, 0x004dcff0, 0x004dd050, 0x004dc160, 0x004dc1f0, 0x004dc280, 0x004d9a60, 0x004dabd0 |
| (a) x87 operand order in products and sums | 0x004d9360, 0x004d9ee0, 0x004da000, 0x004daff0 |
| (a) constant-multiply decomposition (`*59`, `*30`) | 0x004dfab0, 0x004dfb10, 0x004dfb60 |
| (a) callee-saved push shrink-wrapped past an early return, prologue scheduling | 0x004d8300, 0x004d8480, 0x004d42b0, 0x004e1df0 |
| (a) other | 0x004daa00 (self tail call compiled as a loop), 0x004d80d0 (scratch-register rotation), 0x004d9d00 (spills in dead argument homes) |
| (b) at 95% or above | 0x004d4a20 99.4, 0x004d4f00 98.5, 0x004d7480 96.2, 0x004d4310 95.7 |
| (b) other | 0x004d4250, 0x004d5190, 0x004d5bc0, 0x004d6200, 0x004d6910, 0x004d7100, 0x004d8c40, 0x004d94a0, 0x004d9530, 0x004d9ba0, 0x004da270, 0x004da4f0, 0x004dba60, 0x004dfbc0, 0x004e02d0, 0x004e66d0 |

0x004dcf90, 0x004dcff0, 0x004dd050 (cpuid) and the SSE rows are naked machine-code shapes. A naked inline-asm
transcription is not match evidence (MATCH section), so these 8 rows need library identity or the port axis.

## Gates

Not run: match-only, the merge train runs them. No build input changed (`re/match/s21/` and `verify/` only).

## Raw data (not committed), `D:/MashedWT/MATCH-S21_data`

- decomp.json (decomp_pc.py --port --callees, pool slot Mashed_pool0) sha256 9cf36bdf1bd35845bb825fb3eea82a20a850b8b06bdd70714e3441e7cf054858
- g2s_g5/results.tsv sha256 07713dac2d66cb50466986177ec14678c74871d98d12f748b03a1f95018625c3
- g2s_game/results.tsv sha256 3ecb368a4ac3cb6bbaa8602a59207b959cb1a9a34d24eeefe0a7afa396f5a713

## Reproduce

From the worktree: `py -3.12 verify/matchs21_20261107/tools/collect.py > results.tsv` (reads `slice_rows.tsv` and
`g2s/results.tsv` from the data dir), and `py -3.12 re/tools/match/relocheck.py re/match/s21/<va8>.c` per identical
row.
