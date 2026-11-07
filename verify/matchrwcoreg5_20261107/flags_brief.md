# Flags brief for RenderWare-range MATCH slices (MATCH-RWCORE-G5, 2026-11-07)

For MATCH-S20 (0x004c7860..0x004d41e0) and MATCH-S21 (0x004d4250..0x004ecc6f), and any later slice inside the RW core.

## Use this on line 1 of every RW-core source

```
// FLAGS: /O2 /Ob1 /G5
```

Compiler as for every slice: XP DDK cl 13.00.9176 / c2 13.00.9178 (`tools/vc7ddk`), C, `re/tools/match/match.py`.
In Git Bash set `MSYS_NO_PATHCONV=1` before passing `--flags "/O2 ..."`, or MSYS rewrites `/O2` into a path
(`flags=C:/Program Files/Git/O2` in the match.py header line is the symptom).

## Evidence

| claim | evidence |
|---|---|
| RW core starts at 0x004c0740 | last `third-party-library[lua-4.0]` row in hooks.csv is 0x004c0700; 0x004c0740 is the first row after it and is identical only with `/G5` (sweep below) |
| RW core ends before 0x004ec760 | first `third-party-library[d3dx9-psgp]` row is 0x004ec760; the last `[renderware]` row of that segment is 0x004ebc30 |
| `/G5` is required | all 117 S19 sources scored under three flag sets (`sweep_all.tsv`, raw): 50 are 100% only with `/G5`, 34 are 100% with and without it, 0 are 100% only without it. The 50 span 0x004c0740..0x004c7760, the whole S19 slice |
| no `/GS` | 0x004c6750 holds `char[256]` locals and has no stack cookie: 83.3% with `/O2 /Ob1 /G5`, 74.2% with `/GS` added (S19 flagsweep). On the 84 identical S19 rows `/GS` changes nothing (none has a cookie-eligible buffer). Over all 117, `/GS` changes the score of 3 rows, each one higher without it (0x004c6580, 0x004c6750, 0x004c6ba0). Leave it off everywhere in the range |
| `/O2 /Ob1 /G5` alone reproduces every identical S19 row | 84 of 84 are 100% under it, and `relocheck.py` on scratch copies with line 1 replaced by `/O2 /Ob1 /G5` gives 84 OK, `unchecked=0` (`relocheck.tsv` here) |
| why `/G5` | libid (`verify/libidentity_20261030/RESULT_LIB-IDENTITY.md`, "Flags found"): with the default `/G6` blend, 9178 splits read-modify-write memory ops (`add [esi],0x10` becomes load, add, store). S19 residuals without `/G5` were the same split plus `or al,1` byte-op forms |

`/G3`, `/G4` and `/G5` give the same score on all 33 S19 files in the S19 flagsweep (and the same bytes on the files
helpers h5 and h6 tried). Record `/G5`.

## Flag sets that did not help (33 below-100% S19 rows, this step)

`/Ox /G5`, `/O2 /G5` (Ob2), `/O2 /Ob0 /G5`, `/O2 /Ob1 /G5` plus each of `/Op`, `/Oy-`, `/Oa`, `/Ow`, `/QIfist`,
`/Gs`, `/GF /Gy`; `/Og /Oi /Ot /Ob1 /G5`, `/O1 /G5`, `/Ox /Ob1 /G5 /Op`, `/O2 /Ob1 /G6`, `/O2 /Ob1 /GB`.
No row reached 100%. Every row at or above 85% is unchanged by every set. Low structural rows moved
(0x004c51a0 14.7 -> 32.8 with `/QIfist`, 0x004c6ba0 23.2 -> 35.3 with `/Oy-`, 0x004c6580 36.6 -> 43.4 with `/Ow`),
which says those sources are wrong in shape, not that the RW core used those switches. Do not adopt them.

## Residuals that flags do not move (expect them in S20/S21)

From S19 and this step's permuter (11 rows, 1,940 compiles, 0 gains): x87 operand order inside products and dot
products (`fld m; fmul st(i)` against `fld st(i); fmul m`), callee-saved `push` shrink-wrapped past an early return or
around a loop in ours, a sibling-search loop that reloads through the old register. libid reports the same classes on
Lua and qhull and marks as [UNCERTAIN] whether they come from the 9178 -> 9466 compiler difference (the anchor's Rich
header lists `28/9466 x373`; no 13.00.9466 c2 is available). Budget accordingly: do not spend permuter time on a row
whose only residual is one of these.

## Not RW inside the S20/S21 ranges

- 0x004d1000 is `third-party-library[zlib-1.1.3]` (S20). zlib is its own project; score it under `/O2 /Ob1 /G5` and
  the game flags both and record whichever matches. No zlib flag evidence exists yet.
- S21's 13 `third-party-library[d3dx9-psgp]` rows and every row from 0x004ec760 on are D3DX, not RW. They are library
  rows skipped by the slice; the RW flags say nothing about them.
- hooks.csv tags many RW-core functions as `render`, `frontend` or `util` (S19: 94 `render`, 28 `[renderware]`). The
  subsystem tag does not tell RW from game code here. Score by address range, not by tag.
- 0x004c4680 (matrix orthonormalize, S19) is 34.5% under the game flags and 29.7% under `/O2 /Ob1 /G5`. Its source is
  a first structural pass, so this does not show it is game code. [UNCERTAIN] which project built it: a source at
  >=85% under either flag set is needed to tell.
