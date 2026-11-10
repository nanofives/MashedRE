# MATCH-NEARMISS-2 pool (step 1, measure), 2026-11-09

Base `orch/integration` @ bba25cb03. `build_pool.py` re-scores every row live with `match.py --show --bytes`
(XP DDK cl 13.00.9176 / c2 13.00.9178, the file's line-1 FLAGS). Live score equals the ledger score on all 124 rows.

**Pool:** `re/analysis/match_ledger.tsv` REFUSED rows at 95.0% to below 100% whose hooks.csv confidence is not C4,
minus CRT / Lua / qhull (L-95). None of the three excluded families had a row in this band. 124 rows:
111 C2, 12 C3, 1 with no hooks.csv row. No source uses `__asm` / `naked`. Differing bytes: min 1, median 8, max 398.

95 of the 124 were already in the C4R-NEARMISS bounded permute (2026-11-07, `match_permute.py`, 29,930 compiles):
0 became identical, 1 improved. 29 rows are new since then (later MATCH slices, RW library slices).

## Miss types (mechanical, from the instruction diff; definitions in `build_pool.py`)

| type | rows | permuted 11-07 (0 closed) | new | diff instrs median | read |
|---|---|---|---|---|---|
| order (same instructions, one placed elsewhere) | 41 | 30 | 11 | 2 | scheduler placement of one load/store; permute moved none |
| x87 (fst/fxch vs fld/fstp, stack order) | 30 | 30 | 0 | 6 | C4R-NEARMISS: the flags brief lists this class as not moved by rewrites |
| count (instruction count differs) | 13 | 8 | 5 | 5 | missing/extra code or a size artefact |
| other+regalloc | 11 | 7 | 4 | 10 | mixed |
| regalloc (same ops, different registers) | 9 | 4 | 5 | 2 | |
| other | 7 | 7 | 0 | 4 | mixed opcode differences |
| stack (esp/ebp displacement only) | 5 | 3 | 2 | 4 | local ordering / frame size |
| const / const+* | 8 | 6 | 2 | 4-14 | |

## Predicted yield

No type has evidence of 30% or more. The evidence there is: 0 of 95 closed under the bounded permute, and every row is
the residue a MATCH slice worker already iterated on and refused. The x87 class (30) and the permuted rows of every
type are predicted near 0%. The only untested lever is a hand rewrite by a worker on rows that never went through the
permute (29 new rows) and the small-diff count rows that may be size artefacts. Predicted yield for that set is not
measurable from the tables alone, so step 2 runs one bounded pilot (20 rows) to measure it before any batch:

- order, new, 2 diff instrs: 0x0057c0d0 0x005a8390 0x005a86a0 0x005a8560 0x005ab560 0x005ac7e0 0x004e4450 0x005735f0 0x005a7ee0 0x005bf8a0
- regalloc, new: 0x00540340 0x004c1040 0x004c1210 0x0055ded0
- count, 1-3 diff instrs: 0x004967e0 0x00402b70 0x005a9c50 0x005b0df0
- const, new: 0x005af070 0x005c4ad0

A batch over the rest of a type follows only when the pilot's closure rate for that type is 30% or more.
