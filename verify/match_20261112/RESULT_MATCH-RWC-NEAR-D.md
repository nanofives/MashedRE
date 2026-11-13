# RESULT MATCH-RWC-NEAR-D (run 41): doubled RW match lane, 0x004f1fb0 and 0x0053dea0

Row kind: match-only (no build, no gates, no game lock). Branch `orch/MATCH-RWC-NEAR-D` from `orch/integration`
`a616fcc3b`. Inputs: the MATCH-RWC-NEAR-B RESULT (`verify/match_20261112/RESULT_MATCH-RWC-NEAR-B.md`, 0x004f1fb0 83.1%,
0x0053dea0 71.9%). Compiler: `rw9044` (VC6 SP5 cl 12.00.8804 + Processor Pack c2 13.00.9044, L-131), flags
`/O2 /Ob1 /G5` from each source's `// FLAGS`, scored with `re/tools/match/match.py`. No Ghidra pool slot was taken:
`match.py` reads the original bytes from the anchored image, and no decompilation was needed.

## Verdict

GREEN. **0x004f1fb0** reaches 100%, BYTE-IDENTICAL 1134 B under rw9044, relocheck OK (30 checked, 0 unchecked), plain C
with no inline asm: one C4 by `c4=match`. **0x0053dea0** moves 71.9 to 76.6 and is not promoted.

## 0x004f1fb0: 83.1 -> 100 (promoted)

`re/match/rwl03/004f1fb0.c` had no `// TOOLCHAIN` line, so every earlier score ran under the DDK default. The file now
carries `// TOOLCHAIN rw9044`. All residuals at 83.1 were stack-slot assignment plus one register swap. Steps:

| Step | ddk | rw9044 | What changed |
|---|---|---|---|
| NEAR-B source | 83.1 | 83.1 | frame 0x18 against the original's 0x1c |
| per-run loop counted by `i` (`for (i = 0; i < numRuns; i++)`, the compiler reverses it to a count-down) | 83.1 | | loop-entry test becomes `jbe` as in the original (was `je`) |
| `Strip *strips` at function scope, `strips = 0` before the run loop | 93.8 | 95.5 | strips is address-taken, so it holds the bmesh parameter slot for the whole function. The run-split counter then gets its own local and the frame becomes 0x1c, as in the original |
| mesh allocation size stored in the function-scope `size` the header pass also uses | 91.9 | **100** | fixes the eax/edx swap at the mesh `RwMalloc` and the header-pass slots: idx in the param_2 slot, the loop bound in the param_3 slot, the result in the local at frame +0x10, as in the original |

Tried without effect (all 93.8 ddk): declaration order of the function-scope locals (up to 300 random orders), hoisting any
subset of the nine block-locals (512 subsets), early `return 0` for a NULL sort table, typed `RwGlobals` struct for the
memory macros, `__forceinline` helper for the header pass, index forms of the run loop, merged mesh/dst variables, other
spellings of the mesh size and of `idx`. `match_permute.py` on the 83.1 source (1134 compiles) and on the 93.8 source
(1390 compiles) found nothing.

`c4_reclass.py batch --tsv re/promotions/MATCH-RWC-NEAR-D.tsv` (dry run):
```
[1/1] 0x004f1fb0 match PASS byte-identical 1134 B, relocheck OK (30/0)
batch: 1 pass, 0 refused, 0 skipped (already C4), 1 rows  [by route: match=1]
```
`relocheck.py re/match/rwl03/004f1fb0.c`: `0x004f1fb0 OK checked=30 unchecked=0`. Under ddk the same source scores
91.9, so the match holds for rw9044 only, which is the recorded compiler for RenderWare rows (L-131).

## 0x0053dea0: 71.9 -> 76.6 (not reached)

| Step | rw9044 | Edit |
|---|---|---|
| NEAR-B source | 71.9 | |
| second `FUN_0053e690` argument written `GETCOORD(size, maxAxis)`, `maxSize` pointer variable removed | 74.7 | the original passes it by fld/fstp, the pointer variable gave mov/push |
| both leaf-count tests `n <= 0xef` | 75.1 | the original compares with 0xef and branches `ja` |
| `tri++` in the triangle-loop for header | 75.3 | counter `inc` before `add edx,8`, as in the original |
| `match_permute.py` (2102 compiles): compare written `DAT_005ceac0 > bestScore`, `bestAxis = 0` moved above the `numLeft` store in the same else branch | 76.6 | both preserve semantics |

Tried without gain: hoisting the block-locals to function scope (all but `Err e` neutral at 75.3, all of them 70.3),
merging `split` and `split2` (72.2), `int i`, pointer and reversed-index forms for the verts and idx indexing, six
spellings of the median block (order of lo / mid / hi / medianKey, `lo = hi = mid`). Residual: stack-slot order (for
example bestAxis at frame +0x28 in the original, +0x00 here, and value / tries / scale / q shifted), register choice in
the median block, and the BuildParam child copy. The body keeps one RW inline-asm macro (`RwInt32FromReal`) inside C, as
NEAR-B noted. Under ddk this source scores 36.7.

## Counts

- C4 by `c4=match` gained: 1 (0x004f1fb0). Replay-verified (`c4=frida-replay`): 0.
- Score gain without promotion: 1 (0x0053dea0, 71.9 -> 76.6).

## Files

- `re/match/rwl03/004f1fb0.c`: matched source, `// TOOLCHAIN rw9044`, header note `MATCH-RWC-NEAR-D`.
- `re/match/rwl03/0053dea0.c`: improved source, header note `MATCH-RWC-NEAR-D`.
- `re/promotions/MATCH-RWC-NEAR-D.tsv`: one row (0x004f1fb0, route match). Dry run only on the branch.
- `verify/match_20261112/scores_MATCH-RWC-NEAR-D.tsv`: before and after per RVA.
- Neither source is read by the build (no `rwl03` reference under `mashedmod/`), so no VS2022 compile was needed.
- `hooks.csv` and `re/analysis/CHANGELOG.md` untouched (HB-TRACKER-AT-MERGE).
