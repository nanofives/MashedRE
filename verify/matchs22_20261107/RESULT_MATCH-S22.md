# RESULT MATCH-S22, 2026-11-07

**Verdict: GREEN (match-only, L-93).** Slice 0x004f022d..0x004fb5a0: 58 functions (`slice_rvas.txt`,
`slice_rows.tsv`, `tools/mkslice.py`), with the 22 library rows inside it skipped.
**35/58 identical** under `re/tools/match/match.py` (XP DDK cl 13.00.9176 / c2 13.00.9178, each file's line-1
flags). All 35 pass `relocheck.py` with OK and `unchecked=0` (`relocheck.tsv`, 35 lines). All 58 files pass the
VS2022 C++ check (`cxxmany.bat`, MATCH_PORT). Sources: `re/match/s22/<va8>.c`. Scores, source sha256 and how each
row was scored: `results.tsv` (every row `by = insn`, no bytes-rule or no-size rows).

This step does no body swap, edits no trackers (hooks.csv, CHANGELOG, UNCERTAINTIES, STUBS) and runs no gates.
Step C4R-MATCHBATCH-2 promotes the 35 identical rows.

## Flags

| rows | flags | evidence |
|---|---|---|
| RenderWare plugin/toolkit rows (default) | `/O2 /Ob1 /G5` | raw converter: `/G5` won or tied on 51 of 58 rows against the game flags, all 14 raw matches are under `/O2 /Ob1 /G5`; flags brief of MATCH-RWCORE-G5 |
| 0x004f022d, 0x004f0307 (D3DX, odd entry addresses, ebp frames, call 0x004eede9) | `/O1` | `flagsweep_o1.tsv`: 0x004f022d 45.9% under `/O1` against 20.0% under `/O2 /Ob1 /G5`, 0x004f0307 91.3% against 36.2%. Both are identical under `/O1` (h1). Same finding as MATCH-S21's 7 D3DX rows |
| 0x004fb050, 0x004fb0e0, 0x004fb4d0 (and the residue rows 0x004fb130, 0x004fb2a0, 0x004fb3e0) | `/O2 /Ob1 /G5 /Oy-` | `flagsweep_o1.tsv`: the three identical rows reach 100% only with `/Oy-` (ebp frame); `/O1` scores 21.1..86.1% |
| 0x004fb5a0 | `/O1` | 74.8% under `/O1` against 15.4% under `/O2 /Ob1 /G5 /Oy-` (h7: only flag set giving `fild dword` + 2^32 fixup, `test ah,1` and RMW adds together) |

**/O1 re-score of the residue.** `flagsweep_o1_residue.tsv` runs the 7 flag sets on the 17 residue rows not in
`flagsweep_o1.tsv`. `/O1` beats the file's flags on none of them. `/Oy-` adds a few points on 3 rows that stay far
below 100% (0x004f57e0 14.2 -> 19.8, 0x004f6870 23.9 -> 28.4, 0x004f79f0 27.2 -> 28.7). Their sources keep the
recorded flags; `results.tsv` scores them as committed.

## Counts

| | count |
|---|---|
| slice functions | 58 |
| raw converter (g2s, `/O2 /Ob1 /G5`) at 100%, relocheck OK | 14 |
| hand-matched to 100% (helpers h1-h7, `notes/h*.md`) | 21 |
| **identical total** | **35** |
| below 100% | 23: (a) 9, (b) 13, (c) 1 |

**C++ fixes in this commit.** 4 raw-converter files (0x004f0940, 0x004f0d80, 0x004f4470, 0x004f4900) failed the
C++ check: Ghidra's `typedef int code();` declares no arguments in C++, and 0x004f0940 passed `&DAT_006189e4` to an
`int` parameter. Each `code` typedef now carries the real argument list of its call site, and the address takes an
`(int)` cast. All 4 stay identical and relocheck OK after the change.

## Below 100% (budget spent)

Classes: (a) compiler-version codegen, (b) source shape not found within the budget, (c) tool limit. The per-row
reason is in `notes/h*.md` and each file's header comment (the header is newer where they disagree: h4's note
gives 0x004f5030 at 26.4%, the file is at 54.9%).

| class | rows |
|---|---|
| (a) one scratch register / pop or return scheduling | 0x004f10e0 92.3, 0x004f8730 96.7, 0x004f8580 91.4 |
| (a) x87 operand order inside products | 0x004fb210 92.2 |
| (a) matrix-flags `and r,0x20000; cmp r,0x20000` folded to `test` under every form | 0x004fb130 81.0, 0x004fb3e0 60.3, 0x004fb2a0 78.5 |
| (a) inline-asm float->int temps get a fresh frame, the original packs them into dead param slots | 0x004f71f0 34.7, 0x004f6870 23.9 |
| (b) at 90% or above | 0x004f13d0 94.0, 0x004f4270 93.5, 0x004f1b00 91.9 |
| (b) other | 0x004f0dc0, 0x004f1150, 0x004f1870, 0x004f3150, 0x004f3d40, 0x004f5030, 0x004f7850, 0x004f79f0, 0x004fa5f0, 0x004fb5a0 |
| (c) inline `__asm` `fld/fistp dword` float->int in the original, C stand-in cannot reproduce the 4 B frame | 0x004f57e0 |

**Inline asm in residue sources.** 0x004f5030.c, 0x004f6870.c and 0x004f71f0.c hold RenderWare's
`RwInt32FromRealMacro` as an `__asm fld/fistp` block, which the brief forbids for port bodies. None of the three is
identical, so no evidence rests on them. Before any of them becomes a port body, the `__asm` has to go (C stand-in,
as 0x004f57e0.c does) or the row moves to library identity.

**Library-identity leads.** 0x004f022d and 0x004f0307 are game-tagged rows built as D3DX code (`/O1`, ebp frames,
callee 0x004eede9 is `third-party-library[d3dx9-psgp]`). h7 reads 0x004fa5f0 as D3D caps init, not zlib.

## Gates

Not run: match-only, the merge train runs them. No build input changed (`re/match/s22/` and `verify/` only).

## Raw data (not committed), `D:/MashedWT/MATCH-S22_data`

- decomp.json sha256 767c17844541a971d161cba90d5d6dfad6f0967d61e7cdb0283f3364bd013228
- g2s/results.tsv (`/O2 /Ob1 /G5`) sha256 d4c45af42233f9112713b0172898cc9cd55dbfc96f10934ec6e4f188cbc7967d
- g2s_game/results.tsv (game flags) sha256 bfc1fd3a770ca524c0a484333f62a868a03874769faae3cbc9b31bc3f147b7fc

## Reproduce

From the worktree: `py -3.12 verify/matchs22_20261107/tools/collect.py > verify/matchs22_20261107/results.tsv`
(reads `slice_rows.tsv` and `g2s/results.tsv` from the data dir), `py -3.12 re/tools/match/relocheck.py
re/match/s22/<va8>.c` per identical row, and `py -3.12 verify/matchs22_20261107/tools/flagsweep_o1.py <va8> ...`
for the flag sweeps (scratch copies only).
