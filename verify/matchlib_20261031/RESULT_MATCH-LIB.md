# MATCH-LIB: library identicals under libid, CRT L-67 waiver, missing library rows, libpng / zlib (2026-10-31)

**Rebuilt 2026-10-31 (run 27) on `orch/MATCH-S02-RC` @ 85958bad5** from the old `orch/MATCH-LIB` @ 28aa6a524 (renamed
`orch/MATCH-LIB-old-28aa6a524`): its two LIB-specific commits were cherry-picked without the old S02-RC commits, and L-70 (a) was added
(section "L-70 (a)" below). L-68 check: this step promotes 0 rows to C4, so the callerless ruling changes nothing here.

Tracker and text only. No `mashedmod/` change, no build, no MASHED.exe run, no Frida, no Ghidra write, no `original\` write.
Identity input: `verify/libidentity_20261030/libidentity_functions.tsv` (LIB-IDENTITY, anchor `MASHED.exe.unpatched`).
Rubric: `re/CONFIDENCE.md` "C4 routes by byte match" (libid + L-67). Tracker edits through the re-classify skill's new
helper `re/tools/libid_reclass.py` (documented in `.claude/skills/re-classify/SKILL.md`, self-test 6/6 PASS).

## Verdict

GREEN for (2), (3) and (4). (1) promoted **0** rows: no library group meets the libid C4 gate today.

- (1) libid C4: Lua 0 of 87 identical, qhull 0 of 116 identical. Lua: the port compiles no Lua 4.0 source. qhull: the
  port links the same vendored source but builds it with another compiler and other flags, so the port's qhull bytes
  are not the identical ones. Promoting qhull needs a change to `build_qhull.bat` (a .bat this worker may not edit)
  and a gate run. This is the open item for the lead (see "Needs a decision").
- (2) L-67: 450 CRT rows set `waived-platform-crt`, 108 of them refiled from other subsystems, and 34 CRT rows added.
- (3) 13 Lua / qhull rows added (the 16 TSV entries; 2 addresses carry 2 names each) and 1 Lua row refiled from `render`.
- (4) libpng 1.0.12 and zlib 1.1.3 fetched from the official project sites and measured: 42 libpng and 14 zlib addresses
  identical, 0 of the 4 `[libpng]` rows.
- (5) L-70 (a): those 42 + 14 are refiled and waived (below).

## Counts

| group | TSV | rows touched | result |
|---|---|---|---|
| Lua 4.0 identical (rows) | 87 | 0 | stays C2 / C3. Not C4: no vendored Lua in `mashedmod/`, no `lua_*` body in either target |
| qhull 2002.1 identical (rows) | 116 | 0 | stays C2. Not C4: port builds `mashedmod/deps/qhull-2002.1` with VS2022 `cl /Ox /arch:IA32 /fp:precise` (`build_qhull.bat`), not DDK 13.00.9178 `/O2 /G5` |
| CRT rows in `[msvc-crt*]` buckets | 342 | 342 waived | 186 identical + 17 ambiguous = 203 `identified, waived`; 88 differs + 51 not-compared = 139 `waived` |
| CRT functions at other-subsystem rows | 161 entries / 108 rows | 108 waived + refiled | boot 86, d3dx9-shader-compiler 8, render 6, util 5, save 1, audio 1, frontend 1 |
| CRT functions with no row | 36 entries / 34 addresses | 34 added (C1, waived) | 30 in `-main`, 4 in `-tail` |
| Lua functions with no row | 9 entries / 7 addresses | 7 added (C1) | 2 addresses ambiguous (`_math_mod` / `_math_pow`), named `FUN_<rva>` |
| Lua function at a `render` row | 1 | 1 refiled | 0x004b7020 `_lua_typename`, render -> `third-party-library[lua-4.0]`, C3 kept |
| qhull functions with no row | 6 | 6 added (C1) | all `third-party-library[qhull-2002.1]` |
| **total** | | **498 CHANGELOG lines** | 450 waived (108 refiled) + 47 added + 1 refiled; 0 promoted |

Waived total: 450 existing + 34 added = **484 CRT rows out of the P-DoD count** (311 identified, 139 not identical,
34 added identified). Confidence cells unchanged on every existing row.

### Refiles (CRT, L-67 step 3)

Bucket by address: `0x004a0000..0x004b4000` -> `[msvc-crt-main]`, `0x005c0000..0x005d0000` -> `[msvc-crt-tail]`, else
`[msvc-crt]`. The 8 `d3dx9-shader-compiler` addresses (0x004fcbca..0x00510d27) and the `audio` one (0x005a7b40) sit
outside both bands and went to `[msvc-crt]`. Every refiled address is `identical-unlisted` in the TSV (identical or
ambiguous), so none was refused.

Seven waived rows carry a port body (level kept, row now out of P-DoD, exe link unchanged per L-67):

| rva | name | level | port body |
|---|---|---|---|
| 0x004a2c48 | FUN_004a2c48 (`__ftol2`) | C3 | `Math/FPURound.cpp` (asi + exe) |
| 0x004a31b1 | CrtExitProcess_j5 | C3 | `Boot/Boot.cpp` (asi) |
| 0x004a332b | CrtExitNoReturn_j5 | C3 | `Boot/Boot.cpp` (asi) |
| 0x004a3440 | CrtStackProbe | C3 | `Boot/CrtCompilerSupport.cpp` (asi) |
| 0x004a4541 | FsopenSafe (`_fopen` / `__wfopen`) | C2 | `Save/FsOpen.cpp` (asi + exe); also the `fix_fopen` patch site |
| 0x004a4b93 | CrtFastErrorExit | C3 | `Boot/CrtCompilerSupport.cpp` (asi) |
| 0x005a7b40 | AudioCtxSwap5a7b40 | C3 | `Util/PromoLoop_sessionB.cpp` (asi) |

## Notes written

- Identical: `identified, waived: platform CRT (L-67); libid identical <fn> (libcmt.lib <member> @comp.id <prodid>/<build>); libmatch: py -3.12 re/tools/match/libmatch.py --objs tools/vc7ddk/lib/wxp/i386/libcmt.lib --subsystem third-party-library[msvc-crt (verify/libidentity_20261030/libidentity_functions.tsv) | <old notes>`.
  `@comp.id` read from each member of the DDK `libcmt.lib` (651 members carry one; 28/9178 C, 29/9178 C++, 64/9210 MASM).
- Ambiguous: `libid identical-ambiguous, candidates a / b / c` with every member and its `@comp.id`.
- Not identical: `waived: platform CRT (L-67); libid differs vs libcmt.lib <member> <fn> at +<off> (<category>)` or
  `libid not-compared`.
- Added Lua / qhull: `C1 by libid: identical <obj> <fn> (<flags>, XP DDK cl 13.00.9176 / c2 13.00.9178; <tsv>); row added MATCH-LIB; not C4: <reason>`.

## libpng 1.0.12 and zlib 1.1.3

Fetched 2026-10-31 (account3, web available, nothing invented), into the main checkout's gitignored
`tools/_download/pngzlib/` (extracted under `src/`):

| file | URL | SHA-256 | gzip stamp |
|---|---|---|---|
| zlib-1.1.3.tar.gz | https://zlib.net/fossils/zlib-1.1.3.tar.gz | `cae5847bc0e1cf113d3f70d037400da3e47c2e2b7b1c96b0b08447a5fbb906f4` | 1998-07-10 |
| libpng-1.0.12.tar.gz | https://download.sourceforge.net/libpng/libpng-1.0.12.tar.gz (libpng project's SourceForge release area) | `e2ba46d58e7a1a7034d01a27da8ac1d189ae3fb233dbf06f0cf00e4d5d33645f` | 2001-06-08 |

Method as LIB-IDENTITY: `period_cc.py` (DDK cl 13.00.9176 / c2 13.00.9178) on the library sources (zlib: the 14 library
TUs; libpng: the 15 library TUs, no `pngvcrd.c` / `pnggccrd.c` / `pngtest.c`), then `libmatch.run` with search over
`.text`. Driver: `verify/matchlib_20261031/pngzlib/pngzlib_sweep.py`. 13 flag sets:

| flags | zlib identical addrs | libpng identical addrs |
|---|---|---|
| `/O2`, `/Ox`, `/O2 /G6` | 7 | 8 |
| `/O2 /GS /Ob1` (game-code flags) | 8 | 8 |
| `/O1` | 0 | 0 |
| `/O2 /G5`, `/O2 /Ob1 /G5`, `/O2 /G4`, `/O2 /G5 /Gy`, `/Ox /G3 /Gs`, `/Ox /G5 /Gs`, `/Og /Oi /Ot /Oy /Ob1 /GF /G5`, `/Og /Oi /Ot /Oy /Ob2 /GF /G5` | **14** | **42** (43 entries) |

`/G3`-`/G5` is the switch here too, as for Lua and qhull. zlib has 95 object functions and libpng 309, so the plateau
is 15% and 14%. The `[libpng]` subsystem's 4 rows: 0 identical. Upstream `makefile.vcwin32` uses `-Ox -GA3s`.

Where the identical addresses sit in hooks.csv (`/O2 /G5`; `verify/matchlib_20261031/pngzlib/*.tsv`):

- zlib 14: `util` 11 (0x00521990 `_deflateEnd` .. 0x0052c660 `_inflate_flush`), no row 2 (`_zcalloc` copies at
  0x004d1000 and 0x004fa6f0), `third-party-library[renderware]` 1 (0x004f1f90 `_gztell`).
- libpng 42: `util` 28 (0x00516bb0 `_png_write_data` .. 0x00528ba0 `_png_read_start_row`), and **14 under
  `third-party-library[d3dx9-backward-extension]`** (0x005150e0 `_png_write_flush` .. 0x00516430 `_png_set_sBIT`). One
  address is ambiguous (0x00523110 `_png_get_int_32` / `_png_get_uint_32`).

[UNCERTAIN] why the remaining 85% differ: the defines the game's libpng / zlib were built with (pngconf.h options,
RenderWare's `rtpng` build) were not swept, and the same 9178 -> 9466 compiler question as LIB-IDENTITY applies. No
refile was made for these: neither library has a subsystem bucket or a re-classify rule yet.

## L-70 (a): libpng / zlib refiled and waived (Mariano 2026-10-31)

New helper operation `libid_reclass.py waive-lib` (self-test now 7/7 PASS), run once per library, a second run writes nothing:

| library | identical addresses | rows refiled | rows added (C1) | new subsystem |
|---|---|---|---|---|
| libpng 1.0.12 | 42 (43 TSV entries, 0x00523110 ambiguous `_png_get_int_32` / `_png_get_uint_32`) | 42 (28 from `util`, 14 from `third-party-library[d3dx9-backward-extension]`) | 0 | `third-party-library[libpng-1.0.12]` |
| zlib 1.1.3 | 14 (0x004d1000, 0x004fa6f0, 0x00528e00 are `_zcalloc` copies) | 12 | 2 (`_zcalloc` at 0x004d1000, 0x004fa6f0) | `third-party-library[zlib-1.1.3]` |

All 56 rows: status `waived-library`, out of the P-DoD count, confidence kept (libpng: 39 C2 + 3 C3 (0x00523110, 0x005173d0, 0x0051ca60);
zlib: 12 C2 + 2 C1), none C4. Notes start `identified, waived: vendored library <lib> (L-70)` and carry the libid identity
(object, function, `/O2 /G5`, compiler, TSV path) as provenance. 56 CHANGELOG lines. The 4 older
`third-party-library[libpng]` rows are not identical and were not touched. Rubric text in `re/CONFIDENCE.md` and the
re-classify skill.

## Needs a decision (lead)

1. **qhull libid C4 (116 rows).** The gate needs the port to compile `mashedmod/deps/qhull-2002.1` with DDK cl
   13.00.9176 / c2 13.00.9178 and `/Og /Oi /Ot /Oy /Ob2 /GF /G5`. Today `build_qhull.bat` uses VS2022 `/Ox /arch:IA32
   /fp:precise`. The change is a .bat edit plus a link check (VC7 objects into the VS2022 link) plus the merge-train
   gates (qhull is on the collision path, `QhullBridge.cpp`, so the gates can move). Out of this worker's permissions
   (no .bat edit, no gate run). Recommendation: a follow-up step owning the build switch and the gate run, behind a
   default-OFF knob if the gate hashes move.
2. **Lua (87 rows).** C4 by libid would need the port to link Lua 4.0 itself, an architecture choice (the port reads
   the game's Lua data with its own code today). No recommendation to do it for C4 alone.
3. ~~libpng / zlib rows~~ Answered by L-70 (a), done below.

## Files

- `hooks.csv`, `re/analysis/CHANGELOG.md` (498 lines below `<!-- ENTRIES -->`): written by `re/tools/libid_reclass.py`
  (`crt-waive`, `add-rows --library msvc-crt|lua-4.0|qhull-2002.1`, `refile-lib --library lua-4.0`; `refile-lib
  --library qhull-2002.1` changed 0 rows). A second run of each writes nothing.
- `re/tools/libid_reclass.py`, `re/tools/tests/test_libid_reclass.py` (6 PASS), `.claude/skills/re-classify/SKILL.md`
  (helper documented under "CRT rows (L-67)" and the new "Library rows from libid").
- `verify/matchlib_20261031/pngzlib/`: `zlib_O2_G5.tsv`, `libpng_O2_G5.tsv`, `pngzlib_sweep.py`.

Reproduce:

```
L=<main>/tools/vc7ddk/lib/wxp/i386/libcmt.lib; T=verify/libidentity_20261030/libidentity_functions.tsv; E=verify/matchlib_20261031/RESULT_MATCH-LIB.md
py -3.12 re/tools/libid_reclass.py crt-waive --tsv $T --evidence $E --lib $L
py -3.12 re/tools/libid_reclass.py add-rows --library msvc-crt --tsv $T --evidence $E --lib $L
py -3.12 re/tools/libid_reclass.py add-rows --library lua-4.0 --tsv $T --evidence $E --not-c4 "..."
py -3.12 re/tools/libid_reclass.py add-rows --library qhull-2002.1 --tsv $T --evidence $E --not-c4 "..."
py -3.12 re/tools/libid_reclass.py refile-lib --library lua-4.0 --tsv $T --evidence $E
MSYS_NO_PATHCONV=1 py -3.12 verify/matchlib_20261031/pngzlib/pngzlib_sweep.py "/O2 /G5"
```

Gates: not run. No source in either target changed, so `mashed_re.exe` and the `.asi` are byte-for-byte what
`orch/integration` builds.
