# RESULT MATCH-RWC-NEAR-B (run 40, part B): RWC near misses, compiler shaping

Row kind: match-only (no build, no gates). Branch `orch/MATCH-RWC-NEAR-B` from `f83829aad` (orch/RWC-MATCH, not yet in
orch/integration). Part B of ledger row MATCH-RWC-NEAR. Part A holds the four 98.5-99.3% RVAs and the ledger row. This
part does not touch the ledger. Compilers: XP DDK cl 13.00.9176 / c2 13.00.9178 (`ddk`) and `rw9044`, flags
`/O2 /Ob1 /G5` from each source's `// FLAGS`, scored with `re/tools/match/match.py`.

## Verdict

GREEN. **0x004f0ae0** reaches 100%, BYTE-IDENTICAL, so it gets one C4 by `c4=match`. The other three are out of reach
in this row, but all three moved up: 0x004f1fb0 37.0 to 83.1, 0x0053dea0 28.1 to 71.9, 0x00538d60 12.3 to 12.8 (that
last one is a correctness fix: 10 fild and 3 `__ftol` calls are gone). Promotion file:
`re/promotions/MATCH-RWC-NEAR-B.tsv` (dry run only, the merge train applies it).

## Promoted (route match)

| RVA | before | after | compiler | source |
|---|---|---|---|---|
| 0x004f0ae0 | 81.8% (regs 100) | 100%, BYTE-IDENTICAL 234 B | ddk and rw9044 | `re/match/rwl03/004f0ae0.c` |

What closed the esi/edi swap: the run 40 sources tested the allocation result first (`if (piVar1 != 0) {...}` or the
early-error form) and named the size in a local. RW's own shape is the success branch nested, `if (o) { ... return o; }`,
with the out-of-memory error for the header at the end. The size `num * 20` is written at both uses (alloc and error
argument) with no local, and the data pointer is tested through the struct field (`if (!o->data)`). With all three in
place the allocator gives the header pointer edi and the size esi, as the original does. The 7 spellings tried
(scratch variants rw1-rw7) and their scores were:
RW order with `if (!o)` first 24.7 / 67.5, data in a local 24.7, a size local declared first 29.9, `o->n` stored
before the test 24.7, nested success branch 100 (both compilers).

`c4_reclass.py batch --tsv re/promotions/MATCH-RWC-NEAR-B.tsv` (dry run):
```
[1/1] 0x004f0ae0 match PASS byte-identical 234 B, relocheck OK (9/0)
batch: 1 pass, 0 refused, 0 skipped (already C4), 1 rows  [by route: match=1]
```
relocheck: `0x004f0ae0 OK checked=9 unchecked=0`. Plain C, no inline asm. The struct typedefs (`Err004f0ae0`,
`Obj004f0ae0`) sit above the `// MATCH:` line so the permuter's function-span search still finds the body.

## Not reached (time-boxed)

| RVA | before | after | compiler | what was done | residual |
|---|---|---|---|---|---|
| 0x004f1fb0 | 37.0 | 83.1 (regs 85.1, 355/355 ins) | ddk = rw9044 | Rewritten from raw g2s as typed C: triangle / run / strip / mesh / mesh-header structs, RW memory calls through RwGlobals `0x007d3ff8` +0x108 / +0x10c / +0x11c, mesh-module globals at `DAT_00912118` (+0 serial u16, +4 freelist), qsort `0x005c24e0`, memcpy intrinsic for the index copy. 60.6 with a run pointer; 81.7 once the run table was written by index `runs[numRuns]` (the original strength-reduces from `&runs[0].num`, `lea [ecx+eax*4-8]` at `0x004f2129`) and both `n >= 2` guards were spelled `>= 2` (`cmp ebx,2 / jb` at `0x004f2025`). `match_permute.py` (6 rounds + walk 60, 1964 compiles) reached 83.4 by reordering the three final `RwFree` calls. The original frees sorted, then meshes, then runs (slots L0x14 / L0 / L0x18 at `0x004f23df`..`0x004f2400`), so the original order is kept: 83.1. | The original frame is 0x1c (7 locals, with param slots reused for the strip list and for `idx`), this source's is 0x18, so stack offsets differ throughout. In the header copy loop the original advances `dst` before the stores (`[ebp-8]` / `[ebp-4]`). |
| 0x0053dea0 | 28.1 | 71.9 (regs 79.5, 473/474 ins) | rw9044 (ddk 36.4) | Rewritten as typed C: build-param struct (0x3c bytes, copied for each child at `0x0053e32c` / `0x0053e384`), bbox `sup`/`inf` as floats (the raw body read them as uint->float, fild), RW `GETCOORD` byte-offset axes (axis += 4 to 0xc, `0x0053e211`), the RW inline-asm `RwInt32FromReal` (fnstcw / or ah,0xc / fldcw / fistp / fldcw at `0x0053e125`..`0x0053e142`), leaf and node structs, `0x11d` error struct. 69.8 once the main body sits in `else if (p->depth < 0x20)` (the error tail is laid out last, `0x0053e3db`). 71.3 with the max-axis test written `size[axis] > size[maxAxis]` (fld order at `0x0053dfb5`) and `minExtent = GETCOORD(size, maxAxis) * DAT_005cd03c` (fld then fmul [const], `0x0053dfd0`). Declaration order changed nothing. Permuter: 6 rounds + walk, 3043 compiles, best 74.7, but that variant reads `hi` before setting it (a statement moved above `hi = lo + mid`), so it was rejected. Of its four semantics-preserving edits, two were kept (store `split2.leftValue` before `rightValue` in the fallback, load `medianKey` before computing `hi`): 71.9. `tries >= 1` in the loop test drops the score to 54.7.. | The body uses one RW inline-asm macro (RwInt32FromReal) inside C, noted for any later promotion review (the body is not asm-only). Stack slot assignment (bestAxis / bestScore / bestValue / value at different offsets), the two float arguments to `FUN_0053e690` passed by fld/fstp in the original against push in this source (`0x0053dfec`), and loop-end reload order in the split search. |
| 0x00538d60 | 12.3 | 12.8 (regs 21.9, 746/755 ins) | ddk | Finished the int/float retype that run 40 began: `_DAT_005cc320`, `_DAT_005ce54c`, `_DAT_005e4574`, `_DAT_005cea1c` declared float, 7 `(float)x[i]` reads written `*(float *)&x[i]`, 3 `x[i] = (int)(...)` writes written as float stores. Compiled body before: 10 fild and 5 calls (3 `__ftol`). After: 0 fild and 2 calls, matching the original (0 fild, 2 calls). | Structure and register allocation over the whole 755-instruction body (regs 21.9%). A full typed rewrite is needed, and it does not fit this row. |

## Build inputs

None of the four sources is `#include`d by `mashedmod` (`grep rwl03/ mashedmod/src` is empty; the only hit for these
RVAs is an RVA in a comment in `Collision/CollisionWorld.cpp`), so no VS2022 compile was needed.

## Counts

- C4 by `c4=match`: 1 (0x004f0ae0). Replay-verified (`c4=frida-replay`): 0.
- Score gains without promotion: 3 (0x004f1fb0, 0x0053dea0, 0x00538d60).

## Files

- `re/promotions/MATCH-RWC-NEAR-B.tsv`: the promotion row (dry run only on the branch).
- `verify/match_20261112/scores_MATCH-RWC-NEAR-B.tsv`: before and after for each RVA.
- Edited sources (each carries a `MATCH-RWC-NEAR-B` header note): `re/match/rwl03/004f0ae0.c`, `re/match/rwl03/004f1fb0.c`,
  `re/match/rwl03/0053dea0.c`, `re/match/rwl03/00538d60.c`.
- `hooks.csv` and `CHANGELOG.md` untouched (HB-TRACKER-AT-MERGE).
