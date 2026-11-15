# RESULT RW-TAIL-RECOUNT: recount of the RW executed tail (L-144) on the merged head

Row kind: text/static. No build, no gates, no game run, no knob boot. Head: `198109d503ca30ff0e5cedfb75c1a9799731930a` (orch/integration, the merged head of run 46).

## Method
- Set: the 176 RVAs of `verify/match_20261112/RWT_TAIL_CENSUS.tsv` (membership fixed by MATCH-RWT-REFRESH: 163 ranked RWT rows + 12 hand bodies + 0x004c7a70; decision L-144 text, `log/orch/DECISIONS.md` line 204). Membership is not recomputed. 176 rows, 176 unique, all 176 have a hooks.csv row.
- Resolved = hooks.csv `confidence` is C4 (any route) or the RVA has a row in `re/match/ADMITTED.tsv`. Provisional bodies (`PROVISIONAL.tsv`) are not resolved (L-126, L-128).
- Tool: `re/tools/rw_tail_recount.py` (new, committed on this branch). Command:
  `py -3.12 re/tools/rw_tail_recount.py --out verify/hb_20261114/RW_TAIL_RECOUNT.tsv`
- Per-function table: `verify/hb_20261114/RW_TAIL_RECOUNT.tsv`.

## Count (measured)
| state | n |
|---|---|
| C4 by `c4=match` | 57 |
| C4 by `c4=frida-replay` (0x004c1a70, 0x004c4d20) | 2 |
| C4 other route (frida, 0x004c5010) | 1 |
| admitted EQUIV, not C4 (0x004c4a50, 0x00536e90) | 2 |
| **resolved** | **62** |
| **open (unresolved)** | **114** |

Strict L-144 reading (match or admitted EQUIV only, the 3 frida rows not counted): 59 resolved, 117 open.
Earlier figures: 136 open at 145dec4d0 (MATCH-RWT-REFRESH, 43 C4, 0 admitted), then 133, 129, 122, 118 as run-report arithmetic. The measured 114 is 4 below the run 45 report's 118 [UNCERTAIN: which 4 rows the report arithmetic missed. The per-run RANKED_MATCH-RWT-REFRESH.tsv tallies were never re-censused, so the difference is not attributed].

## Cross-check
Independent second pass in PowerShell (Import-Csv on hooks.csv and the two TSVs, no shared code): members 176, C4 60, admitted-not-C4 2, resolved 62, open 114, c4=match 57, same two admitted rows. Identical to the Python tool.

## Unresolved (114)
0x004c1de0 0x004c1f80 0x004c2620 0x004c2a30 0x004c30b0 0x004c32b0 0x004c39b0 0x004c3e90 0x004c4220 0x004c4360 0x004c4680 0x004c4eb0 0x004c51a0 0x004c7a70 0x004c9290 0x004ca160 0x004cab30 0x004cc050 0x004cc5e0 0x004cca80 0x004ccef0 0x004ccf00 0x004cdf20 0x004cf160 0x004cf5a0 0x004cfdd0 0x004cfdf0 0x004cff20 0x004d2340 0x004d3090 0x004d4380 0x004d4f90 0x004d5190 0x004d7de0 0x004d8300 0x004d8810 0x004dabd0 0x004daff0 0x004db3e0 0x004dc370 0x004dc410 0x004dc4a0 0x004dc8a0 0x004dcdc0 0x004dcf90 0x004dcff0 0x004dd050 0x004dd0b0 0x004ddcb0 0x004de1b0 0x004de390 0x004dee40 0x004df030 0x004df120 0x004e0920 0x004e13f0 0x004e1e70 0x004e2090 0x004e2650 0x004e2930 0x004e2bf0 0x004e3160 0x004e3800 0x004e3dc0 0x004e4650 0x004e5300 0x004e5850 0x004e6100 0x004e6ab0 0x004e7060 0x004e7420 0x004e8120 0x004e8a10 0x004e8c70 0x004e9330 0x004ea9a0 0x004f0f10 0x004f2570 0x004f3150 0x004f3440 0x004f36c0 0x004f3e90 0x004f57e0 0x004f6870 0x004f71f0 0x004fa5f0 0x0052daf0 0x00534d00 0x00535330 0x00536630 0x00536780 0x00536fe0 0x0053d0b0 0x0053d2b0 0x0053d820 0x0053e830 0x0053f3c0 0x0053fbb0 0x0053fec0 0x005405c0 0x005412d0 0x00543b30 0x005441f0 0x00545080 0x005451f0 0x005465c0 0x00546890 0x005492b0 0x00549970 0x0054f8d0 0x00550130 0x00557cb0 0x005cb2a0 0x005cb404 

## Limits
Not measured: whether each of the 176 is still executed (the set is the 2026-11-12 review's executed RW tail, `log/orch/rw_closure_review_20261112`). [UNCERTAIN: membership drift since 2026-11-12 needs a fresh reach.tsv census.]
