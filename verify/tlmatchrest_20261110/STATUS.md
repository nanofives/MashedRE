# TL-MATCH-REST interim status (paused 2026-11-10 for PARTICLE-3-SWAP)

BYTE-IDENTICAL + relocheck OK, not yet promoted (c4_reclass on resume):
- 0x00409680 re/match/tlr/00409680.c (20 B, relocheck 1/1), 0x00409710 re/match/tlr/00409710.c (119 B, 10/10): C4 rows
  that had no matched body.
- 0x00474f90 re/match/tlr/00474f90.c (32 B), 0x005aaaa0 re/match/tlr/005aaaa0.c (30 B): no hooks.csv row; c1_add first.
- 0x005addd0 re/match/s26/005addd0.c (49 B, relocheck 2/2): 88.2% -> identical by returning the node (`int *`).

Near misses: verify/tlmatchrest_20261110/scores.tsv (match.py + relocheck, 30 rows) and residual_kinds.tsv
(residual_kind.py). Field-only residuals for the admission list: 0x005ae920 SIB-only 96.8% (25 spellings tried),
0x005aa060 register-only 85.2%. New sources with structural residuals: 0x005ac770 79.3%, 0x005ac930 71.4%,
0x0047f7d0 76.5% (re/match/tlr/).

Not started: row-less 0x00440bc0 (1063 B), 0x00487180 (247 B), 0x004b4eb0 (384 B), 0x00562ac0 (1021 B); levers on the
structural rows.
