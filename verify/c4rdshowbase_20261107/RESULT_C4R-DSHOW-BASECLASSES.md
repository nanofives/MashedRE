# C4R-DSHOW-BASECLASSES: DirectShow BaseClasses rows by library identity against the DX 9.0b SDK source (2026-11-07)

**Verdict: GREEN.** 126 rows promoted to C4 by route `libid` (L-93) through `re/tools/c4_reclass.py batch`
(122 C2 + 4 C3). 19 rows refused as ambiguous, 14 rows were already C4. Text and tracker only: no mashed_re build,
no game run.

## Build

Source: `tools/dxsdk9/sum2003/Samples/C++/DirectShow/BaseClasses/*.cpp` (29 files, DirectX 9.0b SDK Summer 2003,
main checkout, gitignored, L-94). Compiler: XP DDK `cl` 13.00.9176 / `c2` 13.00.9178 (`tools/vc7ddk`).
Headers in order: the BaseClasses dir, `stubinc/`, `tools/dxsdk9/sum2003/Include`, `tools/vc7ddk/inc/wxp`,
`tools/vc7ddk/inc/crt`.

`stubinc/` holds three empty headers: `cderr.h`, `lzexpand.h`, `winperf.h`. The DDK `windows.h` includes them
unconditionally, and the DDK SDKINCS cabs do not carry them (only `ME_ENV` does, per `tools/vc7ddk/_cab/ME_ENV.INF`).
BaseClasses uses none of their declarations: all 29 files compile with them empty.

Base config = the SDK's own `baseclasses.dsp` "Win32 Release" line less the PCH/IDE switches (`/Yu`, `/FD`, `/Gi`,
`/W3`): `/Gz /MT /GX /O2 -D NDEBUG WIN32 _MBCS _LIB _WIN32_DCOM WINVER=0x400`.

Driver: `MSYS_NO_PATHCONV=1 py -3.12 verify/c4rdshowbase_20261107/dshowbase.py all` (same libmatch machinery as
`verify/c4rdxlib_20261107/dxlib.py` and `verify/c4rlibid_20261107/reverify.py`: `libmatch.run`, unique identity
only, relocation sites masked, intra-object relocations checked).

| config | flags / defines | identical | of which ambiguous | differs | not compared |
|---|---|---:|---:|---:|---:|
| rel | `/Gz /MT /GX /O2`, MBCS | 141 | 21 | 14 | 4 |
| rel_uni | `/Gz /MT /GX /O2`, UNICODE | 131 | 26 | 19 | 9 |
| rel_o1 | `/Gz /MT /GX /O1`, MBCS | 24 | 7 | 97 | 38 |
| **rel_ob1** | **`/Gz /MT /GX /O2 /Ob1`, MBCS** | **159** | **19** | **0** | **0** |
| rel_g5 | `/Gz /MT /GX /O2 /G5`, MBCS | 68 | 20 | 73 | 18 |
| rel_g6 | `/Gz /MT /GX /O2 /G6`, MBCS | 141 | 21 | 14 | 4 |
| rel_uni_ob1 | `/Gz /MT /GX /O2 /Ob1`, UNICODE | 149 | 25 | 5 | 5 |
| rel_nogz | `/MT /GX /O2`, MBCS | 138 | 21 | 15 | 6 |
| (before) XP DDK SP1 `strmbase.lib` | prebuilt | 14 | 10 | 90 | 45 |

Rows: 159 `third-party-library[directshow-baseclasses]` (138 C2, 7 C3, 14 C4).

**Recorded build: `rel_ob1`**, 159 of 159 identical, no row differs. `/Ob1` is also the inline setting of the game's
own objects (`/O2 /Ob1` family in `re/analysis/match_ledger.tsv`). `rel_g6` gives every row the same status and
identity as `rel` (only the nearest-function pairing of one differing row, 0x004a2620, changes), so the 9176 default
code generation matches `/G6` on this library. Every row identical under any other config is also identical under
`rel_ob1` (rows identical under `rel_ob1` but not under: rel 18, rel_uni 28, rel_g6 18, rel_nogz 21, rel_uni_ob1 10).
`re/tools/c4_reclass.py` `LIB_FLAGS` gains `directshow-baseclasses-sum2003` with that build, so each promoted row's
note names it.

The 14 rows C4R-DXLIB promoted from the DDK `strmbase.lib` are identical under `rel_ob1` too.

## Promotion

```
py -3.12 re/tools/c4_reclass.py batch --tsv verify/c4rdshowbase_20261107/batch.tsv --report verify/c4rdshowbase_20261107/dryrun_verdicts.tsv --date 2026-11-07
py -3.12 re/tools/c4_reclass.py batch --tsv verify/c4rdshowbase_20261107/batch.tsv --apply --result verify/c4rdshowbase_20261107/RESULT_C4R-DSHOW-BASECLASSES.md --date 2026-11-07 --report verify/c4rdshowbase_20261107/verdicts.tsv
```

`batch.tsv` = all 159 rows, route `libid`, evidence `verify/c4rdshowbase_20261107/rel_ob1.tsv`.
Result: **126 pass, 19 refused, 14 skipped (already C4)**. hooks.csv is edited line by line by `c4_reclass.py`
(no csv round-trip), only on `directshow-baseclasses` rows.

Refused, `identical-ambiguous` (the same bytes are several BaseClasses functions: COM `QueryInterface`/`AddRef`/
`Release` thunks, scalar deleting destructors, empty destructors, one-field getters), 16 C2 + 3 C3:
0x0049c690 0x0049de60 0x0049de70 0x0049de80 0x0049eab0 0x0049ead0 0x0049ec80 0x0049eeb0 0x0049f090 0x0049fa40
0x004a0550 0x004a06c0 0x004a1790 0x004a23e0 0x004a2500 0x004a25f0 0x004a2680 0x004a26e0 0x004a2b50.
These cannot move by libid. Their names in hooks.csv can still be read from the candidate list in `rel_ob1.tsv`.

Seven of the promoted or already-C4 identities are bodies under 16 bytes (`??1CAutoLock`, `??0CMediaType`,
`?SignalTimerFired@CBaseRenderer`, `?CompatibleTimeSetEvent`, plus three that were C4). They are unique among the
1569 functions of the build, which is the libmatch rule.

## Outside the rows

`libmatch` search found 179 (va, library function) pairs identical at addresses that are not BaseClasses rows:
96 distinct VAs. 94 have no hooks.csv row, 83 of them with a single candidate. The other 2 VAs (0x004943d0,
0x00494c60) are `render` C4 rows whose bytes equal 23 BaseClasses scalar deleting destructors, a generic body the
same compiler emits for game classes too, so they are not library evidence. The 83 single-candidate VAs are
candidate new BaseClasses rows (C1 by identity first, then a libid batch). Not done here.

## Gates

tracker_lint before: `pre-existing=3 NEW=0`. After: see `lint_after.txt`.
Ladder before: `ladder_before.json`. After: `ladder_after.json`.

Files: `dshowbase.py` (driver), `stubinc/`, `rel*.tsv` (one per config), `dshowbase_log_grid.txt`, `batch.tsv`,
`dryrun_output.txt`, `dryrun_verdicts.tsv`, `apply_output.txt`, `verdicts.tsv`, `lint_before.txt`,
`lint_after.txt`, `ladder_before.json`, `ladder_after.json`.
