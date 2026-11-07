# RESULT MATCH-S23, 2026-11-07

**Verdict: GREEN (match-only, L-93).** Slice 0x00516bf0..0x00534920: **118 functions** (`slice_rvas.txt`,
`tools/mkslice.py`). **57/118 byte-identical** under `re/tools/match/match.py` (XP DDK cl 13.00.9176 / c2 13.00.9178),
and all 57 have `relocheck.py` OK (`relocheck.tsv`): 54 with `unchecked=0`, 3 with one CRT name unchecked
(0x00517250 and 0x005172b0 `_longjmp`, 0x00528e20 `__free`). No file uses `__asm`. All 57 pass the C++ check
(VS2022 /TP, MATCH_PORT). 55 match by instruction, 2 by whole-symbol bytes (0x00517250, 0x005172b0: the Ghidra size
stops 1 byte short). Flags: 54 `/O2 /Ob1 /G5`, 3 `/O2 /GS /Ob1` (0x005171a0, 0x005209d0, 0x00528e20, raw-converter
files where that set won or tied).

Scope per L-93: sources under `re/match/s23/`, `results.tsv`, `relocheck.tsv`, this file. **No body swap into
`mashedmod/src`, no tracker edits** (hooks.csv, CHANGELOG, UNCERTAINTIES, STUBS untouched). C4R-MATCHBATCH-2 promotes
the 57 identical rows. Gates: not run (match-only, the merge train runs them).

## Slice count: 118, not 157

`verify/matchscope_20261031/slices.tsv` gives 157 game functions. 39 of those were refiled after the cut by MATCH-LIB
L-70 (a) (commit 78f19019d): 28 to `third-party-library[libpng-1.0.12]` and 11 to `third-party-library[zlib-1.1.3]`,
all `waived-library` because they are byte-identical to the official sources. The remaining 118 rows are the slice
(same rule as `cut_slices.py`: no `third-party*` subsystem, no LIB-IDENTITY identical address). 0x00516bb0, the
slice's first RVA in slices.tsv, is one of the 39.

## Finding: the slice is libpng 1.0.12 + zlib 1.1.3, not game code

All 118 rows have subsystem `util`, but 116 carry hooks.csv notes `library_residue libpng` / `zlib` / `image-library`.
The image holds `1.0.12` (file offsets 0x1e31ff, 0x220e1c), `deflate 1.1.3 Copyright 1995-1998 Jean-loup Gailly`
(0x1e33ae) and `inflate 1.1.3 Copyright 1995-1998 Mark Adler` (0x1e42f0). Contents: libpng 1.0.12 (png.c, pngerror.c,
pngread.c, pngrtran.c, pngrutil.c, pngwutil.c, pngwtran.c, pngwrite.c functions), zlib 1.1.3 (deflate.c, crc32.c,
inflate.c, trees.c, infblock.c, infcodes.c, inftrees.c, inffast.c), an image/BMP loader with no library source
0x0052cb00..0x0052df40 (8 rows), and 0x00534920, a plugin registrar calling `FUN_004c2d90`.

Method change from S20: `libpair.tsv` pairs every slice RVA with its official library function (libmatch mnemonic
pairing of the official sources compiled `/O2 /G5`, `tools/libsweep.py`; a hint: at least 2 pairs were wrong (0x0051d0f0 is png_write_IDAT, 0x0051d1b0 png_write_IEND) and the helpers
corrected them in their notes). The helpers wrote each function from the official source text with the game's struct
offsets taken from the decomp. The game's `png_struct` is not the official layout (`png_info_destroy` 0x005171a0:
official `[esi+0x220]`, original `[esi+0x1d0]`); `deflate_state` offsets matched where checked (`_tr_init` 0x00528e30).

Library identity (compile the official files as-is) does not lift this slice: `libsweep.py` on the official sources
gives 5 of the 118 identical at `/O2 /G5`, `/O2 /Ob2 /G5` and `/Ox /Ob2 /G5`, 2 at `/O2 /Ob2 /G6`, 0 at
`/O2 /Ob2 /G5 /Oy-`. Whole-program optimization was tested too (`tools/ltcg.py`, DDK link 7.00, one DLL of both
libraries): 3 exact without /GL, 0 with `/GL` + `/LTCG`. The original inlines static helpers that the official files
define later in the same file (`init_block` into `_tr_init`, `png_crc_read/finish/error` into every `png_handle_*`, the
zlib reset functions into the init functions). Neither /Ob2 nor /GL makes this c2 do that, so every match writes those
bodies inline by hand.

## Static leg

| | count |
|---|---|
| slice functions | 118 |
| raw converter (g2s, best of `/O2 /GS /Ob1` and `/O2 /Ob1 /G5`) at 100% | 10 (all relocheck OK as converted; 5 needed C++ casts after) |
| hand-matched to 100%, round 1 (helpers h1-h5) | 44 |
| hand-matched to 100%, round 2 (h6, h7 on 35 near-misses) | 3 (0x00527c30, 0x00517940, 0x00520ef0) |
| **identical** | **57** |
| below 100% | 61: 8 at 95-99.9, 13 at 90-95, 19 at 80-90, 21 under 80 (table below; budget spent) |

Raw converter flag split: `/O2 /Ob1 /G5` scored higher on 63, `/O2 /GS /Ob1` on 16, equal on 39. 29 did not compile.

Source forms that moved functions to 100% (top lines of `helper_notes_h1.md`..`h7.md`):
- **Argument copies** (`T x = x_arg;` at entry): the original keeps spilled locals in dead argument slots, and copying
  the parameter frees its slot. 12 of h2's 17 matches needed it, 6 of h1's, and 0x00528630 went 72.4 -> 100 only with
  all four arguments copied. `D:/MashedWT/MATCH-S23_data/scratch/h2/argcopy.py` sweeps the subsets.
- **Inlined helpers written as their own blocks** (one `buf[4]` per inlined chunk helper), or `static __forceinline`.
- **Constant on the left of a mask compare**: `4 != (x & 7)` keeps `and ecx, 7` where `(x & 7) != 4` gives `and cl, 7`
  (0x00517940).

## Residual families (why 61 stay below 100)

Five systematic differences account for most of the residue. Each was tried with many source forms and with the flag
sets `/G3`-`/G7`, `/Ox`, `/Os`, `/O1`, `/Oy-`, `/Ob2`, `/Gy`, `/Op`, `/GS`. None moved it:

| family | functions | what the original does that this c2 does not |
|---|---|---|
| flags-load hoist in inlined `png_crc_error` | 15: 0x005232c0, 0x00523170, 0x00523cd0 and 12 `png_handle_*` that differ only by it after round 2 | `mov eax,[reg+0x6c]` scheduled between the `chunk_name[0] & 0x20` test and its branch. 14 source forms tried (h6) |
| partial shrink-wrap of callee-saved pushes | 9: 0x00516de0, 0x0051bb40, 0x00528810, 0x0051f4f0, 0x00519820, 0x0051b750, 0x0051cb40, 0x0052aa40, 0x0052c4a0 | pushes in the prologue with inline early exits. This c2 sinks them below the first early-out. h7: only an `{} else return X;` form stops it, for value-returning functions, and it moves the return block |
| `strlen` as `repne scasb` | 6: 0x005278a0, 0x0051e990, 0x0051f0e0, 0x0051e7f0, 0x0051ee70, 0x0051ead0 | this c2 emits a byte loop under every flag set, which also moves the register allocation |
| inline float-to-int (`fnstcw/fldcw/fistp`) | 4: 0x0051d240, 0x0051dca0, 0x005185a0, 0x0051c220 | this c2 calls `__ftol` |
| u16 zero extension `xor r16 / mov r8` | 3: 0x0051a6c0, 0x00526310, 0x00525fb0 | this c2 emits `movzx` |

[UNCERTAIN] These five point to libpng and zlib having been built with a different compiler from the one this
project has (the RenderWare core matches DDK 13.00.9176/9178 at `/O2 /Ob1 /G5`). Missing evidence: a VC7.0 retail
(13.00.9466) or VC7.1 c2 to compile one of these functions with, and the flags RenderWare or Supersonic built
libpng/zlib with. An earlier search did not find the retail 9466 compiler. Until then the rows in these families are
class (a) for this toolchain.

| rva | size | best % | regs % |
|---|---|---|---|
| 0x0051cbe0 | 922 | 98.8 | 98.8 |
| 0x00516de0 | 946 | 98.4 | 98.7 |
| 0x0051bb40 | 555 | 98.1 | 98.1 |
| 0x00527a40 | 481 | 97.8 | 97.8 |
| 0x00528810 | 912 | 97.8 | 98.1 |
| 0x0051f4f0 | 793 | 97.6 | 98.4 |
| 0x00517da0 | 83 | 97.1 | 97.1 |
| 0x00519820 | 418 | 96.3 | 96.3 |
| 0x0051b750 | 975 | 94.8 | 97.2 |
| 0x00521410 | 267 | 94.0 | 97.6 |
| 0x00526630 | 731 | 93.5 | 93.5 |
| 0x00526910 | 732 | 93.5 | 93.5 |
| 0x00525870 | 851 | 93.0 | 93.0 |
| 0x00526310 | 786 | 92.0 | 92.0 |
| 0x00523cd0 | 356 | 91.3 | 91.3 |
| 0x005245b0 | 2481 | 91.2 | 92.8 |
| 0x005253c0 | 1188 | 90.7 | 90.7 |
| 0x00523170 | 324 | 90.6 | 90.6 |
| 0x005270e0 | 1147 | 90.5 | 90.5 |
| 0x00527560 | 827 | 90.5 | 90.5 |
| 0x00523e40 | 932 | 90.0 | 90.0 |
| 0x0052aa40 | 190 | 89.3 | 89.3 |
| 0x005241f0 | 949 | 89.2 | 89.2 |
| 0x0052df40 | 45 | 88.9 | 88.9 |
| 0x00524f70 | 1098 | 88.7 | 88.7 |
| 0x005232c0 | 147 | 88.2 | 88.2 |
| 0x00525fb0 | 850 | 87.4 | 89.6 |
| 0x005238c0 | 1034 | 87.1 | 87.1 |
| 0x0052bff0 | 1198 | 86.7 | 87.3 |
| 0x0051a6c0 | 4162 | 85.9 | 86.1 |
| 0x00521520 | 1121 | 84.5 | 84.5 |
| 0x0052c4a0 | 393 | 84.1 | 87.8 |
| 0x0051fa50 | 3041 | 83.9 | 86.0 |
| 0x00521110 | 753 | 83.9 | 84.7 |
| 0x005173f0 | 437 | 83.8 | 83.8 |
| 0x00516d50 | 69 | 83.3 | 83.3 |
| 0x0052c7a0 | 849 | 83.0 | 83.0 |
| 0x0051cb40 | 46 | 82.6 | 87.0 |
| 0x00522b50 | 364 | 81.8 | 81.8 |
| 0x00527ee0 | 785 | 80.6 | 84.1 |
| 0x00516c40 | 128 | 79.5 | 79.5 |
| 0x00526bf0 | 1255 | 78.3 | 78.6 |
| 0x0051cfc0 | 303 | 76.7 | 95.1 |
| 0x00523360 | 722 | 76.5 | 77.4 |
| 0x005278a0 | 415 | 75.8 | 90.1 |
| 0x0051f0e0 | 339 | 73.9 | 76.9 |
| 0x00525bd0 | 988 | 72.6 | 77.7 |
| 0x0051e990 | 306 | 68.3 | 74.0 |
| 0x0051ee70 | 610 | 67.4 | 81.1 |
| 0x00522ce0 | 1005 | 66.6 | 77.1 |
| 0x0051ead0 | 641 | 63.8 | 74.4 |
| 0x0051d240 | 247 | 58.4 | 58.4 |
| 0x0052d170 | 955 | 57.7 | 73.0 |
| 0x0051e7f0 | 406 | 54.4 | 78.8 |
| 0x00520c70 | 629 | 52.3 | 60.5 |
| 0x0052b820 | 1749 | 45.5 | 57.0 |
| 0x0051dca0 | 1411 | 40.0 | 40.0 |
| 0x0052ab00 | 3084 | 32.2 | 50.3 |
| 0x005185a0 | 3242 | 27.3 | 46.2 |
| 0x0051c220 | 1990 | 25.1 | 35.0 |
| 0x0052cb00 | 1633 | 22.9 | 43.7 |

Per-row residual class and what was tried: `helper_notes_h1.md`..`helper_notes_h7.md`, one line per function. Round-2
lines in h6/h7 supersede the round-1 lines for the same RVA. Helper notes report 0x0051c220 as the one file failing the C++ check
(raw-converter form, not identical). Permuter caution (h1): one 92.8% permuter variant of 0x0051a6c0 moved a store
above the computation it uses and was rejected. Kept files follow the official source's order.

## Raw data (not committed), `D:/MashedWT/MATCH-S23_data`

| file | sha256 |
|---|---|
| decomp.json | 59f3c7de4aafa2e81981c5add49d5e8916dc15e8f78db8b8887ee5b54202857c |
| g2s/results.tsv (best of both) | e107d15291c6679115f87752927a96afd138e660c602566e541464c8eefaacab |
| g2s_game/results.tsv (/O2 /GS /Ob1) | 146834a024a576c6ea9b276da1d12361c97959a9cfb7328de220d412e1a77731 |
| g2s_g5/results.tsv (/O2 /Ob1 /G5) | f1d39944f71b131a47a0891fb81e541cab36aeaf0e6d3722f5e895534e6f89f7 |
| libsweep/base.tsv (official sources /O2 /G5, libmatch pairing; source of libpair.tsv) | ef6edb9be0d5492a782bce89be5e6bce84d7100cec8c7f5ffdfa7323beb470d3 |
| ltcg/nogl.tsv (LTCG probe, no /GL) | f62f90077f04603ba5c244b7a8eb04a0f4115d9496b79f5ca2fa914fe3267f3c |
| ltcg/gl.tsv (LTCG probe, /GL + /LTCG) | 663380ba8670c84a9336e514e0812535a9eb886ef7233c27d4a5b4e2ed599983 |
| notes/h1..h7.md, scratch/ | helper reports (copied here as helper_notes_h*.md), scratch |

The official libpng 1.0.12 / zlib 1.1.3 sources are the ones MATCH-LIB fetched (main checkout's gitignored
`tools/_download/pngzlib/src/`, SHA-256 in `verify/matchlib_20261031/RESULT_MATCH-LIB.md`).

## Follow-ups (not filed)

- Subsystem: the 118 rows are libpng / zlib / image-loader code filed under `util`. L-70 (a) refiled only the 56
  addresses identical to the official sources. Whether the non-identical libpng/zlib rows get the same refile (and
  leave the P-DoD port count) is a lead or user call. This step does not touch hooks.csv.
- Compiler check for the five residual families (see [UNCERTAIN] above), if a VC7.0 retail or VC7.1 c2 turns up.
- Port leg for the 57 when the port axis picks this range (MATCH-S19 `SWAP_BRIEF.md`). L-70 keeps libpng/zlib out of
  the port count unless the port decodes PNG itself.
