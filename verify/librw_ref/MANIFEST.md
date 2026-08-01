# E3' reference captures — current D3D9 path (pre-librw baseline)

**Captured 2026-07-31 22:32** from `mashedmod/build/mashed_re.exe`, librw vendored
but **gated OFF** — these are the hand-written D3D9 path, not librw.

**Re-captured after the PAL4 fix.** The first set (22:15–22:16) was taken before
`TrackRenderer::MakeTexture`'s PAL4 decode bug was found and fixed in the same
session, so it is superseded and must not be used. Effect of the fix on these
viewpoints, measured old-vs-new with `imgdiff.py`: mean-abs **0.85 / 0.88** on the
two race shots, **0.01** on the menu and chase shots.

> ### CORRECTION 2026-07-31 — the "deterministic harness" claim below was WRONG
>
> This file previously said the 0.01 mean-abs seen on two shots meant the capture
> harness was "effectively deterministic", with ~0.1 as the signal threshold.
> **That was an overgeneralisation from a 4-shot sample, two of which happened to
> be stable.** Measured properly over all ten shots, two boots of a *byte-identical
> binary* differ by:
>
> | shot | run-to-run mean-abs |
> |---|---|
> | 01_menu_challenge_select | 2.74 |
> | 02_race_grid_startline | **32.94** |
> | 03_race_midlap_props | **32.88** |
> | 04_race_action | **36.74** |
> | 05_car_spawn | 0.10 |
> | 06_car_chase_speed | **35.73** |
> | 07_car_chase_late | **30.68** |
> | 08_results | 0.83 |
> | 09_match_result | **31.65** |
> | 10_menu_return | 3.69 |
>
> **Root cause:** `exe_main.cpp:2318` computes `dt` from the GetTickCount wall
> clock and `:2416` passes that wall-clock `dt` into `UpdateCar`, so the race
> simulation advances by a frame-rate-dependent amount. Capture triggers also fire
> on wall-clock `t` (`t >= 0.8f`, `>= 9.0f`, `>= 16.0f`). The seed is fine — spawn
> state is bit-identical run to run (`gate0=(-26.08, 0.04, 17.00) yaw=1.55`) — the
> divergence accumulates purely from variable `dt`. That is also why `05_car_spawn`
> (earliest capture, t=0.8 s) is the only stable in-race shot.
>
> **Consequence: these captures cannot serve as an imgdiff baseline for E3' as
> specified.** Run-to-run noise up to ~37 mean-abs would swamp any realistic
> renderer delta. They remain valid as a *visual* record of the pre-librw path,
> and `05_car_spawn` alone is usable numerically. Fixing this needs a deterministic
> capture mode (fixed `dt` everywhere plus frame-count-driven capture triggers —
> the same "clock = render frames" approach that already gives +0 drift in
> `replay_verify.py`). Tracked as risk R10 in `re/analysis/LIBRW_SIZING_2026-08.md`.

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

> ### SUPERSEDED 2026-08-01 — this whole capture set is INVALID as a baseline
>
> Root cause of R10b, found by looking at two "identical build" frames side by
> side: one showed the chase camera behind the car, the other a high orbit view
> with the car off-screen — same simulation instant. The camera reads **live
> DirectInput keyboard and mouse** (`exe_main.cpp` camera block + `GetCursorPos`),
> and the device is opened `DISCL_BACKGROUND | DISCL_NONEXCLUSIVE`, so it reads
> the keyboard **even with no window focus**. Typing in a terminal while a capture
> ran flew the camera and steered the car. That, not build nondeterminism, is what
> produced every "rebuild changed the output" result — and it is also what
> produced the false "the E2'c refactor broke it" verdict.
>
> Suppressing ambient input under `MASHED_DETERMINISTIC` takes two rebuilds of
> identical source from 5/13 to **11/13** bit-identical.
>
> **Why this set must be re-taken:** with ambient input gone the sim trace shows
> `spd=00000000` and an unchanging `pos` at every capture — the player car never
> moves, because `MASHED_DRIVE_HOLD` alone leaves `human_drive_` true and nothing
> supplies input. So the in-race shots here (`02`, `03`, `04`, `05`, `06`, `07`)
> recorded a car driven by **stray keystrokes**, not by the game. They are not
> reproducible and must not gate anything.
>
> The corrected recipe needs a scripted driver — `MASHED_DRIVE_DEMO=1` (auto-follow
> + scripted throttle/weave) or `MASHED_PLAY_DEMO=1` — on top of
> `MASHED_DETERMINISTIC=1 MASHED_DET_FRAMES=N`. Re-take the set that way before
> E3' relies on it.

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
