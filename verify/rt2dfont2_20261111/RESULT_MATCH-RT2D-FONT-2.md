# RESULT MATCH-RT2D-FONT-2 (2026-11-11)

Row: shape the three Rt2d residuals of MATCH-RT2D-FONT to a byte match with relocheck OK: 0x00554390 (62.6%),
0x00554200 (69.5%), 0x00556e90 (98.0%). Match-only row: no build, no gates. Branch base `orch/MATCH-RT2D-FONT`
c3a567839. Compiler XP DDK cl 13.00.9176 / c2 13.00.9178 (`tools/vc7ddk`), flags `/O2 /Ob1 /G5` as C.

## Verdict

0 of 3 reached 100%, so nothing is promoted (no `c4_reclass.py` run). One row improved (0x00556e90 98.0 -> 99.0%).
The other two stall on code-generation behaviour that the recorded DDK back end does not produce for any source
or flag set tried. The RenderWare objects in MASHED.exe were built by a different compiler, build 9044, which is not
on disk (details below).

| RVA | before | after | source | stalled on |
|---|---|---|---|---|
| 0x00556e90 | 98.0% | **99.0%** (198/198 ins) | re/match/s24/00556e90.c (updated) | one integer copy of a spilled delta scheduled one x87 instruction late, twice |
| 0x00554390 | 62.6% | 62.6% | re/match/rwl05/00554390.c (unchanged) | DDK c2 merges every `return 0` tail and shrink-wraps the register saves |
| 0x00554200 | 69.5% | 69.5% | re/match/s24/00554200.c (unchanged) | DDK c2 jump-threads the failure paths and merges result with obj |

## The compiler finding

Rich header of `original/MASHED.exe.unpatched` (pefile): prodid 0x1c (Utc13_C) build 9466 x373 is the game code,
prodid 0x1c/0x1d build 9178 x8/x49, and **prodid 0x30 build 9044 x349**. The DDK we match with is 9176/9178. The 349
build-9044 C objects are the ones left over once the game code and the 9178 objects are counted. RenderWare has no
other candidate of that size, so Rt2d is among them [UNCERTAIN: no object-to-function map ties a given RVA to a
Rich entry. The evidence is the object count and the behaviour below.]

Probe (`verify/rt2dfont2_20261111/compiler_probe.txt`, wrapper `m6_probe.py` adds SimGolf's local VC6 12.00.8168
and VC6 SP5 c2 12.00.8966 to match.py; same sources, `/O2 /Ob1 /G5`):

| RVA | DDK 9178 | VC6 8168 | VC6 SP5 |
|---|---|---|---|
| 0x00554390 | 62.6% | **80.9%** | 80.9% |
| 0x00554200 | 69.5% | 69.5% | 69.5% |
| 0x00556e90 | 99.0% | 86.9% | 86.9% |
| 0x00557110 | **100%** | 86.0% | 86.0% |
| 0x00552920 | **100%** | 54.8% | 54.8% |
| 0x00553cf0, 0x005c4ad0, 0x005c4c60 | 100% | 100% | 100% |

What the original's code shows:
- **One epilogue per return.** 0x00554390 pushes ebx/ebp/esi/edi at entry (0x00554393..0x00554396). Every read failure
  returns through its own `pop edi; pop esi; pop ebp; pop ebx; add esp,0x30; ret` with the zero already in eax (for
  example 0x005543c2..0x005543c9 and 0x005543f0..0x005543f7). Only the version check, where eax is not zero,
  carries `xor eax,eax` (0x005543d9). VC6's c2 emits this shape (80.9%, 429/439 instructions, every epilogue
  matching). The DDK's c2 never does: on a 17-line toy (`toy.c`, output `toy_ddk_tailmerge.txt`) every `return 0`
  jumps to one shared `pop; xor eax,eax; pop; ret` tail. Writing the returns as `return (Font *)r` with r the read
  result, as `font = 0; return font`, as `(Font *)0`, as an `&&` chain, or as nested ifs with one exit gives
  byte-identical DDK output or a lower score. A VC6 front end with the DDK c2 (`/B2`) gives the DDK result
  (62.6%), so the tail merge is a back-end property.
- **IEEE float compares.** The original tests `fnstsw ax; test ah,0x44; jp` (0x00557110, 0x00556e90). DDK emits that.
  VC6 emits `test ah,0x40; je`, with or without `/Op`. That is why VC6 loses 0x00557110 and 0x00556e90.
- So the build-9044 back end has a VC7-style compare and a VC6-style epilogue. Neither local compiler has both.
  No other c2.dll exists on disk (searched Proyectos, Program Files (x86), D:).

## Per function

**0x00556e90 (Rt2d brush set RGBA), 99.0%.** Lever: the two alpha deltas written `(float)((double)(d - c))`
(memory ddk-match-levers, double intermediate). That moved the `mov esi,[esp+0x18]` reload of param_3 to the original
position, one of the two residual pairs. What is left is one pair per block: the original loads the spilled blue
delta (`mov edx,[esp+0x1c]` at the d0 block, `mov edx,[esp+0x10]` at the d1 block) between `fld` and `fsub` of the
alpha step, and ours loads it after the `fsub`. Tried, all at or below 99.0%: 2^8 per-field double masks,
5^4 x 2 subtraction forms (plain, double left, double both, float cast, comma rvalue, negated, added negation),
1,000+ named-local delta variants, inline From/Sub helper functions (59.6%), raw-copy placement (61-84%),
700 random combinations of subtraction, conversion and compare-order forms, and match_permute (2,035 compiles, walk
300). `x87 instruction order is never admitted` (L-126), so this row stays unmatched with the DDK.

**0x00554390 (Rt2d font stream read), 62.6%.** No change in the source. The residual is the epilogue shape above
(about 70 instructions) plus what follows from it: the loop counter `n` lives in the dead stream parameter slot
`[esp+0x44]` in the original, and the frame is 0x30. Under VC6 the same source reaches 80.9%, with 0x2c frame bytes
and `shl ecx,1` for `add ecx,ecx` left. That points at the build-9044 compiler again, not at the source.

**0x00554200 (Rt2d path stream read), 69.5%.** The original keeps the result in ebp apart from the node (`mov ebp,esi`
at 0x00554256), zeroes it on each failure (`xor ebp,ebp; jmp 0x0055432d`), and threads only the header failure
(`xor ebx,ebx; jmp 0x00554335`). The DDK threads every failure and drops the result variable. Tried 45 structural
variants: failure via break, nested else and goto; five final-return shapes; three header shapes; three init forms.
All are at 69.5% or lower. VC6 gives the same 69.5%, so this one is not explained by the epilogue rule.

## What would close them (for the lead)

A decision, not a worker step: find the build-9044 compiler (Rich prodid 0x30). The candidates are a pre-release
VS .NET 13.00.9044 or a VC6-family compiler shipped in an SDK or DDK of 2001-2002. Then add it as a match.py
toolchain for the RenderWare slice and re-run the RW residuals. Until then, RW rows that fail on return-tail
merging or x87 scheduling should be expected to stall under the DDK. Running the decision through the ledger is the
lead's call. Recommendation: file it as one L-n row covering every RenderWare MATCH residual, not per function.

## Files

- `re/match/s24/00556e90.c`: alpha deltas through a double intermediate, header comment with the residual.
- `verify/rt2dfont2_20261111/compiler_probe.txt`, `m6_probe.py`: DDK vs VC6 table.
- `verify/rt2dfont2_20261111/toy.c`, `toy_ddk_tailmerge.txt`: DDK return-tail merge on a minimal function.
- `re/tools/match/test_equiv.py` still points at the frozen `equiv_tests/refs/00556e90_held.c`, so the live source
  change does not move its fixtures.

Commands: `py -3.12 re/tools/match/match.py re/match/s24/00556e90.c` (99.0%, 198/198);
`py -3.12 re/tools/match/odis.py 0x00554390` for the original listing.
