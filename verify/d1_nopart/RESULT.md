# D1 — `MASHED_NO_PARTICLES=1` probe: the particle hypothesis is CONFIRMED, and it was hiding a second defect (2026-08-16)

> **CORRECTION, same day — read `verify/d1_fxcut/RESULT.md` first.** Finding 1 below stands.
> **Finding 2 is WRONG.** There is no sky-colour divergence between the two renderers. The
> orange I read as "librw's sky" is the *same D3D9 FX bloom*, still present in the librw run
> and merely surviving in the one region where librw's world submit does not overpaint it.
> Measured with FX cut on both sides, the renderer A/B is 0.38% on `01_inrace_track` and
> 0.01% on `01_action` — the shot Finding 2 was built on. Kept unedited below as the record
> of how the wrong reading was reached: the residual was compared against a librw capture
> that still had the defect in it.

The probe that failed to reach the race on 2026-08-15 was re-run and completed: 16/16 shots,
same CAPMODE distribution as the reference run (14 InRace, 2 Frontend, 0 Results —
`capmode.log`).

Recipe, identical to the `verify/allmode` A/B except for the added flag:

```
MASHED_DETERMINISTIC=1 MASHED_DET_FRAMES=3000 MASHED_RACE_DEMO=1 MASHED_GOTO=6
MASHED_DRIVE_HOLD=1 MASHED_DRIVE_DEMO=1 MASHED_WIN_POS=left-bl
MASHED_NO_PARTICLES=1 MASHED_VERIFY_OUT=verify/d1_nopart
```

D3D9 side (no `MASHED_RENDER_LIBRW`). Metric: `re/tools/imgdiff.py`, pixels over threshold 16.

## Where the two renderers actually draw

`RaceSubmit_Render` is **not** inside `TrackRenderer::Render`. It is called from
`exe_main.cpp:2794-2795`, after `g_track.Render()` has returned — so after every D3D9 pass,
including the particle pass at `TrackRenderer.cpp:4577-4602`. The D3D9 pass order is
sky → world → props → copters → cars → particles (the `ds_flush` tags at
`TrackRenderer.cpp:4107/4295/4345/4377/4567`; particles are **not** tallied into the
drawstream, which is why an orange particle wash was invisible to the drawstream
measurement that reported 13 world batches).

So librw's late submit paints over the D3D9 particles. That is the hypothesis, and it holds.

## Three-way table (all 16 shots)

`orig_AB` = the `verify/allmode` d3d9-vs-librw figure, reproduced unchanged.

| shot | orig_AB | nopart vs d3d9 | nopart vs librw |
|---|---:|---:|---:|
| `race1/01_inrace_track` | 71.61% | 87.83% | **16.67%** |
| `r6/round3_result` | 69.15% | 98.37% | **35.52%** |
| `r6/round2_result` | 68.94% | 98.57% | **35.25%** |
| `race1/01_action` | 21.69% | 99.40% | **77.72%** |
| `r5/car_5_chase` | 0.92% | 1.84% | 1.10% |
| `r5/car_3_weave` | 0.64% | 0.22% | 0.44% |
| `r6/round1_result` | 0.01% | 0.04% | 0.05% |
| `r5/car_1_spawn` | 0.03% | 0.01% | 0.01% |
| `r5/car_2_drive` | 0.03% | 0.01% | 0.02% |
| `r5/car_4_chase` | 0.01% | 0.01% | 0.02% |
| `r6/round1_go` | 0.03% | 0.01% | 0.03% |
| `r6/round2_go` | 0.03% | 0.03% | 0.04% |
| `r6/round3_go` | 0.02% | 0.00% | 0.02% |
| `race1/01_grid` | 0.02% | 0.01% | 0.01% |
| `race1/00_challengeselect` | 0.00% | 0.00% | 0.00% |
| `race1/02_back_to_menu` | 0.00% | 0.00% | 0.00% |

`01_grid` at 0.01% against a *different run* re-confirms the determinism gate: every number
above is signal.

## Finding 1 — the saturated orange IS the D3D9 particle pass

`nopart_01_inrace_track.png` shows the frame the drawstream said was there all along: the
world, props, the copter, all four cars, legible. Same camera, same scene as the librw shot.
With particles on, that frame is a screen-filling orange bloom.

The residual against librw is **spatially confined to the sky**. Region grid (8x6) for
`01_inrace_track`, nopart vs librw:

```
   0.0    2.7   36.7   69.0   76.3   88.3  102.1   99.5
   0.0    1.5   13.1   18.2    9.9   57.0   69.3   14.8
   0.0    0.0    0.0    0.0    0.0    3.1    1.5    0.2
   0.0    0.0    0.0    0.0    0.0    2.0    0.5    0.0
   0.0    0.0    0.0    0.2    0.2    0.0    0.0    0.0
   0.2    0.2    0.0    0.1    0.2    0.0    0.0    0.0
```

Rows 3-6 are zero. In the world region the two renderers are **pixel-identical** once the
particle pass is off. `round2_result` and `round3_result` have the same shape (top three
rows carry the delta, bottom half at ~0).

The particle bloom is a D3D9-side defect, not a librw one. It was already known as a
loose end — `TrackRenderer.cpp:4575-4576` calls the dust/snow bloom "a separate WS-E
tuning issue" — but it was not known to be the cause of the D1 divergence.

## Finding 2 — the bloom was MASKING a sky-colour divergence

`01_action` moved the wrong way: 21.69% with particles, **77.72%** without. Its region grid
is uniformly high, not sky-confined, because that shot's camera is pointed at mostly sky.

The two renders of `01_action` are the same geometry, same camera, same car, differing by a
global tint: librw renders the sky/void **orange**, D3D9 renders it **grey cloud**. With
particles on, the D3D9 orange bloom happened to approximate librw's orange sky, so the
measured delta was smaller. The 21.69% was two defects partially cancelling.

The sky delta is the same one behind the top-band residual in Finding 1. `TrackRenderer.cpp`
draws its sky clump at `:4072-4106` **ungated by `rw_world`**, so in a librw run both
renderers draw a sky and librw's wins by drawing last.

## What this does NOT establish

Which sky is faithful. Both renderers were compared to each other, never to the original —
the same limitation recorded on 2026-08-15. Deciding it needs an original-side capture at
the same pose (`MASHED_CAM_POSE` + the d3d9 shim's `draw3d.json`).

D1 inversion is still blocked, but the blocker is now named: it is not "an accumulating
divergence" of unknown origin. It is (a) a D3D9 particle bloom and (b) a sky-colour
disagreement, and (a) is on the path we would be inverting *away* from.
