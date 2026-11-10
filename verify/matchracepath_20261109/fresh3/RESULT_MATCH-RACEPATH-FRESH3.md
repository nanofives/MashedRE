# RESULT MATCH-RACEPATH-FRESH3 (race-path rows with no hand source, third slice), 2026-11-09

**Verdict: GREEN. 1 of 6 worked rows closed (17%): 0x00418560, promoted to C4 by route match. The yield rule
stopped the slice after the sixth row (fewer than 2 closed). 2 of the 8 rows were not worked (handoff below).**

Branch `orch/MATCH-RACEPATH-FRESH3`, base `orch/integration` @ 79a4dc85c. Every score uses
`re/tools/match/match.py` (XP DDK cl 13.00.9176 / c2 13.00.9178) with the file's own line-1 FLAGS,
`/O2 /GS /Ob1` in every file. No `__asm`, no naked body, no pragma. No mashedmod/ edit, no game run, no Ghidra
write, no change to `re/tools/match/` code. The new sources are in `re/match/racepath/`; nothing in
`mashedmod/`, `scripts/` or `re/tools/` references them, so the default gates are unaffected (not run).
`mashedmod/src/mashed_re/Ai/AiController.cpp` (hooks.csv file of 0x00418560) was read, not edited.

Decomp: `re/tools/decomp_pc.py --port --callees --json` on read-only pool slot `Mashed_pool0` (run with the
main checkout's copy of the script, because the worktree has no `mashed_pool/`). The "before" column is the
raw `g2s.py --flags "/O2 /GS /Ob1"` score of that JSON. Per-row before/after, residual, approximate compile
count and sha256: `results.tsv`.

## Yield

Worked smallest first: 0x00414c30, 0x00418560, 0x00414570, 0x0040c690, 0x00488e70, 0x00489500.

| rva | subsystem | before (raw g2s) | after | source |
|---|---|---|---|---|
| 0x00418560 | ai | 8.8% | 760 B BYTE-IDENTICAL (relocheck OK, 67 checked) | `re/match/racepath/00418560.c` |
| 0x00488e70 | util | 6.8% | 99.6%, 7 B differ | `re/match/racepath/00488e70.c` (kept, not promoted) |
| 0x00414c30 | ai | 3.4% | 96.5%, 50 B differ | `re/match/racepath/00414c30.c` (kept, not promoted) |
| 0x00414570 | ai | 10.5% | 93.0%, 156 B differ | `re/match/racepath/00414570.c` (kept, not promoted) |
| 0x0040c690 | gameplay | 7.7% | 80.5%, 638 B differ (4 B length shift) | `re/match/racepath/0040c690.c` (kept, not promoted) |
| 0x00489500 | gameplay | 7.1% | 48.8%, 734 B differ | `re/match/racepath/00489500.c` (kept, not promoted) |

0x00418560's 760 B symbol includes the 4-entry jump table at 0x00418848 (body 0x00418560..0x00418844).

## What moved the closed row (and the near misses)

1. **A switch whose `case 0` shares the default's code needs both written (0x00418560).** The bank-select
   switch at 0x00418717 is a 4-entry table whose entry 0 is 0x00418754, the default body. `case 0: default:`
   lets cl drop case 0 and emit a `dec` chain (89.1%). `case 0:` and `default:` as two separate identical
   bodies keep the table, and cl merges the bodies afterwards: BYTE-IDENTICAL. The same shape held in
   0x0040c690: 0xffff32ff is the BST pivot at 0x0040c849 with `je` to the default body, so it is a case of its
   own with the default's body, not a label on `default:`.
2. **Index a global record table directly, no pointer local (0x00418560).** `DAT_0089a4c4[car].f` gives
   `imul esi, 0x74` and `[esi + 0x89a4d0]` operands; a `AiVeh *a = &DAT_0089a4c4[car]` local gives `add esi, 0`
   and `[esi + 0xc]`.
3. **Callees pushed more arguments than their matched sources declare (0x00418560).** 0x00416250, 0x00416a30
   and 0x00417da0 are pushed (bank, car, ctrl, 0x42c80000 = 100.0f) at 0x00418776 / 0x004187be. Their s03
   sources take 3 parameters; this file declares 4 (cdecl, caller cleans), which is what the bytes show.
4. **cl orders the frame by reference count (0x00488e70, 67.2% to 92.0%).** Declaration order of locals never
   moved a slot (14 orders tried). The four lock structs land in the original order (pos, col, rgba, uv at
   T+0x14..0x2c) only when col has as many references as the others, which the source gets by writing
   `col[1]` in both branches of the `i == 4` test (cl cross-jumps the identical tails back into one store).
5. **Comma rvalue on a float copy gives fld/fstp copies (0x00488e70, 96.0% to 98.4%).** `wp.x = (0, e->pos.x)`
   compiles to `fld [esi-0x28]; fstp [esp+0x34]` as at 0x00488fa3; the plain assignment gives mov pairs. The
   same lever fixed the dot order (`(0, e->mtx->at.x) * cam->at.x`) and the `k * 0.5f` product, and the
   field-first `fsubr` in 0x0040c690 (`v.x - (0, m->x)`).
6. **Loop shape decides the IV base (0x00414c30).** The object walk must be indexed `o[i]` (a pointer `o++`
   keeps EBX at the base); cl then biases EBX to the field it judges most used. Three `o[i].sub` reads gave
   +0x14, one read into a local gave the original's +0xc (71.8% to 92.9%).
7. **Loop-scoped arrays change store scheduling (0x00414c30, 92.9% to 96.5%).** own[] / obj[] declared in the
   loop body let cl interleave their stores with the field loads as the original does; per-branch block scope
   gives the same scheduling but a frame 0x10 B larger (no slot sharing).

Guard checks: no closed row introduces a `qword` operand. Semantics read for each rewrite kept:
`!(s->f10 < 0.4f)` equals the original's `jnp` at 0x00414d9f (it also differs from `0.4f <= x` on NaN, and
the original's code is the `!(x < c)` form); the duplicated `col[1]` stores and the separate case 0 /
default bodies are the same value written on every path.

## Not closed (5 worked, kept as the next slice's starting point)

- **0x00488e70, 7 B:** `fstp [esp+0x18]` (len) after `call 0x004c39b0` is scheduled after `mov ecx,
  [esi-0x1c]` (e->count); the original stores first (0x0048902b). Tried: comma rvalue, double temp, block
  scope, (float) cast, double / long double return types, k and len as one array, for-init forms, while and
  do-while loops, `(0, e->count)`, match_permute (688 compiles, no gain). 0x00488320 is a same-TU static
  (EAX = cloud, EDX = &k); the file carries a stand-in written from its body, with no MATCH line.
- **0x00414c30, 50 B:** two store-scheduling spots: own[0] = c->x stored late and o.z loaded late in the
  original (0x00414d05..0x00414d39, 0x00414e0c..0x00414e28). All 24 store orders per block, V2 / int typed
  copies, comma rvalues, double temps, dblgreedy (153 variants): no gain.
- **0x00414570, 156 B:** `dist` gets bestd's frame slot (S+0x10, the original S+0x20), and out[1] stays in
  ST0 (`fst`) where the original stores and reloads it (0x004147f1..0x004147fe), 3 B shorter, so every later
  jump shifts. Block scope for dist / bestd, `d` reused, dv as float[3], V2 out / tgt, dblgreedy (166): no gain.
- **0x0040c690, 638 B:** one hoisted load. The original loads `big` into EDI before the `jne` in both
  threshold predecessors (0x0040c6f4, 0x0040c716); this source loads it in each successor, one extra 4 B
  `mov`, so the symbol is 878 B against 874 B and the byte compare shifts from +0x2f. Instruction match is
  98.6% ops. Ternaries, goto forms, if-chain, 6 case orders, an `__inline` helper: no gain.
- **0x00489500, 734 B:** the original has an EBP frame with `and esp, -8` (0x00489500..0x00489506) and this
  source does not, so EBP becomes a fifth pointer register and every stack operand differs. The x87 body of
  the three spline components already compiles to the original sequence. What makes cl 13 align this frame
  is an open question: no double local is visible in the body, and 0x00414c30 also aligns without this slice
  finding the trigger (changing the V4 / Box6 sizes kept the alignment).

## Handoff: rows not worked (2)

| rva | subsystem | raw g2s | plate |
|---|---|---|---|
| 0x0040ca00 | gameplay | 10.0% | `re/analysis/bucket_gameplay_0040ad90_00412010/0x0040ca00.md` |
| 0x004128f0 | hud | 6.8% | `re/analysis/bucket_gameplay_00412100_00418e50/0x004128f0.md` (g2s: `in_*` / `unaff_*` register values) |

Next slice, cheapest first: 0x00488e70 (7 B, one fstp), then 0x00414c30 (50 B), then the frame-alignment
question that blocks 0x00489500 (answering it also explains 0x00414c30's prologue).

## Files

| file | what |
|---|---|
| `results.tsv` | per-row before/after, verdict, residual, approximate compile count, sha256 |
| `c4_batch.tsv`, `c4_dryrun.tsv` | promotion batch and gate dry run (1 PASS, 0 refused) |
| `relocheck.tsv` | relocheck of the closed source (67 checked, 0 unchecked) |

Raw data kept off the repo:

| path | SHA-256 |
|---|---|
| `D:/MashedWT/fresh3/decomp.json` (decomp JSON, 8 rows) | `3d1e027393124b5f27b2935df8d420876224ef8e6a2811dd6ff7c84161b05e14` |
| `D:/MashedWT/fresh3/g2s/results.tsv` (raw g2s scores, the "before" column) | `7be231fc95919d1e3ead792b21801e7785fe37089542d6943c2ae1b5e85531a9` |
| `D:/MashedWT/fresh3/var/`, `D:/MashedWT/fresh3/dg414570/`, `D:/MashedWT/fresh3/v*.py`, `D:/MashedWT/fresh3/tools/` | every variant compiled and the scratch helpers (not cited row by row) |
