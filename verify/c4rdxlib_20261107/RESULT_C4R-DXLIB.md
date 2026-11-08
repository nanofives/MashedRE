# RESULT C4R-DXLIB (roadmap run 35, 2026-11-07)

**Verdict: GREEN.** 234 rows promoted to C4 by route `libid` (L-93) through `re/tools/c4_reclass.py batch`:
d3dx9-psgp 144, d3dx9-shader-compiler 69, d3dx9-backward-extension 6, one d3dx9 body filed under `render`
(0x004fb5a0), DirectShow BaseClasses 14. 323 rows refused, each with its libmatch reason. Text and tracker only: no
build of `mashed_re`, no game run, no `mashedmod/` change, no download, nothing from `tools/` committed.

Branch `orch/C4R-DXLIB` from `orch/integration` @ 3445c9f38.

## 1. Release choice: DirectX 9.0b SDK, Summer 2003

Inputs (L-94, gitignored, SHA-256 in `tools/dxsdk9/SHA256SUMS.txt`): `sum2003/Lib/d3dx9.lib` `9496d4ce...6099ed56`
(5,534,754 B, 228 i386 members, 6,025 functions), `sum2004/Lib/d3dx9.lib` `3eec8cbc...2b818a88` (5,959,648 B, 253
members, 7,776 functions). Both are plain COFF archives (no `/GL` member was skipped). libmatch (`re/tools/match/
libmatch.py`, same masking and ambiguity rules as C4R-LIBID) ran each release against all 397 hooks.csv rows whose
subsystem starts with `third-party-library[d3dx9`, anchor `MASHED.exe.unpatched`:

```
MSYS_NO_PATHCONV=1 py -3.12 verify/c4rdxlib_20261107/dxlib.py d3dx_sum2003 d3dx_sum2004 dshow_strmbase
```

| release | identical (unique) | ambiguous | differs | not compared | identical at non-row-set addresses |
|---|---|---|---|---|---|
| **9.0b Summer 2003** | **219** | 26 | 121 | 31 | 1209 entries / 176 addresses |
| 9.0c Summer 2004 | 31 | 23 | 300 | 43 | 143 |

Per subsystem (unique identical, 2003 vs 2004): psgp 144 vs 17, shader-compiler 69 vs 10, backward-extension 6 vs 4,
fx 0 vs 0. No row is identical under 2004 and not under 2003; 188 rows identical under 2003 are not under 2004. The
Summer 2003 library is the one the game links, consistent with `D3D_SDK_VERSION` 31 at 0x004c7e02 (9.0b). Log:
`dxlib_log.txt`. TSVs: `d3dx_sum2003.tsv`, `d3dx_sum2004.tsv`.

The brief scoped psgp (189) and shader-compiler (149). The batch also carried the 57 `d3dx9-backward-extension` and 2
`d3dx9-fx` rows, since they are the same library and the L-93 rule does not depend on the sub-tag. Of those, 6 passed.

## 2. Counts per subsystem (route libid)

| subsystem | rows | identical -> C4 | ambiguous | differs | no pair |
|---|---|---|---|---|---|
| `[d3dx9-psgp]` | 189 | **144** | 12 | 28 | 5 |
| `[d3dx9-shader-compiler]` | 149 | **69** | 12 | 64 | 4 |
| `[d3dx9-backward-extension]` | 57 | **6** | 2 | 29 | 20 |
| `[d3dx9-fx]` | 2 | 0 | 0 | 0 | 2 |
| `render` 0x004fb5a0 (d3dx9 body, see 4) | 1 | **1** | 0 | 0 | 0 |
| `[directshow-baseclasses]` vs XP DDK SP1 `strmbase.lib` | 159 | **14** | 10 | 90 | 45 |
| **total** | 557 | **234** | 36 | 211 | 76 |

Every row was C2 before except 7 DirectShow C3 rows, which all stay C3 (3 ambiguous, 3 differ, 1 no pair). Per-row
verdicts: `verdicts.tsv` (rva, subsystem, release, symbol, verdict, c4_reclass). `batch.tsv` (557 rows),
`dryrun_output.txt` / `dryrun_verdicts.tsv` (234 PASS, 323 REFUSED), `apply_output.txt` (234 hooks.csv lines + 234
CHANGELOG lines). A second `--apply` wrote nothing (0 pass, 234 skipped already C4). `git diff --stat hooks.csv`: 234
lines changed.

Short bodies. 21 of the 234 have 4 or fewer unmasked bytes: 19 d3dx9 dispatch stubs of 5 or 6 B (`jmp [pfn]`, e.g.
0x004ecafb `_D3DXVec2Transform@12`, 0x004ed6ba `_D3DXMatrixMultiply@12`, 0x0050f1bf
`_x3d_D3DXVec3TransformCoord@12`) and 2 DirectShow destructors (0x0049c6a0 `CCritSec::~CCritSec` 8 B, 0x004a2ae0
`CMediaType::~CMediaType` 7 B). libmatch resolved each to one candidate through its object-section base and
external-reference votes (the same resolution C4R-LIBID accepted for `CCritSec::~CCritSec`). 180 have 16 or more
unmasked bytes.

Ambiguous rows (several library functions with these bytes, refused by L-93): d3dx9 26, e.g. 0x004efe8a
`c_D3DXQuaternionNormalize` / `c_D3DXVec4Normalize`, the `sse_` / `sse2_` twin pairs (0x005046dd, 0x0050475b,
0x0050481d, 0x00505f2c, 0x00506428, 0x00506ce3, 0x00507e7b, 0x00508252, 0x00508326, 0x00508939, 0x005089fb), the
`??_G` scalar-deleting destructors 0x00510d43 .. 0x0051185d. DirectShow 10 (same as C4R-LIBID section 5). Full list:
`verdicts.tsv` rows with `ambiguous`.

## 3. DirectShow

Re-run of C4R-LIBID's measurement on the current hooks.csv: same result, 24 identical of which 10 ambiguous, so 14
unique (0x0049c6a0, 0x0049c6b0, 0x0049c750, 0x0049c770, 0x0049cc40, 0x0049ceb0, 0x0049ea70, 0x0049ebc0, 0x0049f210,
0x004a1030, 0x004a1180, 0x004a2310, 0x004a2ae0, 0x004a2b30). Those 14 are promoted. The DDK `strmbase.lib` is a
2001-2002 build; [UNCERTAIN] how close it is to the BaseClasses the game linked (DX 9.0b SDK). The 90 differs and 45
no-pair rows stay C2/C3.

Second reference not built. `tools/dxsdk9/sum2003/Samples/C++/DirectShow/BaseClasses/` ships the source plus
`baseclasses.dsp` and `baseclasses.vcproj` (sum2004: `.vcproj` only), so the SDK's own flags are on disk. The C4R-LIBID
recipe has no BaseClasses build (its `period_cc` path covers the C libraries, and DirectShow was matched only against
the prebuilt `strmbase.lib`), so per the brief this is a follow-up, not done here.

## 4. Rows found outside the d3dx9 row set

Under the 2003 release, 1209 identical entries sit at 176 addresses that are not d3dx9 rows:

- **159 addresses have no hooks.csv row.** Follow-up: add them as `third-party-library[d3dx9-*]` rows, then libid
  them with this TSV.
- **9 are `render` rows.** 8 are already C4 by route match (0x004ec839 `c_D3DXFloat16To32Array`, 0x004ec915
  `c_D3DXVec2Hermite`, 0x004ec9cd `c_D3DXVec2CatmullRom`, 0x004ecaab `c_D3DXVec2BaryCentric`, 0x004ecb97
  `c_D3DXVec3Hermite`, 0x004ecc6f `c_D3DXVec3CatmullRom`, 0x004f022d `c_D3DXMatrixShadow`, 0x004f0307
  `c_D3DXMatrixReflect`). The library identity is a second witness for them and was not written. The 9th,
  **0x004fb5a0**, was C2 and is promoted here: identical to `psgpmesh.obj` `c_D3DXPSGPUpdateSkinnedMesh` (1034 B,
  one candidate). Its row note records U-9135's 2026-09-12 disposition "a CLIENT of D3DX, not D3DX itself; library
  attribution [UNCERTAIN U-9135]". The byte identity settles it: the function is a D3DX body that calls other PSGP
  bodies. Follow-up: refile 0x004fb5a0 and the 8 match-route rows from `render` to `[d3dx9-psgp]` (they count as game
  rows in the ladder now) and note the correction on U-9135. Not done here: `c4_reclass.py` has no refile operation
  and the trackers are re-classify-only (same situation as C4R-LIBID's 0x005c2a0e).
- **8 are `[msvc-crt]` rows**, all ambiguous in d3dx9 (shared helper bytes). Nothing written.

## 5. Tool change

`re/tools/c4_reclass.py` `LIB_FLAGS` gains `d3dx9-sum2003`, `d3dx9-sum2004`, `directshow-baseclasses-ddk`, so the
hooks.csv note names the library. For the prebuilt DirectX archives the note omits the `XP DDK cl 13.00.9176` compiler
text (it is not our compile). The CRT / Lua / qhull / libpng / zlib note format is unchanged.

## 6. Ladder and lint

`py -3.12 verify/c4rmatchbatch_20261107/ladder_counts.py hooks.csv` (`ladder_before.json` / `ladder_after.json`):

| | before | after |
|---|---|---|
| overall | C1 10, C2 2540, C3 221, **C4 3203**, other 32 | C1 10, C2 2306, C3 221, **C4 3437**, other 32 |
| game | C2 720, C3 103, C4 2555 | C2 719, C3 103, C4 2556 (0x004fb5a0, still filed `render`) |
| library | C1 10, C2 1820, C3 118, C4 648 | C1 10, C2 1587, C3 118, C4 881 |

`py -3.12 re/orchestrator/tracker_lint.py --repo . --base orch/integration`: before and after **OK, pre-existing=3
NEW=0** (hooks.csv 5974 rows). `lint_before.txt`, `lint_after.txt`.

## 7. Merge note

Both this branch and any parallel C4 batch prepend below `<!-- ENTRIES -->` in `re/analysis/CHANGELOG.md`. A text
conflict at that marker means keep both blocks. hooks.csv lines touched: only the 234 promoted rows.

## Files

`dxlib.py` (driver), `dxlib_log.txt`, `d3dx_sum2003.tsv`, `d3dx_sum2004.tsv`, `dshow_strmbase.tsv`, `verdicts.tsv`,
`batch.tsv`, `dryrun_output.txt`, `dryrun_verdicts.tsv`, `apply_output.txt`, `ladder_before.json`,
`ladder_after.json`, `lint_before.txt`, `lint_after.txt`, this RESULT.
