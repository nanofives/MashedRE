# E3' reference captures — current D3D9 path (pre-librw baseline)

**Captured 2026-07-31 22:32** from `mashedmod/build/mashed_re.exe`, librw vendored
but **gated OFF** — these are the hand-written D3D9 path, not librw.

**Re-captured after the PAL4 fix.** The first set (22:15–22:16) was taken before
`TrackRenderer::MakeTexture`'s PAL4 decode bug was found and fixed in the same
session, so it is superseded and must not be used. Effect of the fix on these
viewpoints, measured old-vs-new with `imgdiff.py`: mean-abs **0.85 / 0.88** on the
two race shots, **0.01** on the menu and chase shots.

That 0.01 pair is itself useful: two independent boots of the same scene differ by
0.01 mean-abs, so **the capture harness is effectively deterministic** and an E3'
delta above ~0.1 mean-abs is signal, not run-to-run noise.

Purpose: gate D2 makes librw the shipping renderer, and its acceptance is
**behavioural parity with documented visual deltas, explicitly not bit-parity**
(`re/analysis/LIBRW_SIZING_2026-08.md` §4-E3', risk R2). That comparison needs a
baseline taken *before* any librw submission lands. This is it. Once E2' starts
changing the draw path, this set can no longer be re-created.

Diff a librw-path capture against these with:

```
py -3.12 re/tools/imgdiff.py verify/librw_ref/<name>.png <librw_capture>.png \
    --out verify/<name>_heat.png --grid 8x6
```

## Set

| # | File | Source BMP | Captured | sha256[:16] |
|---|---|---|---|---|
| 1 | `01_menu_challenge_select.png` | `verify/race1/00_challengeselect.bmp` | 22:32:11 | `87fdb02fa7587a72` |
| 2 | `02_race_grid_startline.png` | `verify/race1/01_grid.bmp` | 22:32:13 | `977cfe2220847e7e` |
| 3 | `03_race_midlap_props.png` | `verify/race1/01_inrace_track.bmp` | 22:32:15 | `7e828240e79d7ce1` |
| 4 | `04_race_action.png` | `verify/race1/01_action.bmp` | 22:32:18 | `50d33eb33356b0b6` |
| 5 | `05_car_spawn.png` | `verify/r5/car_1_spawn.bmp` | 22:32:12 | `45c5e7de859c0ef2` |
| 6 | `06_car_chase_speed.png` | `verify/r5/car_4_chase.bmp` | 22:32:20 | `56472e66933f2889` |
| 7 | `07_car_chase_late.png` | `verify/r5/car_5_chase.bmp` | 22:32:27 | `8699f585b3f8b6c1` |
| 8 | `08_results.png` | `verify/race1/00_results.bmp` | 22:32:09 | `26323f9a31a43a22` |
| 9 | `09_match_result.png` | `verify/r6/match_result.bmp` | 22:32:06 | `27e96960715e7249` |
| 10 | `10_menu_return.png` | `verify/race1/02_back_to_menu.bmp` | 22:32:41 | `ebf64ad9788b1c36` |

All ten are 640x480, all confirmed non-degenerate (distinct sha256 per shot;
950-2686 distinct sampled colours each; non-zero means). Lossless BMP -> PNG
conversion only; no resampling, no colour transform.

Shots 1-4, 8-10 come from the `MASHED_RESULT_DEMO` boot; shots 5-7 from the
`MASHED_DRIVE_HOLD` boot. Both boots ran on the same post-PAL4-fix binary.

## Recipe (reproducible)

```powershell
# shots 1-4, 8, 10  (verify/race1/) and 9 (verify/r6/)
$env:MASHED_RACE_DEMO="1"; $env:MASHED_GOTO="6"; $env:MASHED_RESULT_DEMO="1"
mashedmod\build\mashed_re.exe          # exits on its own

# shots 5-7  (verify/r5/) — DRIVE_HOLD keeps InRace past t=9s and t=16s so the
# two late chase frames actually fire; kill the process after ~40 s
$env:MASHED_RACE_DEMO="1"; $env:MASHED_GOTO="6"; $env:MASHED_DRIVE_HOLD="1"
mashedmod\build\mashed_re.exe
```

**`MASHED_GOTO=6` is mandatory** and is the whole reason an earlier attempt
produced nothing: `RunRaceDemoStep` is documented at `exe_main.cpp:1029-1030` as
"paired with `MASHED_GOTO=6` so we start parked on the Challenge Select screen".
Without it the driver never advances a step, emits no log line, and writes no
BMP — it looks like a broken driver but is a missing companion variable.

## Pre-existing deltas visible in this baseline

Recorded now so they are not later misattributed to librw:

- **D-REF-1 — trackside banner text renders mirrored. OPEN.** In
  `03_race_midlap_props.png` the "SUPERSONIC" and "EMPIRE" banners read
  backwards (legible, but horizontally flipped). Present in the current D3D9
  path, before librw touches anything. **PAL4 has been ruled out as the cause** —
  the banners are still mirrored in this post-fix set. So it is a UV or
  winding-order issue in the banner prop path. If E3' shows it on the librw side
  too, it is inherited, not caused by librw.
- **D-REF-2 — the captured track is very dark** (sampled means 31-36 vs 97-111
  for menu shots). Expected for this course at 640x480, but it compresses the
  dynamic range available to `imgdiff`. When judging E3' deltas on shots 2-7,
  prefer per-region grid stats over the whole-image mean, which will look
  flatteringly small on a dark frame.

## Coverage gaps

Two of the eight viewpoints named in the sizing brief are **not** isolated here:
a particle/weather-active frame and a pickup-orb frame. Both effects appear
incidentally inside the in-race shots, but neither has a dedicated capture point.
If E3' needs them isolated, add capture hooks in `ParticleSystem` / `PickupField`
during E2'c rather than trying to time them from outside.
