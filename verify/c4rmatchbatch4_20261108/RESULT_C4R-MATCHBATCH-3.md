# RESULT C4R-MATCHBATCH-3 (2026-11-08)

**Verdict: GREEN.** 365 rows promoted to C4 by route match (L-93) through `c4_reclass.py batch --apply` (XP DDK cl 13.00.9176 / c2 13.00.9178, byte compare plus relocheck OK). 0 refused, 2,549 already C4. Text-only: no build, no game run, gates not run.

Branch `orch/C4R-MATCHBATCH-3` from `orch/integration` (add03069c).

## Source fixes (before / after, match.py --bytes + relocheck.py)
| RVA | change | before | after |
|---|---|---|---|
| 0x004e3470 | RpAtomicRegisterPlugin -> FUN_004e7d40 (bytes call 0x4e7d40 at +0x51, +0x170), RpClumpRegisterPlugin -> FUN_004e7d70 (+0x73) | 100%, byte-identical, relocheck OK checked=61 unchecked=3 | 100%, byte-identical, relocheck OK checked=64 unchecked=0 |
| 0x0057c270 | RpAtomicRegisterPlugin -> FUN_004e7d40 | OK checked=6 unchecked=1 | OK checked=7 unchecked=0 |
| 0x00562460 | RpWorldForAllWorldSectors -> FUN_004e5c70 (bytes call 0x4e5c70 at +0xd, +0x1a) | OK checked=2 unchecked=2 | OK checked=4 unchecked=0 |
| 0x005578a0 | already FUN_004e7d40 in the source, no edit needed | OK checked=16 unchecked=0 | unchanged |
| 0x0054e520 | 6.0f -> MGLOBAL float 0x005cd0a0 | BAD (literal, not 100%) | BAD, 1188 differing bytes (was 1178). **Refused, reverted.** |
| 0x0054eb90 | 6.0f -> MGLOBAL float 0x005cd0a0 | BAD (literal, not 100%) | BAD, 1469 differing bytes (was 1451). **Refused, reverted.** |

Call-target addresses read from original/MASHED.exe.unpatched by scanning E8 operands. The two 0x0054exxx sources are not 100% in either form (the h3 header already says the global read costs 10 points), so they are not in the promoted set and their files are unchanged.

## Promoted: 365
| slice | rows |
|---|---|
| RWL01 | 70 |
| RWL02 | 74 |
| RWL03 | 55 |
| RWL04 | 60 |
| RWL05 | 68 |
| RWL06 | 38 |

All 365 are RenderWare library rows (hooks.csv library scope). Nothing outside the rwl slices newly passed. The ledger now has 4,078 RVAs, 2,914 pass (was 2,548).

## Refused: 0
(Only candidates outside the batch: 0x0054e520, 0x0054eb90 above, plus g2s_raw/00499740.c "compile failed (ddk)" as before.)

## Ladder (hooks.csv, 6,047 rows, unchanged)
| scope | C1 | C2 | C3 | C4 |
|---|---|---|---|---|
| overall | 10 -> 10 | 2183 -> 1868 | 217 -> 167 | 3605 -> **3970** |
| game | 0 -> 0 | 718 -> 718 | 103 -> 103 | 2547 -> 2547 |
| library | 10 -> 10 | 1465 -> 1150 | 114 -> 64 | 1058 -> **1423** |

## Other
- `scripts/backfill_port.py --allow-stale --build-dir ../D4-FLIP-ST5-LEG2/mashedmod/build`: hooks.csv already current, 0 rows changed. Malformed records 1206/3127/3137 left untouched (not mine).
- tracker_lint before `pre-existing=3 NEW=0`, after `pre-existing=3 NEW=0`.
- hooks.csv written line-based by c4_reclass, no csv round-trip.
- Files: verify/c4rmatchbatch4_20261108/ (build_ledger.py, ladder_counts.py, batch.tsv, verdicts.tsv, dryrun.log, apply.log, build_ledger.log, backfill_port.log, lint_before/after.txt, ladder_before/after.json). Also edited: hooks.csv, re/analysis/CHANGELOG.md (+365), re/analysis/match_ledger.tsv, re/match/rwl02/004e3470.c, rwl05/00562460.c, rwl06/0057c270.c.
