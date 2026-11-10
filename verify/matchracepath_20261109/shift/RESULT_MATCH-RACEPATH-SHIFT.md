# RESULT MATCH-RACEPATH-SHIFT (race-path byte-match residue, smallest first), 2026-11-09

**Verdict: RED by the yield rule. 0 of 6 rows closed, so no promotion. 3 sources are strictly better and committed:
0x00408b00 214 -> 93 bad B, 0x00405540 256 -> 51 bad B (relocheck OK, length now the original's), 0x0047d3c0
265 -> 67 bad B. The other 9 rows of the brief were not started (yield rule: after 6 rows worked, fewer than 2 closed,
stop).**

Branch `orch/MATCH-RACEPATH-SHIFT`, base `orch/integration` @ 3fbf77214. Every score uses `re/tools/match/match.py
--bytes` (XP DDK cl 13.00.9176 / c2 13.00.9178) with each file's own line-1 FLAGS, unchanged. No `__asm`, no naked
body, no pragma. No mashedmod/ edit, no game run, no Ghidra use, no tool code change, no tracker change.
`grep -rn "<file name>" mashedmod/src` for the six worked files: the only hits are comments naming the RVA
(`Race/RoundGrid.h:39` for FUN_00408b00, three AI files for FUN_00416250); no `#include` of any of the changed
sources, so the port compile and the default gates are unaffected (not run). The t01g file 0x00416250 is unchanged.
Each changed file holds one `// MATCH:`.

## Rows worked (smallest residue first)

| rva | before | after | first divergence | verdict |
|---|---|---|---|---|
| 0x00416250 | 88.9%, 116 / 2015 B | unchanged | +0x339, no length change | not closed |
| 0x0045efe0 | 89.8%, 172 / 1540 B | unchanged | +0x14c, ours 1 insn short | not closed |
| 0x00408b00 | 90.2%, 214 / 1932 B | 97.0%, 93 / 1932 B | +0x11f -> +0x1f5 | improved |
| 0x005ace70 | 89.7%, 242 / 364 B | unchanged | +0x1b, ours 9 B short (merged exit) | not closed |
| 0x00405540 | 88.1%, 256 / 487 B | 93.4%, 51 / 495 B | +0x3e, length change fixed | improved, relocheck OK |
| 0x0047d3c0 | 91.9%, 265 / 649 B | 96.5%, 67 / 638 B | +0xb2, length change fixed | improved |

Per-row detail, compile counts and source sha256: `results.tsv` (pilot columns plus `first_divergence`).

## What moved the three improved rows

1. **0x00408b00.** The original loads `d.y` / `d.z` of the first `FUN_00426d00` result with `fld` and spills it with
   `fstp` to the dead `out_ang` argument slot before the second call; ours copied it with `mov`/`mov` through the inline
   getter. The BIG comma-rvalue lever applied inside the getter, `return (0, FUN_00426d00(h, n)->y);`, gives the
   `fld`/`fstp` spill, but reversed the two calls. `d.y = -GY(h, 3) + GY(h, 0)` restores the original call order and
   its `fsubr` (same value: `a - b` is `a + (-b)` and negation is exact). 214 -> 97 B. The s3/s4 multiplies back in
   x, y, z order: 93 B. Left: the `mov eax, place; cmp eax, 0x64; pop edi; pop esi` group sits inside the s3.z / s4
   multiplies in the original and after them in ours (8 orders tried, permuter 1532 compiles, no byte gain), one
   `lea eax, [eax+edx-4]` base/index swap (6 operand orders of `i = place - 4 + DAT_0063a5e4`, no change) and the
   `r.z` `fstp` placed one instruction earlier in three `*out = r` tails.
2. **0x00405540.** The first length change was block 2's dot product with the `{0,0,1}` axis: 9176 factored
   `v.x*0 + v.y*0` into `(v.x + v.y) * 0` (2 instructions short, 487 vs 495 B). `v.x * zaxis.x + (0, v.y * zaxis.y) +
   v.z * zaxis.z` keeps the two products and gives the original's length. Then the dot operand order: field first in
   block 1 (`(0, m->up.x) * v.x`), and in block 3 `(0, x-term + y-term) + (0, m->right.z) * v.z`. Then the compare
   loads: `d = FUN_004a3384(..); d = d * 57.29578f;` as two statements (same double arithmetic) puts the
   `fld v.?` before `add esp, 8` in all three blocks, as in the original. Last, independent stores reordered:
   `a[0]` / `a[1]` below the next block's first matrix copy (permuter, 953 compiles) and the `v.z` copy before them
   (order search). 256 -> 51 B, relocheck OK (17 checked). Left: in each block the original loads both matrix
   elements first and puts the zero store and the last copy after the two pushes for `FUN_004c39b0`; 12 rows, all
   scheduling. Statement orders (all 6 / 24 per block), comma rvalues on the loads, the assignments moved into the
   call arguments, a pointer local, a temp: no gain.
3. **0x0047d3c0.** The pilot's double intermediate on one statement, `v.y = (float)(v.y * (double)DAT_005cc9dc)`,
   stops 9176 keeping the scale constant in an x87 register (`fld k; fmul st(2)` ... `fstp st(0)`): 173 -> 170
   instructions, 649 -> 638 B, 265 -> 76 B. The operand is the float global, so the multiply stays `fmul dword`
   (the pilot's guard against a qword constant holds, 0 `qword` in the listing). `v.z = (float)((0, v.z) * (0,
   (double)DAT_005cc9dc))`: 67 B. Left: v.x (ours `fld k; fmul st(2)`, original `fxch st(1); fmul [k]`, 8 spellings),
   v.z still loads k first, and the `e0` value of `DAT_008816e0` stored to a stack temp `[esp+0x14]` and copied from
   it to `DAT_00883dec` in the original (5 temp forms, a named temp is worse: 259 B). Permuter 671 compiles, no gain.

## Rows not closed, source unchanged

- **0x00416250** (116 B, 24 compiles). The source's own residual (1): `volatile float steer` is always loaded first,
  the original has `fld b; fcomp [steer]`. New evidence: writing steer into an address-taken slot that already
  exists (`tgt[0]`, `pos[0]`, `ang`, `speed`) gives exactly the original compare shape (`fld [ecx+8]; fcomp [slot]`),
  but the frame shrinks by 8 and the call result is stored with `fstp` (no register copy for the tail tests): 524 B.
  So the original's steer is a separate memory-resident slot without volatile semantics. A 1-element array, a
  struct, a dead `(void)&steer`, a pointer copy and `__inline` helpers taking `&steer` are all enregistered (frame
  -8, 892 B). The comma rvalue on the field side has no effect while steer is volatile.
- **0x0045efe0** (172 B, 18 compiles). The original keeps the CSE'd `22050.0 - sin(t) * 500.0` on the x87 stack across
  the `h != 0` branch. Every form that makes cl CSE it (float / double / long double local, file-static phase with the
  expression repeated, comma, conditional and `&&` statement forms) spills it to a float temp `[esp+4]` and grows the
  frame from 0x24 to 0x28 (209 B).
- **0x005ace70** (242 B, 6 compiles). 9176 cross-jumps the first and the third `return 1` exits, so ours is 9 B short
  and every later byte is shifted. A `goto` form, an embedded-assignment return, an else chain and `!` forms are merged
  the same way, and constant forms fold. The file's header already classes it (a) candidate.

## Not started (yield rule)

0x00546e70, 0x00413cb0, 0x00463640, 0x004252c0, 0x00468d80, 0x0047eb30, 0x0040e590, 0x0047def0, 0x00412f30.

## Gates and checks

- relocheck (`re/tools/match/relocheck.py` on the three changed files): 0x00405540 OK (was BAD on 11 sites);
  0x00408b00 BAD on 3 sites (was 12), 0x0047d3c0 BAD on 21 sites (was 44). Every remaining BAD site lies inside a
  still-misaligned span listed above; no relocation target changed.
- No `c4_reclass.py` run: nothing is byte-identical.
- Tracker lint: `py -3.12 re/orchestrator/tracker_lint.py --repo . --base orch/integration`, NEW = 0 (no tracker file
  touched).

## Next slice (handoff)

- 0x00405540 is closest (51 B, 12 rows, one class: v-copy scheduling around the pushes). Worth one permuter run with
  `--only move,swap` from this source and a longer walk.
- 0x00416250: try a separate float local whose address is taken by a real call the original also makes (none found
  yet), or give `steer` a slot inside an existing address-taken aggregate so the frame keeps its 0x44.
- The comma rvalue lever also works on function-result fields (`(0, f()->y)` inside an inline getter) and stops
  9176's `(a + b) * c` factoring; the pilot's double intermediate also breaks a register-held constant CSE. Both are
  worth adding to the next race-path x87 slices.

## Files

| file | what |
|---|---|
| `results.tsv` | per-row before/after, verdict, residual, compiles, sha256, first divergence |
| `variants/` | the variant specs run (`v416*`, `v45*`, `v408*`, `v5ac`, `v405*`, `v47*`) |
| `tools/` | `tryv.py` (the pilot's string-replacement runner), `subv.py` (the same with a function per variant); scratch |
