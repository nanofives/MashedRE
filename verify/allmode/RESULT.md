# D1 renderer A/B — ALL 16 shots with a MEASURED GameFlow mode (2026-08-16)

`DumpBackbufferBMP` now logs `CAPMODE tag=<file> mode=<GameFlow mode>` for every
capture. Instrumenting the SINK rather than each driver means no capture path can be
forgotten - which mattered immediately, see below.

| shot | delta | mode (measured) |
|---|---:|---|
| `race1/01_inrace_track.bmp` | 71.61% | InRace |
| `r6/round3_result.bmp` | 69.15% | InRace |
| `r6/round2_result.bmp` | 68.94% | InRace |
| `race1/01_action.bmp` | 21.69% | InRace |
| `r5/car_5_chase.bmp` | 0.92% | InRace |
| `r5/car_3_weave.bmp` | 0.64% | InRace |
| `r6/round2_go.bmp` | 0.03% | InRace |
| `r6/round1_go.bmp` | 0.03% | InRace |
| `r5/car_2_drive.bmp` | 0.03% | InRace |
| `r5/car_1_spawn.bmp` | 0.03% | InRace |
| `race1/01_grid.bmp` | 0.02% | InRace |
| `r6/round3_go.bmp` | 0.02% | InRace |
| `r6/round1_result.bmp` | 0.01% | InRace |
| `r5/car_4_chase.bmp` | 0.01% | InRace |
| `race1/02_back_to_menu.bmp` | 0.00% | Frontend |
| `race1/00_challengeselect.bmp` | 0.00% | Frontend |

```
InRace   n=14  max=71.61%  median=0.03%  <=1%: 10/14
Frontend n=2  max=0.00%
```

## The finding: `round*_result` are InRace, NOT Results

This overturns the reasoning of the previous two writeups. Those shots were assumed to be
`Results` frames and therefore excluded from the gate on the grounds that
`TrackRenderer::Render` does not run there. **Measured, all three are `InRace`** - the r6
driver captures them while GameFlow is still in the racing state, before the transition.

Consequences:

- They are **valid** for the renderer A/B after all, and they carry three of the four large
  divergences (~69%).
- The drawstream result still holds and now applies to them: every InRace frame submits the
  world at 13 batches, and these ARE InRace frames. So **the world is submitted for the very
  frames that come out saturated orange.**
- The earlier "the result screen never re-renders the 3D world" measurement remains true
  about the GameFlow `Results` state - it simply does not describe these captures.

## Where that leaves the divergence

Well scoped for the first time: on certain in-race frames (later in a race, after round 1),
the D3D9 path **submits the world normally and still produces a saturated orange frame**,
while librw on the identical frame renders the scene legibly. So it is a render-state or
draw-order problem, not a submission problem.

`MASHED_NO_PARTICLES=1` was the next probe - particles draw after the world, and if librw
submits its world late it would paint over them. That run did not reach the race (only the
menu shot was produced), so the probe is **untested**, not negative.

