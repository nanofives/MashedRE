# RESULT MATCH-RWT-REFRESH: RWT ranked list and executed RW tail census (2026-11-13)

Row kind: match-only/text. No build, no gates, no game run. Base: orch/integration 145dec4d0.

## Census (executed RW tail = 176)
Set: the 163 rows of `RANKED_MATCH-RWT.tsv` + 12 hand bodies + 0x004c7a70 = 176 (the review's row C).
Generator: `verify/match_20261112/rwt_refresh.py` (reads hooks.csv, re/match/ADMITTED.tsv, re/match/PROVISIONAL.tsv, the review composition).
Per-function table: `verify/match_20261112/RWT_TAIL_CENSUS.tsv`.

| state (hooks.csv + ADMITTED.tsv on 145dec4d0) | n |
|---|---|
| C4 by byte match (`c4=match`) | 40 |
| C4 by other route (2x `frida-replay` 0x004c1a70 0x004c4d20, 1x frida 0x004c5010) | 3 |
| admitted EQUIV (ADMITTED.tsv) | 0 |
| open | 133 |

**RW tail remaining (L-144 strict: match or admitted EQUIV): 136 of 176** (133 if the 3 non-match C4 rows count as closed).
Open 133 = 121 ranked + 11 hand bodies + 0x004c7a70 (never measured, unranked).
Previous census: 163 ranked, 20 to 23 already C4; now 43 of 176 are C4.

## Ranked list
`verify/match_20261112/RANKED_MATCH-RWT-REFRESH.tsv` (121 open ranked rows, same order rule: race path first, then best_pct).
Rows C4 on hooks.csv dropped out. The 13 hand/unmeasured rows are in the census table only.
