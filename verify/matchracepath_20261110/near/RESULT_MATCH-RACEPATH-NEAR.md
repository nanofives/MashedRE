# RESULT MATCH-RACEPATH-NEAR (2026-11-10)

Lead: MATCH-RACEPATH (cmw9wztyh0nt1le1cp3q7r2ab). Worker session cmwa42dss1l8ple1ch1kmb6yd.
Branch `orch/MATCH-RACEPATH-NEAR`, base 1cbf98965 (orch/integration e9fce5b22 + FRESH2, FRESH3, X87B).

## Verdict

**Yield 1 / 6 worked. Stopped by the yield rule** (6 rows worked, fewer than 2 closed), so 0x004128f0 and
0x0040be50 were not started.

- Closed and promoted C2 -> C4 by match: **0x0040ca00** (`re/match/racepath/0040ca00.c`, 908 B byte-identical,
  relocheck OK 48/0, symbol ends 0x0040cd8b, next pc_funcs entry 0x0040cd90, no overlap).
- Strictly better source committed, not closed: **0x00411ae0** 27 -> 13 B (value unchanged).
- Not moved: 0x00488e70 (7 B), 0x0040aef0 (12 B), 0x0048aa50 (17 B), 0x0048ab70 (34 B).

Per-row table: `results.tsv` (FRESH's columns). C4 batch: `c4_batch.tsv`, dry run `c4_dryrun.tsv` (1 PASS, 0 refused),
`relocheck.tsv`.

## What closed 0x0040ca00 (raw g2s 46.9%, 729 B differ -> 0)

All floats are `__real@` literals (addresses in the source header). Steps in the order they moved bytes:

1. **Loop shape:** both loops as `i = 4; do { i--; ... } while (i != 0);`. cl strength-reduces the array indexing to
   the original's pointer walks (0x007f1a54 / 0x008856ac in loop 1, 0x007f0a34 in loop 2) and tests the pointer
   (`cmp ebp, 0x88229c` at 0x0040cc45, `cmp edi, 0x7f0a04` at 0x0040cd78). `for (i = 3; i >= 0; i--)` gives offset
   IVs (44.4%); `while (i > 0)` gives `jg`, `!= 0` gives the original's `jne`.
2. **Per-branch call tails:** each parity branch holds its own `if (h != 0) FUN_0040c690(...)`. cl cross-jumps the
   identical tails (`jmp 0x40cbc6` at 0x0040cb9f, `jmp 0x40cd07` at 0x0040cd41). One call after the if/else left
   the register assignment off (src in EBP instead of EBX): 543 B -> 16 B.
3. **Stack packing by scope:** the loop-2 FUN_0046d320 out value is a block-scoped `int c2` in the loop-2 body. cl packs
   it into the loop-1 pointer spill slot [esp+0x14], as the original does (Ghidra shows one `local_38` for both).
   At function scope it gets its own slot and the frame grows to 0x40. Then `int n` block-scoped in the loop-1 body
   lets the loop-2 `float f` pack into n's slot [esp+0x20] (2 B -> 0). Declaration order has no effect (41 shuffles,
   all equal).
4. **Sum order:** the source `v.z*v.z + v.x*v.x + v.y*v.y` emits `y*y + x*x`, then `+ z*z` (0x0040cac4..0x0040cade),
   the original's order. The source order y, x, z emitted z first. Recorded as observed; the bytes are identical, so
   the computed value is the original's.

New levers for the catalogue: (2) per-branch tails that cl cross-jumps, (3) block scope to steer cl's stack packing.

## Rows not closed

- **0x00488e70 (7 B):** `fstp len` after FUN_004c39b0 lands after `mov ecx, e->count` (original: before, 0x0048902b).
  11 more forms, none moved it: `volatile float len`, `*&len =`, count read through `DAT_0086ae20[j]`, an
  `if (e->count > 0)` guard, a count local before or after the call, float10 return, `e->count > i`, the
  `pos.ptr` check moved after the call. `volatile` not moving it shows the scheduler is not held by aliasing here.
- **0x0040aef0 (12 B):** fld of m->pos.y and v.y one slot after a push (0x0040afbe, 0x0040b020). dblgreedy 100 variants
  (no gain) and 10 hand forms: F/W/comma on each y line, `*=` on the second triple, F on x and y in one block.
- **0x0048aa50 (17 B, X87B variant):** out[1]'s fld issued after `push ebp` (original: before). dblgreedy 52 variants,
  plus out[] stores folded into the call argument with a comma, F/plain/comma/long double forms for out[1]. No gain.
  Source unchanged (the variant stays under `verify/matchracepath_20261109/x87b/variants/`).
- **0x00411ae0 (27 -> 13 B):** `pair.best = &best` moved after the two identity blocks puts the &best store after the
  0x0063bb0c test. `pair` is read only by FUN_0041ad00, so the value is unchanged. Residual: that store is still two
  instructions early, and the final sum is `07f107a + (0063bb1c + diff)` where the original has
  `(07f107a + 0063bb1c) + diff` (0x00411b43). 175 forms did not change the final add: 36 parenthesised orders x 5 diff
  spellings, all 120 flat orders of the five terms, the sum split over statements / the param / a diff local, int and
  uint casts per term, comma rvalues, three `__inline` helpers, a ternary. Only the division order moves (b39 first
  13 B, b38 first 19 B). Other `pair.best` positions: after the first or second FUN_00482c10 if (94 / 119 B), after
  FUN_0041a9b0 (123 B), between the identity blocks (131 B), with pair.ghost moved (28 B).
- **0x0048ab70 (34 B):** finding on the vertex base pointer. With the 10 vertex stores in any order where col.a is
  written last, cl takes the **second field written in source order** as the strength-reduced base (probed: each of
  the 7 other fields placed second becomes the base; 84 interleavings of the col bytes all follow it). The original's
  base is &a.z (0x0071ba38) while its fp stores run a.x, a.y, a.z, and the fp stores follow source order (the
  scheduler does not move a.y above a.z even when legal: `float py = p->pos.y` before the a.z store is folded back).
  Exception seen: with the col.a store earlier than last, the base becomes &a.z with natural a order
  (e.g. `ax ay az ca cr cg cb bx by bz`, 118 B, since col.a's fp code then moves). [UNCERTAIN] the rule behind that
  exception; missing evidence: a form that keeps col.a's fp code after the b stores and still gets base &a.z.
  Also tried: embedded/comma assignments for a.y inside a.z (flattened, base stays &a.y), a `float *` to a.z (base
  &a.y, 69 B), dblgreedy on the `ax az ay` order (220 variants, no gain).

## Not worked (yield rule)

0x004128f0 (hud, 1014 B) and 0x0040be50 (gameplay, 1849 B; FRESH4 handoff: EAX-arg static callee with stand-in
caller 0x0040c5b0, 6-case jump table). Both still have no hand source.

## Tools

Copied into `tools/`: `tryv.py`, `rdf.py`, `odis.py` (FRESH4), `dblgreedy.py` (X87A, transforms W and F). Scratch
variants stayed in the session scratchpad. Decomp of 0x0040ca00 came from the main checkout's
`re/tools/decomp_pc.py --port --callees --json` on read-only slot Mashed_pool0.

## Gates

Not run: no mashedmod/ change, no exe change. Tracker lint (base orch/integration): OK, NEW=0, pre-existing=3.
