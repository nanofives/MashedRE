# D1 renderer A/B restricted to MEASURED-InRace shots (2026-08-16)

## Why restrict

`TrackRenderer::Render` does not run during `Results` (verify/dsproof2/SETTLED.md), so a
renderer A/B is meaningless on a Results shot — neither renderer drew it. The gate can only
speak to frames where the renderer path actually executes.

## Why MEASURED, not inferred

Shots were previously classified InRace by FILENAME. That is the same inference habit behind
three wrong diagnoses of this divergence, so `exe_main.cpp`'s race-demo capture lambda now
writes `CAPMODE tag=<shot> mode=<GameFlow mode>` at capture time. Classification is now read
from the run, not guessed from the name.

Measured: `01_grid`, `01_action`, `01_inrace_track` = **InRace**;
`00_challengeselect`, `02_back_to_menu` = **Frontend**. (Filename inference happened to be
right for these five — but it is now checked rather than assumed.)

## Result

| shot | delta | mode |
|---|---:|---|
| `race1/01_inrace_track` | **71.61%** | InRace (measured) |
| `race1/01_action` | **21.69%** | InRace (measured) |
| `race1/01_grid` | 0.02% | InRace (measured) |
| `race1/00_challengeselect` | 0.00% | Frontend (measured) |
| `race1/02_back_to_menu` | 0.00% | Frontend (measured) |
| `r6/round*_result` | 68.94 - 69.15% | **unmeasured** |
| `r5/car_*`, `r6/round*_go` | 0.01 - 0.92% | **unmeasured** |

```
MEASURED-InRace : n=3   max=71.61%   median=21.69%   <=1%: 1/3
```

## This CORRECTS an earlier reading

An earlier pass classified 11 shots as InRace by filename and reported "9 of 11 at or below
1%, median 0.03%" — a reassuring picture. That reading does not survive: nine of those
eleven come from the r5/r6 driver, whose capture lambda is **not** instrumented, so their
GameFlow state is unknown and they cannot carry the conclusion.

Among shots that can actually be vouched for, the divergence rate is **2 of 3**, not 2 of 11.

## What this establishes

The D3D9-vs-librw divergence is **real and occurs on genuine in-race frames** where both
renderer paths execute. It is not an artefact of the Results path, and it is not confined to
result screens.

The temporal shape survives too: `01_grid` (race start) is at parity, `01_action` and
`01_inrace_track` (both later) diverge. Consistent with something that starts clean and
worsens during a race.

## Next

Instrument the r5/r6 capture lambda the same way. Eleven shots currently sit in the
unmeasured bucket, including the three ~69% `round*_result` ones — and if those turn out to
be `Results`, they should be dropped from the gate entirely rather than counted as renderer
divergence.
