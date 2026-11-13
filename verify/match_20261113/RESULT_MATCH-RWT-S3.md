# RESULT MATCH-RWT-S3 (2026-11-13)

Verdict: **RED** (no 100% match). One source improved (0x004cbbd0, 77.4% -> 79.2%). No promotion TSV.
Match-only row: no build, no gates. Compiler: rw9044 (VC6 SP5 cl 12.00.8804 + PP c2 13.00.9044) and ddk
(cl 13.00.9176 / c2 13.00.9178), flags `/O2 /Ob1 /G5`.

## Re-score first (each source's own // TOOLCHAIN and // FLAGS)

| RVA | source | listed (RANKED_MATCH-RWT-REFRESH.tsv) | re-score ddk | re-score rw9044 | after this row |
|---|---|---|---|---|---|
| 0x004dcbb0 | re/match/rwl01/004dcbb0.c (no TOOLCHAIN line = ddk) | 77.5 | 77.5 | 77.5 | 77.5 (unchanged) |
| 0x004e5fc0 | re/match/rwl02/004e5fc0.c (no TOOLCHAIN line = ddk) | 77.3 | 75.5 | 75.5 | 75.5 (unchanged) |
| 0x004cbbd0 | re/match/s20/004cbbd0.c (TOOLCHAIN rw9044) | 75.5 | - | 77.4 | **79.2** |

The listed 77.3 for 0x004e5fc0 is the pre-RWC-MATCH-S02 score that counted the wrong `*(uint *)0xfffffffd` load
(source header). The listed 75.5 for 0x004cbbd0 predates the rw9044 re-score. ddk and rw9044 give the same
instruction stream for all three sources here.

## Shaping per function

**0x004cbbd0** (accepted): `n = 6` moved after `pl = param_1->planes` (permuter swap, 1280 compiles). Fixes the
`lea ecx,[eax+0x94]` / `mov esi,6` order at function start. Residual unchanged: the original keeps the y and z corner
values on the x87 stack (`fld n.y; fmul st(2)`, `fld n.z; fmul st(1)`, then `fstp st(1); fstp st(0)` after the
`fcomp`), ours multiplies by memory and consumes them. Tried with no gain: dot operand orders (5, all canonicalised to
the same code), `__inline` dot helper by pointer (77.4) and by value (65.1), each of x / y / z inline instead of a
local (66.0 / 71.7 / 79.2), while / for loop forms (77.4), inner-scope V3d + `dot` local (79.2), `<` and `!(<=)`
compare (79.2 / 77.4), `(double)` dot (75.5), flags `/G4 /G3 /Oi-` (79.2), `/G6` (60.4), `/Op` (62.5), `/Oy-` (73.6).
VS2022 `/Zs /TP` of `mashedmod/src/mashed_re/RwCore/RwCore_004cbbd0.cpp` (includes the source): OK.

**0x004e5fc0** (no change): residual is the same x87 shape. The original keeps `t` (the `[+0x5c]*[+0x58]` product)
and the x product on the stack (`fld [eax+0x1c]; fmul st(1); fst`, later `fld [ecx+4]; fadd st(1)`, then two
`fstp st(0)`), ours consumes `t` at its last use (`fxch st(1); fmul [eax+0x24]`). Plus the tail interleave
(`mov cl,[eax+3]` between the `[eax+0x4c]` load and `and`). Tried: multiply and add operand swaps (canonicalised),
RW-macro form through the destination fields, float pointer form (75.5), x forwarding only (67.3), tail swap (73.6),
compound `&= ~2u` / `|= 1` (75.5), int flag (75.5); flags `/G6` 46.4, `/Op` 64.6, `/Oy-` 67.3, others 75.5.
Permuter (1657 compiles) reached 77.3 only by commuting `(param_1 + 0x4c) & 0xfffffffd` back into the
`*(uint *)0xfffffffd` load bug: **rejected** (wrong semantics).

**0x004dcbb0** (no change): residual is register assignment. The original never enregisters `param_1`
(`mov ecx,[esp+4]` at entry, reloaded `mov ebx,[esp+0x14]` per loop iteration, final `div dword ptr [esp+0x14]`) and
keeps the end-offset variable in ebx; ours keeps `param_1` in ebx and spills the end offset to `[esp+0x14]`. Tried:
final store order back (69.2), compare form `>=` (76.7), assignment order at the fall-through (77.5), embedded
assignment in the loop compare (77.5), `register` hint (77.5), operand order of `param_1 * param_2` (77.5), statement
order (77.5), address-taken `param_1` (62.1, not kept). Permuter (1289 compiles): no improvement.

## Open
The 0x004cbbd0 / 0x004e5fc0 shape (x87 register variables kept live past last use and popped later) is common to
both; no source form or flag found that produces it under either toolchain. Next attempt per L-136: Opus high.

Permuter logs (not committed): log/permute/run_0x004cbbd0.txt, run_0x004e5fc0.txt, run_0x004dcbb0.txt in this worktree.
