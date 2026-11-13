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

**Stale inputs, filtered (second commit).** The review's composition predates later merges: rescoring the 22
provisional band-A sources with `match.py` showed 10 already 100% under their file's `// TOOLCHAIN`, and hooks.csv on
`orch/integration` a616fcc3b has **20 of the 163 rows at C4 already** (12 in band A). Two more (0x004d1e30, 0x00534b60)
are closed at 100% on `orch/MATCH-RWC-NEAR-C`, and 0x004f1fb0 is being shaped on `orch/MATCH-RWC-NEAR-D`. Those 23 rows
carry `status = skip: ...` and rank `-` at the end of the TSV. The 140 `open` rows are ranked 1..140.
Columns added: `hooks_conf` (hooks.csv on orch/integration a616fcc3b), `status`.

| band | open, race path | open, menu only | open total | skipped |
|---|---|---|---|---|
| A95 | 0 | 1 | 1 | 3 |
| A80 | 10 | 2 | 12 | 12 |
| B50 | 34 | 18 | 52 | 6 |
| B<50 | 43 | 24 | 67 | 1 |
| B-cf | 7 | 1 | 8 | 1 |
| total | 94 | 46 | 140 | 23 |

MATCH-RWT-A takes the 13 open A95 + A80 rows (ranks 1-10 and 95-97). MATCH-RWT-B takes the open B-cf and B50 rows,
race path first (ranks 11-33 B50 race path, then the B-cf rows), in rank order.
