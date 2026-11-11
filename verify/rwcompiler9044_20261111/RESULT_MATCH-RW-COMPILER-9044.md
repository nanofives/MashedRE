# RESULT MATCH-RW-COMPILER-9044 (2026-11-11)

Row: L-131 (1), Mariano 2026-11-11. Find the build-9044 compiler (Rich prodid 0x30, 349 objects in MASHED.exe), add it
to `re/tools/match/match.py` as a second RenderWare toolchain, re-score the RW residuals, promote what becomes
byte-identical with relocheck OK. Branch `orch/MATCH-RW-COMPILER-9044` off `orch/integration` df08d4cea.

## Verdict

**Found.** Build 9044 is not a pre-release 13.00 front end. It is the **Visual C++ 6.0 Processor Pack** for SP5
(`vcpp5.exe`): its only compiler file is `c2.dll` 13.00.9044.0, installed over VC6 SP5 (`cl.exe` 12.00.8804,
`c1.dll` 12.00.8867, `c1xx.dll` 12.00.8964). Rich prodid 0x30 is `Utc12_2_C`, and the richprint table
(`dishather/richprint` `comp_id.txt`) lists `00302354 [ C ] VS6 SP5 PP build 9044`. This explains RT2D-FONT-2's finding:
a VC6 front end and epilogue shape (one epilogue per return) with a VC7-era back end (IEEE `test ah,0x44; jp` compares).

New toolchain `// TOOLCHAIN rw9044` (`tools/vc6pp/bin`, gitignored like every `tools/` dir). **57 rows promoted to C4
by match** (`c4_reclass.py batch --apply`, byte-identical + relocheck OK, note names the compiler). A further **69
rows become byte-identical under rw9044 but are held**: they are Lua 4.0, libpng/zlib and RenderWare Audio, and L-131
covers RenderWare rows. That scope needs a decision from the lead (below).

## Provenance (unmodified Microsoft files only)

| file | source | size | SHA-256 |
|---|---|---|---|
| `vcpp5.exe` (Processor Pack, Microsoft WEXTRACT self-extractor 4.71.1015.0, "Microsoft Corporation") | `http://download.microsoft.com/download/vb60ent/Update/6/W9X2KXP/EN-US/vcpp5.exe` via Wayback `http://web.archive.org/web/20030202103924id_/...` | 1,180,896 | `96817df1bb0cb58e5b1d0fbcc5b993a63548182add1fbff66365401203ecfb6c` |
| `vs6sp5.exe` (VS6 SP5, used only to verify the front end) | `http://download.microsoft.com/download/vstudio60ent/SP5/Wideband-Full/WIN98Me/EN-US/vs6sp5.exe` via Wayback `http://web.archive.org/web/20060509123628id_/...` | 136,410,832 | `10a834041223ad21181c0e87c0a90930d976498556af2e8f959a044101e6a604` |

- Each download's SHA-1 equals the Wayback CDX digest of Microsoft's URL (`2YSGLQHDEFIIN4HUORJFGQBDPCWYEPKU` for vcpp5.exe,
  identical in all 20 captures 2002-03-06 .. 2006-06-15; `NLYS7M7CY7BHDPTHZHEYGPDDXAV3PON7` for vs6sp5.exe). The files
  are Microsoft's originals as served then. vcpp5.exe carries no Authenticode signature (2001 WEXTRACT packages were
  unsigned). Neither installer was run: both were unpacked with 7-Zip into `tools/_download/vcpp5/x` and
  `tools/_download/vs6sp5/{x,y}`.
- The Processor Pack copies only `c2.dll` and `ml.exe` into VC98\Bin (`vcpp.inf` CopyToVC98Bin), and its readme
  requires VC6 SP5 or later.

`tools/vc6pp/bin` (SHA-256):

| file | version | SHA-256 | origin |
|---|---|---|---|
| cl.exe | 12.00.8804 | `1bf99f206271ecdbd13da2829192ea2c02e2a44c740b8d72935d5d9cb753b156` | vs6sp5.exe `vc98/bin` (byte-identical to SimGolf `tools/vc6sp5`) |
| c1.dll | 12.00.8867 | `0792033fc3d2fec262ff95478c6a9e70a06e836ed55f0a9710200fa06c204754` | vs6sp5.exe `vc98/bin` |
| c1xx.dll | 12.00.8964 | `f014b3bee650224adf6cb44e51f0eeac5abbf5da666fa299d5250b2f3d1937c6` | vs6sp5.exe `vc98/bin` |
| c2.dll | 13.00.9044 | `6c8e3988a5ccd69a9f9829b5c7512848d71c018b8d24be3928d7b00b5f79f231` | vcpp5.exe |
| mspdb60.dll, msobj10.dll | (PDB/obj support, not code generation) | `3355b03f...`, `0ec574f8...` | SimGolf `tools/vc6sp5/Common/MSDev98/Bin` |

The SimGolf `vc6sp5` folder is a GitHub repack (`ifarbod/msvc600_sp5`), so the three front-end files were checked
byte for byte against Microsoft's SP5 package before use. The two support DLLs come from that repack and are not
verified. `mspdb60`/`msobj10` write debug info, not code. Rebuild recipe: copy the three `vc98/bin` files out of
vs6sp5.exe (cab set `VS6sp51.cab`..), `c2.dll` out of vcpp5.exe, and the two DLLs from any VC6 install.

Searched first, nothing at 9044 locally: `tools/_download` (dxsdk_sum2003/2004 ship no cl.exe; xpddk2600 and
xpddk2600sp1 `x86dbins` both hold cl/c1/c1xx 13.00.9176 + c2 13.00.9178), `tools/vc7ddk` (same), SimGolf `vc6`
(12.00.8168), `vc6sp2`, `vc6sp5` (c2 12.00.8966), a `find` for c2.dll over C: and D: (only VS2022's 14.44 copies).
Web: the `13.00.9044` / `12.00.9044` searches returned nothing. The Rich table lookup gave the identity, and the
Wayback CDX search for `vcpp5.exe` gave the file.

## Tool changes

- `re/tools/match/match.py`: `TOOLCHAINS["rw9044"]` (bin `MAIN/tools/vc6pp/bin`, no extra flags), docstring.
- `re/tools/c4_reclass.py`: `RECORDED = {ddk, rw9044}`. The gate accepts either one, and the C4 note names the
  compiler that matched (`VC6 SP5 cl 12.00.8804 + Processor Pack c2 13.00.9044 (L-131)`).
- `re/CONFIDENCE.md` match route: RenderWare rows may record the second compiler (`// TOOLCHAIN rw9044`).
- `ddkobj.py` already honours `// TOOLCHAIN`, so the object build compiles a retagged unit with rw9044 (see port note).

## Sweep: every re/match source under both compilers

`sweep.py` runs `match.py --bytes` under ddk and rw9044 for every source (963 RW-slice rows in rwd01/rwl01-06/s20/s24,
3,407 rows in the other dirs). Rows that became byte-identical only under rw9044: `sweep_gained.tsv` (128). Full
tables (raw, not committed): `D:/MashedWT/MATCH-RW-COMPILER-9044_data/` (SHA-256 below).

- RW slice: DDK 576 byte-identical, rw9044 602. Gained 41 sources (40 RVAs), lost 15. The 15 stay `ddk` (untagged:
  0x004c1210, 0x004ccba0, 0x004ccf20, 0x004db2e0, 0x004dc9e0, 0x004e1ac0, 0x004e1b00, 0x004e1b30, 0x004e1c90,
  0x004f4200, 0x00530010, 0x00540340, 0x00550520, 0x00550750, 0x005748a0). So **RenderWare in MASHED.exe is built by
  both compilers**, per object (Rich 0x1c/0x1d build 9178 x57 plus 0x30 build 9044 x349). Each source carries its
  own `// TOOLCHAIN` line, the default stays `ddk`.
- Other dirs: 87 gained (RW core in s19/s21/s22, RW Audio in s25-s28/tlr, Lua 4.0 in s18/s19, libpng/zlib in s23).
- No source file had a function that the retag would lose, and none scored lower under rw9044 (the files were checked
  for conflicts before tagging).

## Promoted to C4 by match (57)

All via `c4_reclass.py batch --apply` (reports `c4_apply.tsv`, `c4_apply_core.tsv`, `c4_apply_8480.tsv`).

- RW slice (39): 0x004c9cd0, 0x004cb490, 0x004cb8a0, 0x004cba80, 0x004cbb70, 0x004cd550, 0x004cd900, 0x004ce480,
  0x004cf4a0, 0x004e1d20, 0x004e5bf0, 0x004e5ef0, 0x004e6470, 0x004e6650, 0x004e6710, 0x004e6760, 0x004e6920,
  0x004e7cc0, 0x004e7e50, 0x004e98d0, 0x004e9910, 0x004ebb00, 0x004f2e70, 0x00533d00, 0x00538600, 0x0053eaa0,
  0x0053fd30, 0x00541b50, 0x00544d20, 0x00546b10, 0x00549900, 0x0054b6d0, 0x00550a20, 0x00551190, 0x00551a50,
  0x00551c40, 0x005584c0, 0x0055ae50, 0x00563df0.
- RW core in the general dirs (18): 0x004c2f60, 0x004c2fb0, 0x004c5930, 0x004c5cb0, 0x004d4250, 0x004d42b0,
  0x004d4310, 0x004d6910, 0x004d7480, 0x004d80d0, 0x004d8480, 0x004dfab0, 0x004dfb10, 0x004dfb60, 0x004e1df0,
  0x004f10e0, 0x004f4270, 0x004f8580.
- Two sources needed a fix before relocheck passed. Under rw9044 the two global loads come out in the other order, and
  relocheck caught the swap: 0x00549900 (`DAT_007d3ff8 + DAT_007dc720`, original loads 0x7d3ff8 first at 0x00549903)
  and 0x004d8480 (`DAT_007d3ff8 + DAT_007d6c5c`, original loads 0x7d6c5c into ecx first at 0x004d8480). Both are
  operand-order edits with the same value.
- Refused: 0x00550c50 (byte-identical, relocheck not reached, `no hooks.csv row`). It needs a C1 row first.

## Held for a decision (69 byte-identical under rw9044, not promoted)

`rw9044_hold.tsv`, sources not retagged:
- **libpng / zlib** (45, s23, 0x00516c40..0x0052df40). These are L-67/L-70 library rows, which may carry C4 by libid. Here
  the reconstructed sources match under rw9044, so the libpng/zlib objects are build 9044 too.
- **RenderWare Audio** (20, s25-s28 + tlr, 0x005a62b0..0x005c7500: RwaDSRenderer, RWS audio path, codec/mixer), plus
  0x005c4e20 (hooks.csv notes "MIXED" CRT/audio cluster).
- **Lua 4.0** (2: 0x004b71c0 `lua_pushstring`, 0x004c18c0).

Recommendation: extend L-131 to "the Rich prodid 0x30 objects" (RW, RW Audio, Lua, libpng/zlib) rather than "RenderWare
rows". Each of the 69 is the same evidence as the 57: byte-identical, relocheck to run. The follow-up is mechanical:
add `// TOOLCHAIN rw9044` to the listed sources, then `c4_reclass.py batch` on them.

## The named residuals

| RVA | before (ddk) | rw9044 | now | note |
|---|---|---|---|---|
| 0x00554390 Rt2d font read | 62.6% | 82.0% (RT2D source) | **86.1%** | see below |
| 0x00554200 Rt2d path read | 69.5% | 69.5% | 69.5% | same under both: not a compiler effect |
| 0x00556e90 Rt2d brush RGBA | 99.0% | 99.0% | 99.0% | same pair of late spill loads under both |
| 0x004d1640 | 99.8% | 99.8% | | unchanged |
| 0x004d1e30 | 99.1% | 99.1% | | unchanged |
| 0x004d2980 | 97.1% | 97.1% | | unchanged |
| 0x004d7480 | 96.2% | **100%** | C4 | promoted |
| 0x004db3e0 | 95.2% | 95.2% | | unchanged |
| 0x004c9eb0 | 94.8% | 94.8% | | unchanged |
| 0x004c7a70 | 93.2% | 93.1% | | unchanged |
| 0x00555360 (Rt2d, not named) | 94.4% | 98.9% | | worth a pass under rw9044 |

**0x00554390.** rw9044 removes RT2D-FONT-2's blocker: every failure returns through its own epilogue, as in the
original, and the frame is 0x30. Shaping: the outline-glyph reads use two block-scoped temporaries, `t1`
(type/code/end) and `t2` (the read after `FUN_00554200`). The compiler places them in the dead parameter homes
`[esp+0x44]` and `[esp+0x48]`, as the original does at 0x005545c0..0x00554639. Residual (61 instructions): the
original zeroes ebp before the `isOutline` test (`xor ebp,ebp; cmp [ebx],ebp`) and keeps `i` at `[esp+0x10]` in both
loops, plus a separate outline fail tail (`push ebx; call 0x555830; xor ebx,ebx; mov eax,ebx`). match_permute reported
87.0%, but its rewrite was rejected. It had moved `font = 0` ahead of `FUN_00555830(font)` and moved the loop
increments, which changes behaviour. Permuter output must be read before it is kept. **The source change is built on
the RT2D-FONT-2 version** (f97f6798f, 16 lines on top of it, 369 against integration): merge orch/MATCH-RT2D-FONT-2
first. It is committed alone so it can be dropped or cherry-picked. Sonnet-medium shaping stopped here, so 0x00554390
and 0x00554200 go back to the lead (Opus high per the brief), now under rw9044.

## Port-axis note (not run here)

Three retagged sources are #included by the exe (`mashedmod/src/mashed_re/LibRw/RwApiBind.cpp`): 0x005584c0,
0x004c5930, 0x004c5cb0. In the object build (`MASHED_DDKOBJ=1`) `ddkobj.py` will now compile them with rw9044 (VC6
objects carry `/DEFAULTLIB:LIBC` as DDK ones do). Before this row they were not 100%, so they were not substituted.
No build or gate ran on this branch (match-only row). The next object build should run `ddkobj.py check` on them.

## Checks run

- `test_relocheck.py` 4/4 PASS, `test_equiv.py` 44/44 PASS (after the match.py/c4_reclass edits).
- Each promoted row: `c4_reclass.py batch` recompiled it under its own `// TOOLCHAIN` and found `--bytes` identical,
  relocheck OK.

## Files

- `sweep.py` (both-compiler sweep), `sweep_gained.tsv` (128 rows), `rw9044_files.txt`, `rw9044_files_rest.txt`,
  `rw9044_hold.tsv`, `batch_c4*.tsv`, `c4_apply*.tsv`.
- Raw, not committed, `D:/MashedWT/MATCH-RW-COMPILER-9044_data/`:
  `sweep_rw.tsv` 9acaf731cd1743e60190f5e6c13811699b15ef8ea9e0b168b298fce882b79b2d,
  `sweep_rest.tsv` d011618b5c87b1db73c86d670e87a3085afaa3766a0e374f2f584d8da72d26e4,
  `sweep_changed.tsv` 901a0de0b4dbf3351aa98a0e96393760b4f7f73b9731c575a98200ccb5693b0f,
  `c4_dryrun.tsv` fd906f650c8aac4b2fed2ffc4562640cea39e5ff9182ca125c1f2edef57e725a,
  `sweep_err.txt` 054aefbfa3b5c5c2f914ae843a9a910cd16cca45fd520bfeb806b116a6c5e8c7,
  `sweep_rest_err.txt` e25e85c938faa2feac00d2471812a2f5130d1ece3097865d22eb4b6a2f04993e.
- Sources: `rw9044_files.txt` + the 18 core files in `batch_c4_core.tsv` (one `// TOOLCHAIN rw9044` line each, plus
  the two operand-order fixes), `re/match/rwl05/00554390.c`.

## Rebase onto orch/integration 8bc7026f0 (REBASE-RUN37-B, 2026-11-11)

- 9e36314ac (the compiler row, 57 C4) was already in integration. Only 4e0021b23 replayed, conflicting in
  `re/match/rwl05/00554390.c` with MATCH-RT2D-FONT-2 (integration's copy equals f97f6798f's, the base 4e0021b23 was written on).
- Scored each side under its recorded TOOLCHAIN with `match.py`: integration's copy 62.6% (tc=ddk, 353/439), the branch copy
  86.1% (tc=rw9044, 429/439). Kept the branch copy (`// TOOLCHAIN rw9044` retained), byte-identical to 4e0021b23's.
- `c4_reclass.py batch` dry run over the 58 rows of batch_c4*.tsv: 57 SKIP already C4, 1 REFUSED 0x00550c50 (no hooks.csv
  row, refused in the original run too), 0 writes (`c4_dryrun_rebase_8bc7026f0.tsv`).
- `relocheck.py` on the 57 sources plus 00554390.c: 58 lines OK (every promoted row), 1 BAD = 0x00554390 (86.1%, not promoted), as on the branch tip
  (`relocheck_rebase_8bc7026f0.tsv`).
