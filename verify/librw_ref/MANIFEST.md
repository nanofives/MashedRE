# E3' reference captures — current D3D9 path (pre-librw baseline)

**Captured 2026-07-31** from `mashedmod/build/mashed_re.exe` at commit `122e3010`
(the E1' commit; librw vendored but **gated OFF** — these are the hand-written
D3D9 path, not librw).

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
| 1 | `01_menu_challenge_select.png` | `verify/race1/00_challengeselect.bmp` | 22:16:30 | `102dc83f279b848f` |
| 2 | `02_race_grid_startline.png` | `verify/race1/01_grid.bmp` | 22:16:33 | `20ac4ce249a2aed5` |
| 3 | `03_race_midlap_props.png` | `verify/race1/01_inrace_track.bmp` | 22:16:35 | `8011becc7de8440a` |
| 4 | `04_race_action.png` | `verify/race1/01_action.bmp` | 22:16:37 | `a7d8b8f01fdda96a` |
| 5 | `05_car_spawn.png` | `verify/r5/car_1_spawn.bmp` | 22:16:31 | `8bc5a55bb057672c` |
| 6 | `06_car_chase_speed.png` | `verify/r5/car_4_chase.bmp` | 22:15:57 | `b9d39f22c4f216b3` |
| 7 | `07_car_chase_late.png` | `verify/r5/car_5_chase.bmp` | 22:16:04 | `b883870fb756fcfe` |
| 8 | `08_results.png` | `verify/race1/00_results.bmp` | 22:16:39 | `d1b36eb1399ac27b` |
| 9 | `09_match_result.png` | `verify/r6/match_result.bmp` | 22:16:36 | `70e8ed34e4055d92` |
| 10 | `10_menu_return.png` | `verify/race1/02_back_to_menu.bmp` | 22:16:39 | `899cd8271460efa5` |

All ten are 640x480, all confirmed non-degenerate (distinct sha256 per shot;
950-2686 distinct sampled colours each; non-zero means). Lossless BMP -> PNG
conversion only; no resampling, no colour transform.

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

- **D-REF-1 — trackside banner text renders mirrored.** In
  `03_race_midlap_props.png` the "SUPERSONIC" and "EMPIRE" banners read
  backwards. Present in the current D3D9 path, before librw touches anything.
  Likely a UV or winding-order issue in the banner prop path. Not yet filed as an
  uncertainty; if E3' shows it on the librw side too, it is inherited, not caused.
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
