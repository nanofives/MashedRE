# A/B capture report: pre-addback vs post-addback

- arm A (`pre-addback`): `verify\d1_recheck_20260818\arm_a`
- arm B (`post-addback`): `verify\d07_batch1_20260818\arm_a`
- shots compared: **16**
- threshold: 16 (diff% = pixels over threshold, the figure quoted in verify/)
- **worst: `r5/car_1_spawn` at 0.00%**
- at or under 0.5%: 16 of 16

| shot | diff% | mean | R | G | B |
|---|---:|---:|---:|---:|---:|
| `r5/car_1_spawn` | 0.00 | 0.00 | 0.00 | 0.00 | 0.00 |
| `r5/car_2_drive` | 0.00 | 0.00 | 0.00 | 0.00 | 0.00 |
| `r5/car_3_weave` | 0.00 | 0.00 | 0.00 | 0.00 | 0.00 |
| `r5/car_4_chase` | 0.00 | 0.00 | 0.00 | 0.00 | 0.00 |
| `r5/car_5_chase` | 0.00 | 0.00 | 0.00 | 0.00 | 0.00 |
| `r6/round1_go` | 0.00 | 0.00 | 0.00 | 0.00 | 0.00 |
| `r6/round1_result` | 0.00 | 0.00 | 0.00 | 0.00 | 0.00 |
| `r6/round2_go` | 0.00 | 0.00 | 0.00 | 0.00 | 0.00 |
| `r6/round2_result` | 0.00 | 0.00 | 0.00 | 0.00 | 0.00 |
| `r6/round3_go` | 0.00 | 0.00 | 0.00 | 0.00 | 0.00 |
| `r6/round3_result` | 0.00 | 0.00 | 0.00 | 0.00 | 0.00 |
| `race1/00_challengeselect` | 0.00 | 0.18 | 0.18 | 0.17 | 0.19 |
| `race1/01_action` | 0.00 | 0.00 | 0.00 | 0.00 | 0.00 |
| `race1/01_grid` | 0.00 | 0.00 | 0.00 | 0.00 | 0.00 |
| `race1/01_inrace_track` | 0.00 | 0.00 | 0.00 | 0.00 | 0.00 |
| `race1/02_back_to_menu` | 0.00 | 0.16 | 0.16 | 0.15 | 0.16 |

Reminder: all pre-2026-08-16 `verify/` stills are horizontally mirrored relative to current output. Diffing against them reads 30-45% and means nothing.

Pixel percentages localize a divergence. They do not accept a change: acceptance is `drawlist_diff.py` per `re/analysis/parity_tooling.md`.
