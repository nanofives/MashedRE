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


---

# Attempt 2 (Opus high, 2026-11-13)

Verdict: **GREEN** (2 of 3 at 100% BYTE-IDENTICAL + relocheck OK). Promotion TSV `re/promotions/MATCH-RWT-S3.tsv`
(2 rows, route match). 0x004dcbb0 gets a semantic fix and stays open.

| RVA | before | after | toolchain | evidence |
|---|---|---|---|---|
| 0x004e5fc0 | 75.5 | **100.0**, 318 B BYTE-IDENTICAL, relocheck OK checked=0 unchecked=0 | ddk = rw9044 (both 100%) | re/match/rwl02/004e5fc0.c sha256 907404e95b30f678fe52c860948ad753f2c5e2dd90f77ab8d5bf19a0842c1dce |
| 0x004cbbd0 | 79.2 | **100.0**, 130 B BYTE-IDENTICAL, relocheck OK checked=0 unchecked=0 | rw9044 (file TOOLCHAIN) | re/match/s20/004cbbd0.c sha256 6c4f9b405ea1bfe50720455c411ad76d18a22695b07091701f16ba047956d1a9 |
| 0x004dcbb0 | 77.5 (void, see below) | 62.0 (semantically fixed) | ddk = rw9044 | re/match/rwl01/004dcbb0.c |

Both port TUs that include the changed sources pass VS2022 `cl /Zs /TP /DMASHED_STANDALONE`:
`mashedmod/src/mashed_re/RwCore/RwCore_004e5fc0.cpp`, `RwCore_004cbbd0.cpp`, `RwCore_004dcbb0.cpp`.

## The x87 residual, explained

The shape both earlier attempts could not reach (a corner value or product kept on the x87 stack past its last use,
`fld mem; fmul st(i)`, popped later by `fstp st(0)` / `fstp st(1)`) has one cause under c2 9044 and 9178:
**c2 consumes a dying float register variable in place only when it may swap the operation to `fxch; fop [mem]`**.
It does that for a plain field load, never for a `double` register variable and never when the memory operand is a
comma node `(0, p->f)`. Measured on both rows: a struct with `float x; double y, z` reproduced the whole dot of
0x004cbbd0 (`fld [ecx+4]; fmul st(2); fxch st(3); fmul [ecx]; faddp st(3); fld [ecx+8]; fmul st(1); faddp st(3)`),
but a double sum is compared as `fld [ecx+0xc]; fcompp; test ah,5` (double compare loads the float operand), and any
`(float)` conversion pops the dead values before the compare. The comma form keeps everything float.
Matched references with the same shape: 0x00546bf0 (float10 local, `fld [0x5cc32c]; fmul st(1)`), 0x004c4270.

## 0x004e5fc0 levers (in order of gain)

1. `t = [+0x5c] * [+0x58]` and the x product as `double` locals (75.5 -> 80.9, with y 81.8): original never consumes
   them, two `fstp st(0)` after the x sum. Semantics: the original adds the unrounded x87 product
   (`fld [ecx+4]; fadd st(1)`), which a double local states exactly.
2. z product in its own float local `fVarZ`, not reusing t (-> 91.8): the original spills it to `[esp+0x10]` while t stays live.
3. y/z sums as `(float)(k0->v.y + (double)fVar9)` (-> 95.5): field first (`fld [ecx+8]; fadd [esp+0x18]`). Plain
   operand swaps and comma forms on the add were canonicalised (91.8).
4. Tail flag store through a pointer local `f = (unsigned char *)(param_1 + 3)` (-> 99.1): c2 then cannot move the
   `+3` byte load above the `+0x4c` store (original order at 0x004e60e6..0x004e60f9). char*, struct and compound
   forms stayed at 95.5, volatile 96.4.
5. `(0, k1->v.w)` in the w line places `lea edx,[edx+edi*4]` before the `fsub` (0x004e6013), but flips the z sum to
   spill-first (98.2). `(0, k1->v.z)` in the z difference restores it: **100%** (`(0, k1->v.y)` in the y difference
   also gives 100%). Statement orders of pKeys/fVar7/k0/k1 (8), k1 forms (3), w forms (11) did not move the lea.
   Flag sweep (46 sets, both toolchains): no set beats /O2 /Ob1 /G5.

## 0x004cbbd0 lever

Commas `(0, pl->normal.y)` and `(0, pl->normal.z)`, grouped `z + (x + y)` as in the matched sibling 0x004cbb70.
128 combinations of comma placement x operand order x grouping: all 16 with commas on both y and z in the `z + (x + y)`
grouping hit 100%, everything else stayed below. Kept the n-first form
`(0, pl->normal.z) * c.z + (pl->normal.x * c.x + (0, pl->normal.y) * c.y)`.
Tried before the comma, no 100%: 48 operand orders x 2 groupings with the V3d local and with no local (all
canonicalised, 79.2 / 71.7), float / double scalar locals (forwarded, max 71.7), struct field type mixes (max 87.5),
12 compare forms on the `fdd` struct (max 92.7), embedded assignment and `__inline` float returns (92.5), double
products (83.0), 46 flag sets x 2 toolchains (best 79.2, `/Og /G5` only adds a frame).

## 0x004dcbb0: semantic fix, still open

The source carried a wrong store. Found branch was `DAT_007d70c8 = idx; idx = uVar4;` (stores the OLD index, which
c2 drops as redundant since idx was loaded from DAT_007d70c8). The original stores the NEW index:
0x004dcc27 `mov ebp,ecx`, 0x004dcc2c `mov dword ptr [0x7d70c8],ebp`. Fixed to `idx = uVar4; DAT_007d70c8 = idx;` and
the tail stores put back in the original order (DAT_007d70cc store at 0x004dcce9 before `div dword ptr [esp+0x14]`
at 0x004dccf2). Our listing now emits the store (`mov dword ptr [DAT_007d70c8], ecx`). **The 77.5% listed in
RANKED_MATCH-RWT-REFRESH.tsv, attempt 1's 77.5% and this attempt's permuter 77.5% (987 compiles, `log` at
scratch, it reached 77.5 only by swapping the store back) all measured the store-dropping source and are void.**
PROVISIONAL.tsv still lists 77.5 for this row; the knob-path provisional body now has the corrected semantics.

Remaining (62.0%, regs 77.7%, ops 93.4%, 121/119 ins): register assignment. The original never gives param_1 a
register (copy `mov ecx,[esp+4]` at 0x004dcbb0 before the pushes, reload `mov ebx,[esp+0x14]` per loop pass at
0x004dcc00, `div dword ptr [esp+0x14]` at 0x004dccf2) and keeps the end offset uVar5 in ebx and uVar7 in edi; ours
enregisters param_1 (edi), puts uVar7 in ebx and spills uVar5 into param_1's home slot `[esp+0x14]`. First divergent
byte at +0x0 (original `8b 4c 24 04` mov ecx,[esp+4]); 248 of 377 bytes differ.
Levers that failed on the corrected source: 183 declaration orders x 3 found-branch store forms (all 62.0),
`__inline` round-up helpers (3 forms, whole and loop-only) and a block-scope copy of param_1 (copies propagated,
unchanged), address-taken param_1 (`(void)&param_1`, `&param_1;` folded; volatile pointer 43.4; `if (&param_1==0)`
56.9; volatile param 36.4; all uses through `uint *pa` 62.0), natural for-loop shapes (62.0 / 39.5), permuter
(rejected, semantics). Next step: a dedicated row on c2's region allocator weights (what makes a 7-use param lose
to a 5-use local in the loop region); Fable gate 1 does not apply yet (one Opus high attempt on the corrected source).

## c4_reclass dry run (on the branch, no --apply)

```
[1/2] 0x004e5fc0 match PASS byte-identical 318 B, relocheck OK (0/0)
[2/2] 0x004cbbd0 match PASS byte-identical 130 B, relocheck OK (0/0)
batch: 2 pass, 0 refused, 0 skipped (already C4), 2 rows  [by route: match=2]
dry run: nothing written. CHANGELOG lines that --apply would prepend:
  2026-11-13  0x004e5fc0  FUN_004e5fc0  C2->C4  route match (L-93): byte-identical + relocheck OK, re/match/rwl02/004e5fc0.c; evidence: <RESULT>
  2026-11-13  0x004cbbd0  FUN_004cbbd0  C2->C4  route match (L-93): byte-identical + relocheck OK, re/match/s20/004cbbd0.c; evidence: <RESULT>
```

New lever for the memory hub (ddk-match-levers): comma rvalue on the memory operand blocks c2's swap-and-consume of a
dying float register variable; a double local is never consumed.
