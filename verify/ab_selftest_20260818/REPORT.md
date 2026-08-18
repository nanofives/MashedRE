# A/B capture report: d3d9 vs librw

- arm A (`d3d9`): `verify\d1_mirrorfix\final_d3d9`
- arm B (`librw`): `verify\d1_mirrorfix\final_librw`
- shots compared: **16**
- threshold: 16 (diff% = pixels over threshold, the figure quoted in verify/)
- **worst: `r5/car_3_weave` at 1.01%**
- at or under 0.4%: 14 of 16

| shot | diff% | mean | R | G | B |
|---|---:|---:|---:|---:|---:|
| `r5/car_3_weave` | 1.01 | 0.52 | 0.54 | 0.59 | 0.41 |
| `race1/01_inrace_track` | 0.48 | 0.28 | 0.34 | 0.29 | 0.23 |
| `r5/car_5_chase` | 0.36 | 0.37 | 0.35 | 0.35 | 0.39 |
| `r6/round3_result` | 0.10 | 0.51 | 0.47 | 0.45 | 0.60 |
| `r6/round1_go` | 0.09 | 0.05 | 0.04 | 0.05 | 0.05 |
| `r6/round2_go` | 0.07 | 0.04 | 0.04 | 0.04 | 0.04 |
| `r5/car_1_spawn` | 0.06 | 0.04 | 0.04 | 0.04 | 0.04 |
| `r6/round2_result` | 0.06 | 0.31 | 0.27 | 0.26 | 0.39 |
| `race1/01_grid` | 0.05 | 0.04 | 0.04 | 0.04 | 0.04 |
| `r5/car_2_drive` | 0.05 | 0.04 | 0.04 | 0.04 | 0.04 |
| `r6/round3_go` | 0.05 | 0.04 | 0.04 | 0.04 | 0.03 |
| `r5/car_4_chase` | 0.01 | 0.02 | 0.02 | 0.02 | 0.02 |
| `r6/round1_result` | 0.01 | 0.02 | 0.02 | 0.02 | 0.02 |
| `race1/01_action` | 0.01 | 0.02 | 0.02 | 0.02 | 0.02 |
| `race1/00_challengeselect` | 0.00 | 0.00 | 0.00 | 0.00 | 0.00 |
| `race1/02_back_to_menu` | 0.00 | 0.00 | 0.00 | 0.00 | 0.00 |

Reminder: all pre-2026-08-16 `verify/` stills are horizontally mirrored relative to current output. Diffing against them reads 30-45% and means nothing.

Pixel percentages localize a divergence. They do not accept a change: acceptance is `drawlist_diff.py` per `re/analysis/parity_tooling.md`.
