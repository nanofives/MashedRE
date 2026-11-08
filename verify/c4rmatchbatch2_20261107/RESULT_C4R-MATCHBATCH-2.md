# RESULT C4R-MATCHBATCH-2 (2026-11-07)

**Verdict: GREEN.** 400 rows promoted to C4 by route match (L-93) through `re/tools/c4_reclass.py batch --apply`
(XP DDK cl 13.00.9176 / c2 13.00.9178, byte compare plus relocheck OK, recompiled at promotion time).
30 refused, 1,805 already C4 (skipped). Text-only step: no build, no game run, gates not run.

Branch: `orch/C4R-BATCH-M2` = `orch/C4R-BATCH-M` (488fddf5a) + `--no-ff` merges of MATCH-S17, S18, RWCORE-G5, S20, S21,
S22, S23, S24, in that order. All 8 merged with no conflicts.

## Commands (recipe of verify/c4rmatchbatch_20261107, section 1)

1. `py -3.12 verify/c4rmatchbatch2_20261107/build_ledger.py --jobs 8` -> `re/analysis/match_ledger.tsv` (2,771 RVAs, 2,205 pass), `batch.tsv` 2,235 rows.
   One g2s_raw source (`00499740.c`) reports "compile failed (ddk)" in the ledger log; its RVA is not in the refusals below (it falls under the pilot sources the batch already covers as C4 or has a better source).
2. `c4_reclass.py batch --tsv batch.tsv --report verdicts.tsv --jobs 8` (dry run, `dryrun.log`).
3. `c4_reclass.py batch --tsv batch.tsv --apply --result <this file> --jobs 8 --date 2026-11-07` (`apply.log`): 400 pass, 30 refused, 1,805 skipped.
4. `scripts/backfill_port.py --allow-stale --build-dir ../C4R-PORTCOL/mashedmod/build`: **0 rows changed** (`backfill_port.log`).
5. `tracker_lint.py --repo . --base orch/integration` before and after: `pre-existing=3 NEW=0` both times.

## Promoted: 400 (C2->C4 286, C3->C4 114)

By slice source: S17 34, S18 82, S20 77, S21 73, S22 35, S23 57, S24 42. RWCORE-G5 promoted 0 new rows
(nothing of it was newly identical and not yet C4). Library rows included by route match per L-93 (S17 libcmt, S23 libpng/zlib):
library C4 644 -> 647.

## Ladder before / after (`hooks.csv`, 6,001 csv records)

| scope | C1 | C2 | C3 | C4 | other |
|---|---|---|---|---|---|
| overall | 10 -> 10 | 3065 -> 2779 | 433 -> 319 | 2461 -> **2861** | 32 -> 32 |
| game | 0 -> 0 | 1241 -> 958 | 315 -> 201 | 1817 -> **2214** | 32 -> 32 |
| third-party library | 10 -> 10 | 1824 -> 1821 | 118 -> 118 | 644 -> 647 | 0 |

(Before = branch base orch/C4R-BATCH-M HEAD hooks.csv. Counts by the same `ladder_counts.py`.)

## Refused: 30

| RVA | reason |
|---|---|
| 0x00402750 | relocheck BAD, string literals with single backslashes compile `\t` to TAB (carry-over from MATCHBATCH) |
| 0x0040ac80 | whole symbol equal but shorter than the original body (12/22 insns) |
| 0x004c0ad0, 0x004d4bf0, 0x004d8280, 0x004d83d0, 0x004da430, 0x004da480, 0x004dab70, 0x00554010, 0x00554150, 0x0055c4f0 | not byte-identical: 1 differing unmasked byte each, insn 100.0% (recorded by insn score, not bytes) |
| 0x004d8430, 0x004d8470, 0x004d8530, 0x004d8550, 0x004d86d0, 0x004d8700, 0x004d8730, 0x004d8750, 0x004d8b70, 0x004d8fa0, 0x00551180, 0x005514e0, 0x00555910 | no Ghidra size in pc_funcs / hooks.csv, tool refuses to guess the original's length |
| 0x00550c30, 0x00550d80, 0x00551050, 0x00551290, 0x005512d0 | no hooks.csv row (add it first as C1 by identity) |

## Slice follow-ups left untouched

- S21: 10 no-size RVAs and S24: 8 no-size RVAs for `pc_funcs.csv` (these account for most of the "no Ghidra size" refusals above; they re-enter once sized).
- S24: hooks.csv row for held callee 0x00553e80.
- S18: stand-in callers in `004b6710/6b00/6b20/6b90.c` to drop at swap time.
- The 5 no-row RVAs above (0x00550c30 .. 0x005512d0) need hooks.csv rows.

## Files

`verify/c4rmatchbatch2_20261107/`: `build_ledger.py`, `ladder_counts.py`, `batch.tsv`, `verdicts.tsv`, `dryrun.log`,
`apply.log`, `build_ledger.log`, `backfill_port.log`, `lint_before.txt`, `lint_after.txt`, `ladder_before.json`, `ladder_after.json`.
Also rewritten: `hooks.csv` (400 lines, line-based), `re/analysis/CHANGELOG.md` (+400), `re/analysis/match_ledger.tsv`.
