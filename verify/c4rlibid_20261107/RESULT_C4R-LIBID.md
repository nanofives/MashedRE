# RESULT C4R-LIBID (roadmap run 35, 2026-11-07)

**Verdict: GREEN.** 587 library rows promoted to C4 by route `libid` (L-93) through the re-classify batch tool
`re/tools/c4_reclass.py`: Lua 4.0 94, qhull 2002.1 122, CRT 316, libpng 1.0.12 41, zlib 1.1.3 14. 573 library rows
refused, each with its libmatch reason. Text and tracker only: no build, no game run, no `mashedmod/` change, no
`original\` write, no download. Gates: not run (text-only).

Branch `orch/C4R-LIBID` from `orch/C4R-BASE` @ 1fb65e900.

## 1. Method: re-verification, not a TSV copy

The libid gate in `c4_reclass.py` reads a libmatch TSV and does not recompile. So the identity was re-measured
first, against the **current** `hooks.csv` (after MATCH-LIB's refiles and added rows), with the recorded flags and the
period compiler (XP DDK cl 13.00.9176 / c2 13.00.9178, `tools/vc7ddk`), anchor `MASHED.exe.unpatched` (checked by
`coffpe.Image` on load). Driver: `reverify.py` (beside this file), one TSV per configuration, same layout as
`verify/libidentity_20261030/libidentity_functions.tsv` plus a `library` column. Log: `reverify_log.txt`.

```
MSYS_NO_PATHCONV=1 py -3.12 verify/c4rlibid_20261107/reverify.py lua qhull crt_libcmt zlib libpng
py -3.12 re/tools/c4_reclass.py batch --tsv verify/c4rlibid_20261107/batch_libid.tsv --report verify/c4rlibid_20261107/dryrun_verdicts.tsv
py -3.12 re/tools/c4_reclass.py batch --tsv verify/c4rlibid_20261107/batch_libid.tsv --apply --result verify/c4rlibid_20261107/RESULT_C4R-LIBID.md
```

| config (recorded build) | rows | identical | ambiguous | differs | not compared | identical outside these rows |
|---|---|---|---|---|---|---|
| `lua` Lua 4.0 `/Og /Oi /Ot /Oy /Ob1 /GF /G5` | 302 | 95 | 1 | 193 | 14 | 0 |
| `qhull` qhull 2002.1 `/Og /Oi /Ot /Oy /Ob2 /GF /G5` | 313 | 122 | 0 | 152 | 39 | 0 |
| `crt_libcmt` DDK SP1 `libcmt.lib` | 484 | 345 | 30 | 97 | 42 | 1 (0x005c2a0e, a `boot` row) |
| `zlib` zlib 1.1.3 `/O2 /G5` | 14 | 14 | 0 | 0 | 0 | 0 |
| `libpng` libpng 1.0.12 `/O2 /G5` | 46 | 42 | 1 | 2 | 2 | 0 |

Cross-check against the old evidence: all 389 unambiguous `identical` rows of LIB-IDENTITY (Lua 87, qhull 116, CRT
186) and all 55 unambiguous libpng / zlib addresses of MATCH-LIB (41 + 14) re-verify identical and PASS. None was lost.

`batch_libid.tsv` holds every row of the five library groups (1160 rows: the 1159 library rows plus the one CRT
address found by search outside them, 0x005c2a0e, a `boot` row, see section 4), route `libid`, evidence = that library's re-verify TSV.
Dry run: 587 PASS, 573 REFUSED (`dryrun_verdicts.tsv`). `--apply` wrote 587 `hooks.csv` lines (confidence `C4`, notes
`<date> C4 by route libid (L-93): c4=libid; libmatch <tsv>: identical <obj> <fn> (<size> B; <flags>; <compiler>)`)
and 587 CHANGELOG lines (`apply_output.txt`). A second `--apply` wrote nothing (0 pass, 587 skipped already C4).
`git diff --stat`: hooks.csv 587 lines changed, CHANGELOG +587.

## 2. Counts per library, before and after

| library (hooks.csv subsystem) | rows | before | after | promoted |
|---|---|---|---|---|
| Lua 4.0 `[lua-4.0]` | 302 | C1 7, C2 293, C3 2 | C1 1, C2 207, **C4 94** | 94 |
| qhull 2002.1 `[qhull-2002.1]` + `-continuation` | 313 (233 + 80) | C1 6, C2 307 | C2 191, **C4 122** | 122 (86 + 36 continuation) |
| CRT `[msvc-crt-main]` / `-tail` / `[msvc-crt]` | 484 (392 / 81 / 11) | C1 34, C2 444, C3 6 | C1 1, C2 167, C3 1, **C4 315** | 315 (270 / 44 / 1) |
| CRT at a `boot` row (0x005c2a0e `___inittime`) | 1 | C2 | **C4** | 1 |
| libpng `[libpng-1.0.12]` + old `[libpng]` | 46 (42 + 4) | C2 43, C3 3 | C2 4, C3 1, **C4 41** | 41 |
| zlib 1.1.3 `[zlib-1.1.3]` | 14 | C1 2, C2 12 | **C4 14** | 14 |
| **total** | 1160 | 0 C4 | **587 C4** | 587 |

No library row was C4 before. Rows promoted from C1 directly (L-93: identity alone is C4): Lua 6, qhull 6, CRT 33,
zlib 2. Rows promoted from C3: Lua 2, CRT 5, libpng 2.

## 3. Refused rows, grouped by reason

| reason | Lua | qhull | CRT | libpng | zlib | total |
|---|---|---|---|---|---|---|
| libmatch `differs` (paired by mnemonic similarity, first difference in the TSV) | 193 | 152 (116 + 36 continuation) | 97 | 2 (old `[libpng]`) | 0 | 444 |
| libmatch `not-compared` (no library function paired) | 14 | 39 (31 + 8 continuation) | 42 | 2 (old `[libpng]`) | 0 | 97 |
| `identical-ambiguous` (same bytes as another library function, L-93 refuses) | 1 | 0 | 30 | 1 | 0 | 32 |
| **total** | 208 | 191 | 169 | 5 | 0 | 573 |

Ambiguous rows (name not determined): Lua 0x004c0560 `_math_mod` / `_math_pow`; libpng 0x00523110 `_png_get_int_32`
/ `_png_get_uint_32`; CRT 30, among them `_atan`/`_log`/`_log10` 0x004a35e0, the `__CI*` thunks 0x004a3620, `_ctime`
family 0x004a43b9, `_fopen`/`__wfopen` 0x004a4541, the eight `??_G` scalar-deleting-destructor copies 0x004fcbca ..
0x00510d27, `___setfflag` / `__set_inconsistency` 0x005a7b40, `__aligned_realloc`/`__spawnv`/`_strtol` 0x005c2d84.
Full list: `dryrun_verdicts.tsv` rows with `identical-ambiguous`.

The residual `differs` categories are the ones LIB-IDENTITY measured (prologue / argument-load order, register
allocation, operands, branch displacement). [UNCERTAIN] that they close under cl 13.00.9466 (the anchor's Rich header
build): needs a 9466 back end, not on this machine.

### The 213 "identical at other addresses" of RESULT_LIB-IDENTITY

They are 213 entries at 156 addresses. MATCH-LIB refiled or added a row for every one of them, so each now maps to a
`hooks.csv` RVA in a library subsystem, and each went through the batch row by row:

| library | addresses | promoted | refused (identical-ambiguous) |
|---|---|---|---|
| Lua (incl. 0x004b7020 `_lua_typename`, refiled from `render`) | 8 | 7 | 1 (0x004c0560 `_math_mod`/`_math_pow`) |
| qhull | 6 | 6 | 0 |
| CRT (197 entries) | 142 | 127 | 15 |
| **total** | 156 | 140 | 16 |

No address of the 213 is without a row any more, and none was refused for a mapping reason.

## 4. Extension to the remaining rows (flag variants, other archives)

Every variant ran against the same current rows (`reverify_log.txt`, TSVs beside this file; `union.py` gives the
diff of each variant's identical set against the recorded build's):

| library | variant | rows identical (unique) | new rows vs recorded | lost |
|---|---|---|---|---|
| Lua | Lua **4.0.1** source, same flags | 95 | +1: 0x004ba570 `_luaC_checkGC` | 0 |
| Lua | `/Og /Oi /Ot /Oy /Ob2 /GF /G5` | 81 | 0 | 13 |
| Lua | `/O2 /G5` | 76 | 0 | 18 |
| Lua | `/O2 /Ob1 /G5` | 89 | +1 at 0x004bb134 (not a row) | 5 |
| Lua | `/G6` | 38 | 0 | 56 |
| Lua | `/Op` | 74 | 0 | 20 |
| qhull | `/O2 /G5` | 122 | 0 | 0 |
| qhull | `/O2 /Ob1 /G5` | 118 | +2: 0x00584c50 `_qh_degen_redundant_facet`, 0x00584d80 `_qh_degen_redundant_neighbors` | 6 |
| qhull | `/Ox /G5` | 118 | 0 | 4 |
| qhull | `/O2 /G5 /Op` | 97 | 0 | 25 |
| CRT | DDK SP1 `libc.lib` (single-threaded) | 204 | 0 rows (+1 non-row: 0x004af2d4 `__CxxUnhandledExceptionFilter`) | 111 |
| CRT | DDK SP1 `libcmt.lib` + `libcpmt.lib` | 315 | 0 | 0 |
| CRT | DDK **RTM 2600** `libcmt.lib` (other revision) | 315 | 0, identical set equal to SP1 | 0 |
| zlib | `/Ox /G3 /Gs` | 14 | 0 | 0 |
| libpng | `/Ox /G3 /Gs` | 41 | 0 | 0 |

Rows a variant reaches, and why none of them was promoted:

- **0x004ba570 `_luaC_checkGC`** (Lua): identical only to the **4.0.1** source (`luaC_collectgarbage` extern in
  4.0.1, static in 4.0, LIB-IDENTITY finding 1). The binary's version string at 0x005d8790 is "Lua 4.0". Refused:
  the identity is to another release of the library, not to the library the game links. Stays C2.
- **0x004bb130 `FUN_004bb130` / `_luaX_invalidchar`** (Lua): under `/O2 /Ob1 /G5` the 43-byte library body matches
  the image at 0x004bb134. The row starts 4 bytes earlier with `8b 44 24 08` (`mov eax, [esp+8]`) at 0x004bb130,
  then `83 ec 08` (`sub esp, 8`) at 0x004bb134, which is where the library body starts. So the original function is
  the library body with an extra argument load in front: not identical. Refused. Stays C2.
- **0x00584c50, 0x00584d80** (qhull `merge.c`): identical only under `/Ob1`, which is not qhull's recorded `/Ob2`
  build (`/Ob1` loses 6 other rows). L-93 asks for the recorded flags. Refused, stays C2. [UNCERTAIN] whether the
  original's qhull build used per-file inlining settings: no evidence on disk separates per-file flags from a
  compiler-version effect.
- **CRT**: libc.lib, libcpmt.lib and the RTM DDK libcmt.lib add no row. The 42 not-compared + 97 differs CRT rows
  stay below C4.
- **qhull continuation rows** (`[qhull-2002.1-continuation]`, 80): 36 are whole library functions at the row
  address and were promoted with the rest, 36 differ, 8 not compared.
- **msvc-crt-tail rows** (81): 44 promoted, 4 ambiguous, 10 differs, 23 not compared. No other archive on disk
  closes them.
- **0x005c2a0e `___inittime`**: identical to libcmt `___inittime` (one candidate) at a row still filed under `boot`
  (C2). Promoted C4 by libid, which needs no subsystem. Follow-up: refile it to `[msvc-crt-tail]` and set the L-67
  waiver. Not done here: `libid_reclass.py refile` refuses on the current `hooks.csv` (over-quoted row 0041d820)
  and `c4_reclass.py` has no refile operation. Until then this one CRT row still counts in the P-DoD port count
  under `boot`.

## 5. d3dx9 and DirectShow BaseClasses on this machine (for C4R-DXLIB)

Searched, no download: `C:\Program Files (x86)`, `C:\Program Files` (depth 9), `C:\DXSDK`, `C:\WINDDK`, `C:\NTDDK`,
`D:\`, the main checkout's `tools\` and `mashedmod\deps\`, all of `Proyectos\` and `Downloads\` (depth 7), for
`d3dx9*.lib`, `d3dx9.h`, `dxsdk*`, `strmbas*.lib`, `baseclasses`, `amfilter.cpp`, `streams.h`.

- **d3dx9 static library: NOT on this machine.** No `d3dx9*.lib` and no `d3dx9.h` anywhere searched. What exists is
  older or newer: XP DDK `tools\vc7ddk\lib\wxp\i386\d3dx8.lib` / `d3dx8d.lib` / `d3dx.lib` (DirectX 8), and the
  DirectX 8.0 SDK copy at `Proyectos\SimGolf\tools\dxsdk8\lib\d3dx8.lib` (readme: "DirectX 8.0 SDK"). Neither is
  D3DX9. The 2004 DirectX 9.0b/c SDK would have to be fetched (LIB-IDENTITY section 4: Microsoft no longer serves it).
- **DirectShow BaseClasses static library: ON this machine**, two revisions:
  - `tools\vc7ddk\lib\wxp\i386\strmbase.lib` = XP DDK SP1 2600.1106 (`winxp_ddk.2600.1106.iso`, SHA-256
    `3df62741...10d33b4`), SHA-256 `2e70abe0...e6b4e8b0`; also `strmbasd.lib` (debug).
  - XP DDK RTM 2600 (`winxp_ddk.2600.iso`, SHA-256 `93de09fa...f8c067`) `strmbase.lib`, SHA-256 `a927905b...ddfae7a`,
    extracted to the session scratchpad only (not committed, not in `tools\`).
  - `C:\Program Files (x86)\Windows Kits\10\Lib\10.0.26100.0\um\x86\strmbase.lib` (Windows SDK 2026 build, not the
    period library).
- **DirectShow BaseClasses source: ON this machine**, `Proyectos\SimGolf\tools\dxsdk8\samples\multimedia\directshow\
  baseclasses\` (67 files, `amfilter.cpp` etc., DirectX 8.0 SDK, 2000). Same tree under SimGolf's per-build copies.
- **Measured, not promoted** (C4R-DXLIB owns it): libmatch of the 159 `[directshow-baseclasses]` rows against
  `strmbase.lib`: 24 identical, of which 10 ambiguous, so **14 unique** (0x0049c6a0 `CCritSec::~CCritSec`,
  0x0049c6b0 `CAutoLock::CAutoLock`, 0x0049c750 `CBaseRenderer::EndFlush`, 0x0049c770 `CBaseRenderer::Inactive`,
  0x0049cc40, 0x0049ceb0, 0x0049ea70, 0x0049ebc0, 0x0049f210, 0x004a1030 `LoadOLEAut32`, 0x004a1180, 0x004a2310,
  0x004a2ae0 `CMediaType::~CMediaType`, 0x004a2b30 `DeleteMediaType`); 90 differ, 45 not compared. SP1 and RTM give
  the same 24. Its 161 "identical elsewhere" entries are shared C++/CRT helper copies at `[msvc-crt]` rows (160) and
  one address with no row. TSVs: `dshow_strmbase.tsv`, `dshow_strmbase_rtm.tsv`. The DDK `strmbase.lib` is a 2001-2002
  build; [UNCERTAIN] how close it is to the BaseClasses the game linked in 2004 (DX 9.0b SDK, per D3D_SDK_VERSION 31
  at 0x004c7e02).

## 6. Port column and lint

- `scripts/backfill_port.py --build-dir ../C4R-PORTCOL/mashedmod/build --allow-stale`: **rows changed 0** ("hooks.csv
  already current"; port none=5125 body=587 called=253). No build was made here; the C4R-PORTCOL build (map
  2026-11-07 13:48) was used because `mashedmod/` is identical between `orch/C4R-PORTCOL` and this branch
  (`git diff orch/C4R-PORTCOL -- mashedmod/` empty). `--allow-stale` only because this fresh checkout's file mtimes
  are newer than that map. Confidence is not an input of `port`, so 0 changes is the expected result. 4 pre-existing
  rows of another width were left untouched (00404830, 004464c0, 0041f360, 00420870), as on the base.
- `py -3.12 re/orchestrator/tracker_lint.py --repo . --base orch/integration`: **OK, pre-existing=3 NEW=0**
  (hooks.csv 5969 rows).

## 7. Merge note

`C4R-MATCHBATCH` promotes game-code rows in parallel through the same tool, and both prepend below `<!-- ENTRIES -->`
in `re/analysis/CHANGELOG.md`. Expect a text conflict at that marker: keep both blocks. `hooks.csv` lines do not
overlap (this branch touches only library rows plus 0x005c2a0e).

## Files

- `hooks.csv` (587 lines), `re/analysis/CHANGELOG.md` (+587 lines): written by `c4_reclass.py --apply`.
- `verify/c4rlibid_20261107/`: this RESULT, `reverify.py`, `union.py`, `reverify_log.txt`, `batch_libid.tsv`,
  `dryrun_output.txt`, `dryrun_verdicts.tsv`, `apply_output.txt`, one TSV per configuration (recorded: `lua`,
  `qhull`, `crt_libcmt`, `zlib`, `libpng`; variants: `lua401`, `lua_ob2`, `lua_o2g5`, `lua_o2ob1g5`, `lua_g6`,
  `lua_op`, `qhull_o2g5`, `qhull_ob1`, `qhull_ox`, `qhull_op`, `crt_libc`, `crt_cpmt`, `crt_rtm`, `zlib_ox`,
  `libpng_ox`; DirectShow: `dshow_strmbase`, `dshow_strmbase_rtm`).
- Raw inputs not committed (SHA-256): DDK SP1 `libcmt.lib` `bb89e45f...63875e560`, `libc.lib` `b1f5cf7c...8720f63a`,
  `libcpmt.lib` `578552f5...6aa747` (same in RTM); RTM 2600 `libcmt.lib` `de3f5112...7603e8` (scratchpad extract of
  `COMMON\X86DLIB2.CAB` file `X86dLIB2_FILE_50`), RTM `strmbase.lib` (`X86DLIB1.CAB` `X86dLIB1_FILE_118`). The RTM
  configs need `DDK_RTM_LIBCMT` / `DDK_RTM_STRMBASE` pointing at those extracts to reproduce.
