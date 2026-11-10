# RESULT MATCH-X87-CASTLEVER-PILOT, 2026-11-10

Parent row MATCH-X87-CASTLEVER (lead MATCH-RACEPATH). Pilot: re-run parked x87 near-misses with the race-path
levers (RESULT_MATCH-RACEPATH.md "Levers found" / "The cast lever"). Branch `orch/MATCH-X87-CASTLEVER-PILOT` from 1cbf98965.

## Pool (written before any edit)

`build_pool.py` unions C4R-NEARMISS non-closed rows (results.tsv), MATCH-NEARMISS-2 POOL.tsv `miss_type=x87`, and
X87B results.tsv `worked=auto, miss_type=x87`. Excluded: the 8 MATCH-RACEPATH-NEAR rows, every row C4 in this base's
hooks.csv or in `orch/integration:hooks.csv` (`c4_integration.txt`), every row already worked in another race-path slice
(X87A, BIG, BIG2, FRESH*, SHIFT, PILOT, FRAME; `racepath_worked.tsv`), and 0x0041f590 (no single source path).
105 rows -> 68 candidates (`pool_all.tsv`). Gate dry run: `c4_reclass.py batch --tsv pool_dryrun_in.tsv`: 0 pass, 68 refused
(`pool_dryrun.txt`). `classify.py` re-measured each row (match.py --only --bytes) and counted the differing instruction
lines that are x87 (`pool_classified.tsv`): 38 rows have no x87 line in the diff (regalloc / const / count residue), 30 have one or more.

Pilot rows (x87 share of the differing lines >= 0.5, smallest residue first; class read from the diff):

| rva | bytes bad | source pool | residue class (before any edit) |
|---|---|---|---|
| 0x00473c20 | 2 | matchnearmiss2 | spill slot: `fst [esp+8]` ours vs `[esp+0x14]` original (frame slot of an x87 temp) |
| 0x00555af0 | 2 | matchnearmiss2 | spill slot: int->float `fild` temp at `[esp+0x50]` vs `[esp+0x5c]` |
| 0x0040ad90 | 3 | c4rnearmiss | operand order: `fmul st(1)` placed one instruction apart |
| 0x00458e00 | 3 | c4rnearmiss | operand order: `fld [ecx]` issued in a different position |
| 0x004307a0 | 5 | matchnearmiss2 | operand order: `fcomp [esp+8]` vs `[esp+4]` (compare operand swapped) |
| 0x00455150 | 5 | matchnearmiss2 | spill placement: `fstp [esp+0x28]` vs `[esp+0x2c]` at a different point |
| 0x00405920 | 7 | matchnearmiss2 | operand order: `fld [esp+0x14]` / `[esp+0x10]` swapped |
| 0x0044e0a0 | 7 | c4rnearmiss | spill placement: `fstp [esp+0x30]` one step apart |
| 0x004785e0 | 7 | c4rnearmiss | operand order: `fld st(0)` duplicate placed differently |
| 0x0041c180 | 8 | matchnearmiss2 | operand order: frame `fld` vs `fmul [const]` order |
| 0x00414f00 | 18 | matchnearmiss2 | operand order: `fld [esi]; fsub [eax+0x30]` ours vs `fld [eax+0x30]; fsubr [esi]` original |

The class column above was read before any edit. After the attempts, the residues split into four kinds (`results.tsv` miss_type):
x87-operand-order (one memory operand loaded in the other order, or fsub/fsubr), x87-spill-placement (an fstp
issued at a different point), x87-schedule (the x87 op lands one slot before/after an integer op: pop, push,
add esp, mov/test), and x87-spill-slot (the same temp in a different dead frame or param slot).

## Yield: 4 / 11 closed (36%)

| rva | before B | after B | verdict | lever |
|---|---|---|---|---|
| 0x0040ad90 | 3 | 0 | CLOSED | 3, named float local (dblgreedy F) |
| 0x00458e00 | 3 | 0 | CLOSED | 3, named float local (dblgreedy F) |
| 0x0044e0a0 | 7 | 0 | CLOSED | 2, named double temporary (dblgreedy W) |
| 0x00414f00 | 18 | 0 | CLOSED | 4, comma rvalue on x and z, by hand |
| 0x00473c20 | 2 | 2 | NOT-CLOSED | spill slot, scope / declaration-order forms do not move it |
| 0x00555af0 | 2 | 2 | NOT-CLOSED | spill slot, cl keeps its own int temp |
| 0x004307a0 | 5 | 5 | NOT-CLOSED | schedule (fcomp vs pop esi) |
| 0x00455150 | 5 | 5 | NOT-CLOSED | schedule (fstp vs push) |
| 0x00405920 | 7 | 7 | NOT-CLOSED | schedule (fld vs add esp,4) |
| 0x004785e0 | 7 | 7 | NOT-CLOSED | schedule (fld st(0) vs mov/test of norm) |
| 0x0041c180 | 8 | 8 | NOT-CLOSED | schedule (fld/fmul vs push ecx) |

By class: operand-order 3/3, spill-placement 1/1, schedule 0/5, spill-slot 0/2. None of the 7 open rows got a single byte
from any lever: each variant either kept the residue or made it worse. Per-variant detail is in `results.tsv`, and the
variant specs are in `variants/v_<rva>.py` (run with `tools/tryv.py`). The greedy log is `dblgreedy.log`.

Per lever:
- 1, double intermediate `(float)(a + (double)b)`: closed none. It made 0x004307a0, 0x00405920 (named double), 0x0041c180 and 0x00414f00 no better or worse.
- 2, named double temporary: closed 0x0044e0a0.
- 3, named float local: closed 0x0040ad90, 0x00458e00.
- 4, comma rvalue: closed 0x00414f00 (both x and z are needed: x alone 9 B).
- 5, embedded assignment / length change: no pilot row had a length change.
- `_DAT_` -> `__real@` literal: no pilot row had a const residue. 0x00414f00 still declares 4 `_DAT_005cc...` floats, which relocheck leaves unchecked (OK 10/4), as before.

Semantics: each closing edit keeps value and type. A float field read into a float local holds the same float. The
named double temporary narrows the same float product back to float, and the 300.0f operand stays `fmul dword`, so there
is no double literal and no false gain. `(0, x)` yields x. No permuter run (mulswap precedence bug, HARNESS_BACKLOG item 7), so there were no
mulswap variants to screen.

Checks on the 4 closed rows: `match.py --bytes` BYTE-IDENTICAL, `relocheck.py` OK (`relocheck.tsv`), symbol end against the
next pc_funcs entry: 0x0040ae24 <= 0x0040ae30, 0x00415012 <= 0x00415020, 0x0044e18f <= 0x0044e190, 0x00458f12 <= 0x00458f20.
Each closed source holds one `// MATCH:`, and no file under `mashedmod/src` includes any pilot source (`grep -rln`), so
the port side is untouched. `s01/00405920.c` holds 6 MATCH rows and was not edited.
Gate: `c4_reclass.py batch --tsv c4_batch.tsv`: 4 pass (`c4_dryrun.txt`), then `--apply --result` (route match).

Tool note: dblgreedy's operand regex (`name[..]`, `->f`, `.f`) does not match `*(float *)(base + off)`, so it never
offered the comma form that closed 0x00414f00. Running the levers by hand on cast-pointer operands is worth it for the rest of the pool.

## Rest of the pool: 57 rows, predicted yield about 10% overall

- 38 of the 57 have no x87 instruction in their diff (regalloc, const, count, or frame residue). The cast levers do not
  apply: predicted 0%.
- 19 carry x87 lines. Read from the first diffs (`pool_classified.tsv`):
  - operand-order / spill-placement look (6): 0x00557110 12 B (field fld vs fcomp const), 0x0041e6c0 15 B (fstp placement),
    0x00453730 24 B (fld field / fadd frame), 0x0045a190 64 B (fsub operands, plus an add/sub encoding), 0x00405ab0 42 B and
    0x00457100 100 B (fld / fmul const order). The pilot closed 4/4 of this class: predicted about 50-60%, so 3-4 rows.
  - schedule / spill-slot look (6): 0x004910c0, 0x0041ce00 (same shape as 0x0041c180), 0x0042d5a0, 0x00431240, 0x004064c0,
    0x00495870. The pilot closed 0/7 of this class: predicted under 10%.
  - mixed, large, x87 is a minority of the diff (7): 0x00556e90, 0x005acda0, 0x00421d20, 0x0048d360, 0x0048d540, 0x0047ba20,
    0x0045b390. Frame or control-flow residue first. Predicted under 10% inside one bounded attempt.
- Recommendation: a follow-up on the 6 operand-order rows clears the 30% rule (predicted 50-60%). The other 51 rows do not
  (predicted under 10%) and stay parked.
