# RESULT MATCH-NEARMISS-2 (roadmap run 35, M5), 2026-11-09

**Verdict: GREEN, lane closed. 10 near-miss rows C2 -> C4 by route match (pilot 9, batch 1). The rest of the pool is
below the 30% batch threshold and stays parked.**

Branch `orch/MATCH-NEARMISS-2`, base `orch/integration` @ bba25cb03. Compiler for every score: XP DDK cl 13.00.9176 /
c2 13.00.9178 (`tools/vc7ddk`), each file's line-1 FLAGS, unchanged in every row. No `__asm`, no naked, no pragma used
to force bytes. No mashedmod/ edit: none of the edited sources is `#include`d by the port (grep over mashedmod/, the two
hits slice B found are comments), so the default gates are not affected and were not run. No game run, no Ghidra write.

## 1. Pool (measure)

`POOL.tsv`, `POOL_SUMMARY.md`, `build_pool.py`. 124 rows at 95% to below 100% and not C4 (111 C2, 12 C3, 1 with no
hooks.csv row). No CRT, Lua or qhull row is in the band. Live score = ledger score on all 124. Miss types (mechanical,
from the instruction diff): order 41, x87 30, count 13, other+regalloc 11, regalloc 9, other 7, stack 5, const* 8.
95 of the 124 were in the C4R-NEARMISS bounded permute on 2026-11-07, which closed 0.

## 2. Pilot (yield measurement), 20 rows never permuted

`pilot/RESULT_MATCH-NEARMISS-2-pilot.md`. 9 of 20 closed (45%), promoted 9/9 by `c4_reclass.py batch --apply`.

| type | closed |
|---|---|
| order | 6/10 |
| count | 2/4 |
| const | 1/2 |
| regalloc | 0/4 |

All 6 order closures are one family: RW list-link fields written through a typed `{next, prev}` link addressed from
the owner pointer. The other 3 were source errors: callee arities swapped (0x005b0df0), param_6/param_7 stored into
swapped fields (0x005af070), and an index-form join loop (0x00402b70).

## 3. Batch over the types at 30% or more (order, count, const), 46 rows

| slice | rows | closed | result |
|---|---|---|---|
| A | 23 | 0 | `slice_A/RESULT_MATCH-NEARMISS-2-A.md` (RED) |
| B | 23 | 2 | `slice_B/RESULT_MATCH-NEARMISS-2-B.md` (0x0047c0f0, 0x00550da0) |

Batch yield 2/46 = 4%. The pilot rate did not carry over. The pilot rows were the ones that never went through the
permute and that the list-link rewrite fitted. The batch rows had all been permuted and almost none had that pattern.

Promoted here: **0x0047c0f0** (track, C2 -> C4, `re/match/s13/0047c0f0.c`, 111 B BYTE-IDENTICAL, relocheck OK),
batch file `c4_batch_slices.tsv`.

Not promoted: **0x00550da0** (`re/match/s24/00550da0.c`, 369 B BYTE-IDENTICAL, relocheck OK). The gate refuses it
only because hooks.csv has no row for 0x00550da0. Adding a row needs a subsystem and the pc_funcs / master path
C4R-MATCHBATCH-4 used for its 5 created functions, and that is outside this lane's scope. Follow-up: add the row
(subsystem from the RW range classification), then `c4_reclass.py batch` on the same source.

Source fix with no C-level change: **0x0041f8f0** (already C4, `re/match/s05/00420050.c`). The float was passed to
`FUN_00426070` instead of as the 4th argument of `FUN_00422570`. Both readings compile to the same bytes. The fix
follows the callees' own prototypes. Still BYTE-IDENTICAL, relocheck OK. The byte compare cannot see this class of
error, so a pool-wide check of callee arity against the callees' matched sources is worth one sweep over the identical
sources.

## 4. What stays parked (predicted yield below 30%)

- 44 batch rows NOT-CLOSED (slice results list best score and residual per row). 3 improved variants kept under
  `slice_A/variants/` and not adopted, because none closed: 0x004627f0 61 -> 8 bad bytes, 0x0052fdb0 33 -> 25, 0x004c5f60 7 -> 6.
- Types never batched: x87 30, other+regalloc 11, other 7, regalloc 5 (0/4 in the pilot), stack 5. 0x0042f530 is
  PORT-INCLUDED (slice A worked only on a copy of it, not closed).
- Pilot NOT-CLOSED: 11 rows (`pilot/results.tsv`).
- Leads outside the pool from the pilot: 0x005af690 (same split `add esp` as 0x005b0df0), 0x005a8060 (same list as
  0x005a8390).

## Files

| file | what |
|---|---|
| `POOL.tsv`, `POOL_SUMMARY.md`, `build_pool.py` | step 1 pool and classifier |
| `slice_A.tsv`, `slice_B.tsv` | batch slice lists |
| `pilot/` | pilot results, hand log, relocheck, c4 batch + dry run |
| `slice_A/`, `slice_B/` | slice results, RESULT, variants, c4 dry run |
| `c4_batch_slices.tsv` | this transaction's batch file (0x0047c0f0) |
