# C4R-C2-PROBES: which cheap routes turn C2/C3 rows into C4 (2026-11-07)

Rows: C4R-C2-FLAGSWEEP and C4R-DSHOW-BASECLASSES. Branch `orch/C4R-C2-PROBES`. Text and tracker only: no mashed_re
build, no game run.

**Verdict: GREEN.** Library identity against the BaseClasses source moved **126 rows to C4**. The flag sweep moved
**0 rows**: flags are not what holds the 746 game match rows below 100%.

## A. C4R-C2-FLAGSWEEP (`verify/c4rflagsweep_20261107/RESULT_C4R-C2-FLAGSWEEP.md`)

746 C2/C3 rows below the match gate in 736 `re/match` sources, recompiled under 74 flag sets each (O1/O2 x Ob0/1/2
x default/G5/G6 x Oy- x GS, plus recorded and recorded + GF), 54,084 compiles, 0 compile errors.

- Control: all 746 rows reproduce their ledger score under the recorded flags.
- Rows reaching byte-identical + relocheck OK under some other flag set: **0**.
- 73 rows score higher under some grid set, by 4.6 points on average (largest 0x004dfb10, 20.0% -> 51.7%). None
  crosses 95%.
- By 64 KB range the best set is `/O2 /Ob1 /GS` in 10 of the 12 ranges below 0x004c0000 (307 rows; the other two
  tie it or hold 6 rows) and `/O2 /Ob1 /G5` in all 11 ranges above it (439 rows), and the recorded `// FLAGS:` lines already follow that split. No range shows a consistent other set,
  so no FLAGS line changed and no row dropped.

## B. C4R-DSHOW-BASECLASSES (`verify/c4rdshowbase_20261107/RESULT_C4R-DSHOW-BASECLASSES.md`)

DirectX 9.0b SDK BaseClasses source (29 files) built with the XP DDK compiler under 8 configs.

- **Recorded build: `/Gz /MT /GX /O2 /Ob1 -D NDEBUG WIN32 _MBCS _LIB _WIN32_DCOM WINVER=0x400`** (the SDK's
  `baseclasses.dsp` Release line plus `/Ob1`): 159 of 159 rows identical, 19 of them ambiguous. Next best
  `/Ob1` UNICODE 149, then the plain dsp line 141. The DDK `strmbase.lib` matched 14.
- Promoted through `c4_reclass.py batch` route libid: **126 pass** (122 C2 + 4 C3), 19 refused (identical-ambiguous:
  COM thunks, scalar deleting destructors, empty destructors), 14 already C4. `LIB_FLAGS` gains
  `directshow-baseclasses-sum2003`.
- Three empty stub headers (`cderr.h`, `lzexpand.h`, `winperf.h`) stand in for files the DDK SDKINCS cabs lack.

## Gates

| | before | after |
|---|---|---|
| tracker_lint (`--base orch/integration`) | pre-existing=3 NEW=0 | pre-existing=3 **NEW=0** |
| ladder overall | C1 10, C2 2306, C3 221, **C4 3437**, other 32 | C1 10, C2 2184, C3 217, **C4 3563**, other 32 |
| ladder game | C2 719, C3 103, C4 2556 | unchanged |
| ladder library | C1 10, C2 1587, C3 118, C4 881 | C1 10, C2 1465, C3 114, C4 1007 |

`lint_before.txt`, `lint_after.txt`, `ladder_before.json`, `ladder_after.json` beside this file.

## What else would move C2 rows (ranked by rows reachable)

Counts are from hooks.csv after this branch.

1. **83 new BaseClasses rows by libid.** `rel_ob1.tsv` lists 96 VAs outside the BaseClasses rows where a library
   function is identical. 94 have no hooks.csv row, 83 with a single candidate. Route: add them as C1 by identity,
   then one libid batch on `rel_ob1.tsv`. These are new C4 rows, not C2 moves.
2. **RenderWare: 538 C2/C3 `[renderware]` + 230 `[RenderWare-Physics-3.7]`.** The largest library pool. No
   RenderWare 3.x library object or source is on disk under `tools/`, so libid cannot run. Needs the RW 3.7 SDK
   static libs (a user decision: acquisition).
3. **d3dx9: 178 C2** (`d3dx9-shader-compiler` 80, `d3dx9-backward-extension` 51, `d3dx9-psgp` 45, `d3dx9-fx` 2).
   Being worked on `orch/C4R-DXLIB-FU`, not touched here.
4. **Game match rows at >= 95%: 108** (54 with 8 or fewer differing bytes). The flag sweep shows the residue is in
   the source, not the flags: hand-tuning per function is the only route, about one function per slice.
5. **Library residue that every tried variant leaves: Lua 207, qhull 191, CRT 167** (C4R-LIBID section 4: 444
   differs + 97 not compared across these). The BaseClasses result is a hint for the CRT tail only: a library rebuilt
   from source with the game's own `/Ob1` matched where the vendor's prebuilt archive did not. Lua and qhull are
   already built from source under `/Ob1` variants without gain. The CRT has no source build yet: the VC7 CRT source
   (`crt\src`) is not on disk.
6. **`[msvc-seh-funclets]` 43 C2.** Compiler-generated, no library function exists. They can only move with their
   parent function's match.
7. **Game rows without a match source: 78**, and 396 below 80%: match work, not a cheap route.

## Files

`verify/c4rflagsweep_20261107/` (A), `verify/c4rdshowbase_20261107/` (B), this file and the gate files.
