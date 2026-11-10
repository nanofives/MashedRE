# RESULT MATCH-RACEPATH-X87A (race-path pool A, x87 lever), 2026-11-09

**Verdict: GREEN. 8 of 34 closed (24%), all 8 promoted to C4 by route match.** x87 6/27, regalloc 1/4, order 1/2,
stack 0/1.

Branch `orch/MATCH-RACEPATH-X87A`, base `orch/integration` @ 3fbf77214. Pool: `D:/MashedWT/MATCH-RACEPATH_data/poolA.tsv`
(34 rows, SHA-256 `e967e858286b732b7e0d39b4b83fd7ee3bc8c7586057c6ff0c792d8c6e2d1d44`). Every score uses
`re/tools/match/match.py` (XP DDK cl 13.00.9176 / c2 13.00.9178) with the file's own line-1 FLAGS, unchanged in every
row. No `__asm`, no naked body, no pragma. No mashedmod/ edit, no game run, no Ghidra write, no change to
`re/tools/match/` code. None of the 8 changed sources is referenced from `mashedmod/`, `scripts/` or `re/tools/`
(grep for each path: 0 hits), so the default gates are unaffected and were not run. Per-row before/after, miss type,
residual, compile count and source sha256: `results.tsv`.

## Yield

| miss type | closed | rows closed |
|---|---|---|
| x87 | 6/27 | 0x00412cf0 0x00464a50 0x004726f0 0x0048f780 0x00422570 0x00442a60 |
| regalloc | 1/4 | 0x004c1210 |
| order | 1/2 | 0x005a86a0 |
| stack | 0/1 | none |

| rva | subsystem | before | after | source |
|---|---|---|---|---|
| 0x004c1210 | third-party-library[renderware] | 1 B differ, 98.4% | 232 B BYTE-IDENTICAL | `re/match/rwl01/004c1210.c` |
| 0x005a86a0 | audio | 4 B, 98.5% | 176 B BYTE-IDENTICAL | `re/match/s25/005a86a0.c` |
| 0x00442a60 | util | 6 B, 99.4% | 531 B BYTE-IDENTICAL | `re/match/s08/00442a60.c` |
| 0x00412cf0 | hud | 7 B, 99.0% | 313 B BYTE-IDENTICAL | `re/match/s03/h2_00412cf0.c` |
| 0x00464a50 | audio | 7 B, 98.3% | 587 B BYTE-IDENTICAL | `re/match/s10/00464a50.c` |
| 0x004726f0 | gameplay | 7 B, 95.8% | 72 B BYTE-IDENTICAL | `re/match/s11/004726f0.c` |
| 0x0048f780 | particle | 7 B, 98.6% | 287 B BYTE-IDENTICAL | `re/match/s15/0048f780.c` |
| 0x00422570 | render | 16 B, 98.5% | 890 B BYTE-IDENTICAL | `re/match/s05/00422570.c` |

0x00464a50 scores insn 98.9% only because the Ghidra body is shorter than the compiled symbol (584 vs 587 B,
180/178 ins). The whole-symbol byte compare is identical, and `c4_reclass.py` accepts that case (Ghidra body short).

**Yield-rule check (stop under 30% after 12 rows worked).** In the order worked (x87 rows smallest residue first,
with the short non-x87 looks in between: 0x00477e60, 0x00414300, 0x0045c030, 0x004744a0, 0x00412cf0, 0x0047a320,
0x004c1210, 0x0055ded0, 0x00464a50, 0x00475f90, 0x004726f0, 0x00477f00) the first 12 closed 4 (33%), so the slice
continued. In the pool file's own order the first 12 rows closed 3 (25%). The lead can weigh which reading applies.

## What moved the closed rows

1. **Named double temporary (3: 0x00412cf0, 0x00464a50, 0x00422570).** `{ double t = a - b; x = (float)t; }` for one
   vector difference. The pilot's cast form `(float)(a - (double)b)` is not the same: on 0x00412cf0 it reaches 2 B
   (the fsub operand order flips), the named temporary closes it. On 0x00422570 one such statement fixed all 16 B,
   including the fld/fadd operand order of two later sums. The value is a float difference widened and narrowed
   again, so it is unchanged. Added to `tools/dblgreedy.py` as transform W, which found 0x00464a50 and 0x00422570 on
   its own.
2. **Named float local for one operand (1: 0x004726f0, partials 0x00477f00, 0x004818d0, 0x00467350, 0x00454350).**
   `float a = param_1[1];` read first schedules the fld ahead of the second pointer load. Transform F in the greedy
   tool (it reproduces the 0x004726f0 close from the original source).
3. **Comma rvalue (1: 0x00442a60).** The lead's lever from MATCH-RACEPATH-BIG applied to a call result:
   `(0, FUN_004c3ac0(&v.x)) * 0.8f`. The counter load then moves ahead of the fmul. Transform C/K in the greedy tool
   covers memory operands only, so this one was found by hand.
4. **Statement or store shape, no x87 cast (3: 0x0048f780, 0x005a86a0, 0x004c1210).** 0x0048f780: the fade stored as
   the product, first among the four field stores (no write-back to the dead param_4). 0x005a86a0: the pointer store
   written through its address expression `*(int *)(param_1 + 0x28)` and not the alias `piVar1`. 0x004c1210: the
   predecessor walk as an empty-body for over one pointer. Each keeps the C semantics: the same addresses get the
   same values, and no reader sees a different order (read for each row before keeping it).

Guard checks: no closed row and no kept partial introduces a `qword` operand (the closed rows are byte-identical,
the 8 partials show 0 `qword` lines in `match.py --show`).

## Not closed (26)

- x87, 21 rows. Partial gains kept under `variants/` and not adopted: 0x00475f90 6 -> 4 B, 0x004818d0 6 -> 4,
  0x00477f00 7 -> 5, 0x00426810 29 -> 21, 0x00467350 30 -> 18, 0x00454350 37 -> 28, 0x00448220 81 -> 47,
  0x004752f0 236 -> 233. No gain on 0x00477e60, 0x00414300, 0x00482ae0, 0x0045c030, 0x004744a0, 0x0041fcc0,
  0x00487020, 0x0041c410, 0x0041d930, 0x00420050, 0x004809e0, 0x00491340, 0x0041af50. Most remaining residues are
  one instruction (an integer load, an `add esp` or a `pop`) placed one slot off an x87 instruction, which no
  statement-level rewrite of the five forms moved.
- regalloc, 3: 0x0055ded0 (SIB base/index, 14 spellings), 0x00424eb0 (the dead count load into EAX before
  `push esi; call 0x00422fd0`: needs a callee-shape decision, not a rewrite), 0x00408610 (header look only).
- order, 1: 0x0047a320 (`add esi,0x218` hoisted above the copy loop, the loop pad follows it).
- stack, 1: 0x00429310 (header look only).
- 0x00487020 (MATCH-S14 source): MATCH-S14-FU was open and unclaimed, so it was worked. No gain.
- 0x00420050 and 0x0041fcc0 share `re/match/s05/00420050.c` with pool-B row 0x0041faf0. Neither closed, so no file
  under `re/match/racepath/` was needed.

**Size artefacts seen (pc_funcs body_size vs compiled symbol):** 0x00464a50 584/587 (closed, Ghidra body short),
0x00475f90 804/814, 0x0041c410 1294/1420, 0x00408610 1066/1073, 0x0041af50 998/994, 0x004752f0 997/1003,
0x0047a320 121/115 (the loop pad), 0x00448220 2184/1235. 0x00448220 needs an extent analysis (the FRAME route)
before any C4: its Ghidra body is 949 B longer than the symbol.

## Same-file and gate checks

- `re/match/rwl01/004c1210.c` also holds 0x004c0ad0: still 100%, 49 B BYTE-IDENTICAL, relocheck OK. The other 7 files
  hold one `// MATCH:` each.
- `relocheck.py` OK on all 8 (`relocheck.tsv`). Unchecked externs: `__ftol` (0x00412cf0), `__chkstk` + `__ftol`
  (0x00422570), the two semaphore imports (0x005a86a0), the same declarations as the base sources.
- `c4_reclass.py batch --tsv verify/matchracepath_20261109/x87a/c4_batch.tsv`: 8 PASS, 0 refused (`c4_dryrun.tsv`),
  then `--apply --result` this file.

## Files

| file | what |
|---|---|
| `results.tsv` | per-row before/after, miss type, verdict, residual, compiles (variant files kept), sha256 |
| `c4_batch.tsv`, `c4_dryrun.tsv` | promotion batch and gate dry run |
| `relocheck.tsv` | relocheck of the 8 changed sources |
| `variants/` | 8 improved but not closed sources (file name carries the byte count) |
| `tools/` | `tryv.py`, `dblgreedy.py` (scratch helpers copied from the pilot; dblgreedy gained transforms W, V, F, C, K, S; output dir via `MATCH_TRY_OUT`) |
