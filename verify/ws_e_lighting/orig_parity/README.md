# WS-E vehicle lighting — acceptance vs ORIGINAL captures (2026-07-02)

Closes the merge gate left open by the WS-E vehicle-lighting session
(`re/prior_art/notes/rw_lighting_research_2026-07.md` §10): heading-matched
comparison of the standalone's `MASHED_RPLIGHT` relit path against the REAL
`MASHED.exe`, same track, same car, same headings. Every number below is
reproducible with `py -3.12 compare.py` in this directory.

## Provenance

| side | how |
|---|---|
| `orig_*.png` | original `MASHED.exe`, Arctic (track 3) TimeTrial solo, warped via `re/frida/capture_relight_parity.py` (scenario_launch plumbing; d3d9-shim `MASHED_ORIG_BBDUMP_REQ` backbuffer dumps — PrintWindow captures white when DWM isn't composing). Player car ground truth: `CreateFileA/W` log shows `VEHICLES\STALLION.PIZ` (+`DIRT.PIZ`) — the quick-launch car-0 is the **Stallion**, absent from the standalone catalogue until this branch. Headings sampled from the live record (`atan2(+0x9dc, +0x9d4)` on 0x008815a0) at dump time; full series in `shots.json`. |
| `sa_relit/` | standalone `mashed_re.exe` (this worktree, post-rebase build), race demo `MASHED_RACE_DEMO=1 MASHED_GOTO=6 MASHED_DEMO_DRIVE=1 MASHED_DRIVE_HOLD=1 MASHED_GAME_MODE=2 MASHED_RACE_MODE=laps MASHED_CAR_SEL=12` (Stallion), `MASHED_RPLIGHT` unset (relit path ON). Captures fire when the wrapped yaw enters ±0.05 rad windows around the original shots' headings (`RunRaceDemoStep`); logged as `RELIGHT_CAP` lines. |
| `sa_legacy/` | identical run with `MASHED_RPLIGHT=0` (legacy load-time model-space bake). Same headings to 5 decimals (deterministic 60 Hz sim). |

Matched pairs (headings in radians, same convention both sides):

| pair | original | standalone | Δ heading |
|---|---|---|---|
| GRID | `orig_grid.png` +1.57011 | `01_grid.png` +1.54978 | 0.020 |
| A | `orig_donut_01.png` +0.03502 | `01_turned_a.png` +0.05274 | 0.018 |
| B | `orig_donut_24.png` −0.17080 | `01_turned_b.png` −0.16959 | 0.001 |

## Result 1 — roof band, the like-for-like panel (ACCEPTANCE METRIC)

The roof is the only painted panel visible in every composition on both sides,
and its brightness depends only on the world light state. Paint-masked mean R
(rects + mask in `compare.py`):

| heading | orig | relit | legacy |
|---|---|---|---|
| +1.57 (grid) | 66.5 | 65.6 | 65.5 |
| +0.04 (A)    | 70.7 | **73.3** | 65.5 |
| −0.17 (B)    | 75.1 | **74.1** | 65.5 |

- Original roof response across the sweep: **+8.6 R** (curved roof normals catch
  the sun as the car yaws). Relit: **+8.5 R** — matches within 0.1 R end-to-end;
  worst per-heading gap 2.6 R. Legacy: **flat 0.0** (bake frozen in model
  space), diverging to **9.6 R** at pair B.
- Verdict: the relit path reproduces the original's world-fixed-sun shading;
  the legacy bake demonstrably cannot. GREEN.

## Result 2 — full-car paint means

| pair | orig (view) | relit | legacy | note |
|---|---|---|---|---|
| GRID | 70.2 R (rear) | **70.3 R** (rear) | 71.3 R | same view class both sides — absolute parity Δ0.1 R on the relit path |
| A | 85.5 R (left flank) | 59.7 R (rear) | 71.3 R | opposite panels — see below |
| B | 91.8 R (left flank) | 59.2 R (rear) | 71.6 R | opposite panels — see below |

At pairs A/B the two sides expose **opposite panels**: the original camera held
the track-spline direction over its (stationary, wall-pinned) car — side view —
while the standalone chase cam stays behind its (moving) car — rear view. Under
one world-fixed sun the flank brightening (orig +21.6 R) and the rear dimming
(relit −11.1 R) are the same physical statement viewed from opposite sides;
legacy shows neither (flat ±0.3 R). This row is therefore heading-response
evidence, not an absolute-parity comparison; the roof band above is the
absolute check.

## Result 3 — toggle localization (relit vs legacy, identical framing)

`imgdiff` full-frame, per pair: GRID 0.78 mean / 0.04% px over 16; A 1.32 /
3.47%; B 1.43 / 3.13% (`diff_*_toggle.png`) — the relight touches only the
car/wheel pixels, draw order and state untouched.

## Residuals (full-frame imgdiff is RED and attributed)

`diff_*_orig_vs_*.png`: GRID 26.7 mean / 55% px; A 39.8 / 82%; B 51.5 / 92%.
Attribution, in size order:
1. **Camera pose divergence at turned headings** — original cam settles to the
   track direction on a stationary car; standalone chase cam follows the moving
   donut car. Scene composition, not lighting. (Pre-existing; unrelated to
   MASHED_RPLIGHT — identical between `sa_relit` and `sa_legacy`.)
2. **HUD**: TimeTrial clock + start lights (orig) vs demo HUD + countdown (sa).
3. **Location**: the sa donut drifts ~50 m up-road from the grid by pair A/B.
4. **Mask population**: orig car fills 21–26k px, sa car 3.2k (camera distance)
   — G-channel grid delta 6.4 comes from taillight/decal pixels in the larger
   original mask.
None of these move with the RPLIGHT toggle; item 1 is the P3 in-race camera
work, out of WS-E lighting scope.

## Session fixes feeding this evidence

- `RaceSession.cpp`: vehicle catalogue completed (Shuriken/Sputter/Stallion) —
  the original's default car was not selectable in the standalone before.
- `exe_main.cpp`: race-demo captures are heading-targeted (`RELIGHT_CAP` log).
- `re/frida/capture_relight_parity.py`: original-side warp + drive + shim-dump
  capture aid; cook-descriptor steer bytes are **[0]/[1]** (the [2]/[3] scheme
  in `capture_player_dynamics.py` does not steer — its yaw-rate numbers came
  from crash tumbles, flagged for follow-up).
