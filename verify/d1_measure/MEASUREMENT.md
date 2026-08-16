# D1 measurement — D3D9 default vs librw (2026-08-15)

Taken with the R10b-fixed gate (16/16 byte-identical across same-build runs), so every
delta below is signal, not harness noise.

Recipe: identical env for both runs, differing ONLY in `MASHED_RENDER_LIBRW=1`.
`MASHED_DETERMINISTIC=1 MASHED_DET_FRAMES=3000 MASHED_RACE_DEMO=1 MASHED_GOTO=6
MASHED_DRIVE_HOLD=1 MASHED_DRIVE_DEMO=1`

| shot | pixels over threshold 16 |
|---|---:|
| `race1/01_inrace_track` | **71.61%** |
| `r6/round3_result` | **69.15%** |
| `r6/round2_result` | **68.94%** |
| `race1/01_action` | 21.69% |
| `r5/car_5_chase` | 0.92% |
| `r5/car_3_weave` | 0.64% |
| `r6/round2_go`, `r6/round1_go`, `r5/car_2_drive`, `r5/car_1_spawn` | 0.03% |
| `race1/01_grid`, `r6/round3_go` | 0.02% |
| `r6/round1_result`, `r5/car_4_chase` | 0.01% |
| `race1/02_back_to_menu`, `race1/00_challengeselect` | 0.00% |

## The finding: the divergence ACCUMULATES

- `round1_result` 0.01% -> `round2_result` 68.94% -> `round3_result` 69.15%
- `01_grid` (early race) 0.02% -> `01_action` 21.69% -> `01_inrace_track` (late) 71.61%

12 of 16 shots are at or near parity, **including the first round and the start of the
race**. The large deltas are all *later* in a run. That is the signature of state that is
not reset between rounds or that drifts during a race — not a static difference in how the
two renderers shade a frame.

## What this does NOT establish

**Which renderer is faithful.** Both were compared to each other, not to the original.
An earlier reading of these images as "D3D9 is blown out, librw is cleaner" was wrong as a
general claim: `r5/car_2_drive` is visually identical between the two paths
(`d1_r5_d3d9.png` vs `d1_r5_librw.png`). The `01_inrace_track` pair does show a heavy
yellow haze on the D3D9 side that librw lacks (`d1_cmp_*.png`) — plausibly a fog term,
given the D-S3 fog-colour findings — but which of the two matches MASHED.exe is unknown
from these captures alone.

Resolving that needs an original-side capture at the same pose. Both halves of that
now exist as of today: `MASHED_CAM_POSE` (standalone consumes a pose) and the d3d9 shim's
`draw3d.json` counters (original reports its per-frame submission).

## Consequence for D1

**Do not invert `MASHED_RENDER_LIBRW` yet.** Inverting now would make the shipping default
a renderer whose output drifts across rounds. Find the accumulating state first; the
round1-vs-round2 boundary is a clean bisection point.
