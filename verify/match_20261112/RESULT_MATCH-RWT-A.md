# RESULT MATCH-RWT-A: doubled RW match lane, executed RW tail (2026-11-12)

Row kind: match-only (no build, no gates, no game run). Session on claude3, Opus 5.5.

## Ranked list (for MATCH-RWT-B)

Full list: `verify/match_20261112/RANKED_MATCH-RWT.tsv` (163 rows), generator `rank_match_rwt.py`
(inputs: `log/orch/RW_CLOSURE_REVIEW_20261112.md` row C and its evidence folder `log/orch/rw_closure_review_20261112/`,
`RwCore_Bodies.tsv` from `orch/RWC-PLUGIN-ATTACH` @ e5b357042, `verify/equivcheck_20261112/measure_final.tsv`,
`re/coverage/reach.tsv`).

Set: the executed RW functions of row C, minus the 12 hand bodies and 0x004c7a70 (never measured):
114 provisional NOT-PROVEN (`measure_final.tsv`), 42 g2s NOT-PROVEN (`equiv_g2s_executed_rw_64.txt` minus 0x004c3d90,
which the review puts in class A by its 1-minute fix), 7 g2s compile failures. Every row is executed (reach.tsv).

Order: race path first (reached_race_load or reached_race_run = 1), then best_pct descending.
Columns: rank, rva, size, executed, race_path, stage, best_pct, class, source (`g2s:<dir>` = regenerate with
`decomp_pc.py --port --callees` + `g2s.py`, the review kept only the strict sources), band, equiv kind.

Bands: A95 (>= 95%), A80 (80 to 95%), B50 (50 to 80%), B<50, B-cf (compile failure or no size).

| band | race path | menu only | total | provisional only |
|---|---|---|---|---|
| A95 | 1 | 3 | 4 | 4 |
| A80 | 19 | 5 | 24 | 18 |
| B50 | 37 | 21 | 58 | 44 |
| B<50 | 44 | 24 | 68 | 46 |
| B-cf | 7 | 2 | 9 | 2 |

MATCH-RWT-A takes A95 + A80 (28 rows, 22 provisional + 6 g2s). MATCH-RWT-B takes from B50 down, rank order.
