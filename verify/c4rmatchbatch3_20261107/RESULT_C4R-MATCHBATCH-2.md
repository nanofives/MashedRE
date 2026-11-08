# RESULT C4R-MATCHBATCH-2 + C4R-SIZEFIX (2026-11-07)

**Verdict: GREEN.** 342 rows promoted to C4 by route match (L-93) through `c4_reclass.py batch --apply` (XP DDK cl 13.00.9176 / c2 13.00.9178, byte compare plus relocheck OK). 1 refused, 2,206 already C4. Text-only: no build, no game run, gates not run.

Branch `orch/C4R-MATCHBATCH-2` from `orch/integration` (25fad7e91).

## Commands
1. SIZEFIX: appended 5 hooks.csv rows (line-based, CRLF kept): `00550c30 00550d80 00551050 00551290 005512d0`, name `FUN_<rva>`, subsystem `hud` (the S24 neighbours), C2, status `new`, port `none`.
2. `py -3.12 verify/c4rmatchbatch3_20261107/build_ledger.py --jobs 8` -> `re/analysis/match_ledger.tsv` (3,313 RVAs, 2,548 pass), `batch.tsv` 2,549 rows. `00499740.c` (g2s_raw) still reports "compile failed (ddk)", as in the previous batch.
3. `c4_reclass.py batch --tsv batch.tsv --report verdicts.tsv --jobs 8` (dry run, `dryrun.log`).
4. `c4_reclass.py batch ... --apply --result <this file> --jobs 8 --date 2026-11-07` (`apply.log`): 342 pass, 1 refused, 2,206 skipped.
5. `scripts/backfill_port.py --allow-stale --build-dir ../D4-FLIP-ST5-LEG2/mashedmod/build`: hooks.csv already current (the C4R-PORTCOL build dir used before no longer exists). Existing malformed records 185/1206/3127/3137 left untouched by the tool, not mine.
6. tracker_lint before `pre-existing=3 NEW=0`, after `pre-existing=3 NEW=0`. (An intermediate run had NEW=5 NCOL from an unquoted comma in my first draft of the SIZEFIX notes; fixed before apply.)

## Promoted: 342
| source | rows |
|---|---|
| S25 | 81 |
| S26 | 89 |
| S27 | 62 |
| S28 | 79 |
| NEARMISS | 11 (0x00402750, 0x004c0ad0, 0x004d4bf0, 0x004d8280, 0x004d83d0, 0x004da430, 0x004da480, 0x004dab70, 0x00554010, 0x00554150, 0x0055c4f0) |
| previously unsized, now sized | 13 (0x004d8430/8470/8530/8550/86d0/8700/8730/8750/8b70/8fa0, 0x00551180, 0x005514e0, 0x00555910) |
| SIZEFIX | 5 (0x00550c30, 0x00550d80, 0x00551050, 0x00551290, 0x005512d0) |
| other (S17 1, S24 1) | 2 |

S28 is 79 here against 81 expected, and the 2 "other" rows are one S17 and one S24 RVA that were not yet C4. Sum 342.

## Refused: 1
- **0x0040ac80**: "whole symbol bytes equal but shorter than the original body (12/22 insns)". pc_funcs.csv sizes it 228 B (body 54 B plus jump table 80 B extent plus the four inlined handlers), and `c4_reclass` has no extent override. Per verify/c4rmatchpyrel32_20261107 it matches at its 80 B symbol. **Decision for the conductor:** either set its pc_funcs size to the 80 B symbol extent (a data change) or add an extent override to the gate. I changed neither.

## Not done on purpose
- **0x00553e80** already has a hooks.csv row (`third-party-library[renderware]`, C2, mapped). I added none (a duplicate was caught by c4_reclass and removed). It is not promoted: it is not in the batch as passing.
- Four HIDDEN case handlers of 0x0040ac80, inlined statics with no pc_funcs row, no rows added: **0x0040abc0, 0x0040abe0, 0x0040ac00, 0x0040ac40**.

## Ladder (hooks.csv, 6,001 -> 6,006 records)
| scope | C1 | C2 | C3 | C4 |
|---|---|---|---|---|
| overall | 10 -> 10 | 2779 -> 2540 | 319 -> 221 | 2861 -> **3203** |
| game | 0 -> 0 | 958 -> 720 | 201 -> 103 | 2214 -> **2555** |
| library | 10 -> 10 | 1821 -> 1820 | 118 -> 118 | 647 -> 648 |

(game C2 -243 includes the 5 new rows entering at C2 and leaving as C4: the net comes from 342 promoted minus the 1 library row.)

Files: `verify/c4rmatchbatch3_20261107/` (build_ledger.py, ladder_counts.py, batch.tsv, verdicts.tsv, dryrun.log, apply.log, build_ledger.log, backfill_port.log, lint_before/after.txt, ladder_before/after.json). Also edited: hooks.csv (line-based), re/analysis/CHANGELOG.md (+342), re/analysis/match_ledger.tsv.
