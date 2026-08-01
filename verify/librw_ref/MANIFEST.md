# E3' reference captures — pre-librw D3D9 baseline (deterministic)

**Captured 2026-08-01** from `mashedmod/build/mashed_re.exe` at `01e87920`, librw
vendored but **gated OFF** — this is the hand-written D3D9 path, not librw.

This set **replaces** the 2026-07-31 one, which was invalid: it was captured before
the deterministic capture mode existed, and its in-race shots recorded a car being
driven by stray keystrokes leaking in through background DirectInput. See
"How this was got wrong twice" at the bottom — the failure modes are worth knowing
before trusting any capture from this harness.

## Recipe — reproducible, exits on its own

```powershell
$env:MASHED_DETERMINISTIC="1"   # frame-index clock; suppresses ambient kbd/mouse
$env:MASHED_DET_FRAMES="3000"   # quit after N frames -- NOT a wall-clock kill
$env:MASHED_RACE_DEMO="1"
$env:MASHED_GOTO="6"            # mandatory: parks on Challenge Select
$env:MASHED_DRIVE_HOLD="1"      # hold InRace past the late chase frames
$env:MASHED_DRIVE_DEMO="1"      # scripted driver -- WITHOUT THIS THE CAR NEVER MOVES
mashedmod\build\mashed_re.exe
```

Every one of those matters:
- **`MASHED_DETERMINISTIC=1`** — routes all 29 `GetTickCount()` sites through a
  frame-index clock, pins the physics accumulator to one fixed step per frame, and
  **suppresses live keyboard/mouse**. DirectInput is opened
  `DISCL_BACKGROUND | DISCL_NONEXCLUSIVE`, so without this the game reads your
  keyboard *while you type in another window* and flies the camera.
- **`MASHED_DET_FRAMES=N`** — the run must end on a frame count. Killing it on a
  wall-clock timeout stops it at a different synthetic instant each time, which
  reintroduces exactly the nondeterminism the mode removes.
- **`MASHED_GOTO=6`** — `RunRaceDemoStep` is documented at `exe_main.cpp:1029-1030`
  as paired with it. Without it the driver never advances a step and writes nothing.
- **`MASHED_DRIVE_DEMO=1`** — leaves `human_drive_` false so the auto-follow drives.
  Without it nothing supplies input, and the sim trace shows `spd=00000000` with an
  unchanging position for the whole run: the in-race shots become pictures of a
  parked car.

## Set — and which shots may gate

`gates?` = bit-identical across two rebuilds of identical source, so a difference
means something real. The three marked NO are still build-unstable (R10b residual);
**they must not be used as an E3' gate** until that is closed.

| File | size | sha256[:16] | gates? |
|---|---|---|---|
| `00_challengeselect.png` | 640x480 | `d39654c7c55a6dd8` | **NO** (build-unstable) |
| `00_results.png` | 640x480 | `a1fd7a097aad0cea` | yes |
| `01_action.png` | 640x480 | `81735239fb8fd0b6` | yes |
| `01_cupstandings.png` | 640x480 | `31d92806d190026f` | yes |
| `01_grid.png` | 640x480 | `901b2c65f36fec72` | yes |
| `01_inrace_track.png` | 640x480 | `1e2855d2163a4b23` | **NO** (build-unstable) |
| `01_results.png` | 640x480 | `0bdef305e83947df` | yes |
| `02_back_to_menu.png` | 640x480 | `b2ec6d85f1abbc03` | **NO** (build-unstable) |
| `car_1_spawn.png` | 640x480 | `c873afdf6ed4f461` | yes |
| `car_2_drive.png` | 640x480 | `30faff64a85c2eb5` | yes |
| `car_3_weave.png` | 640x480 | `517f84c07d613512` | yes |
| `car_4_chase.png` | 640x480 | `1b86ebfb7b31c4bf` | yes |
| `car_5_chase.png` | 640x480 | `56f0dfc31f49467c` | yes |

**10 of 13 gate.** Evidence: two independent rebuild-control pairs. POST-vs-POST
differed only on `02_back_to_menu`; PRE-vs-PRE differed on all three marked NO. The
controls disagree with each other, so the residual is *intermittent between build
pairs* — which is also why a single control run proves nothing here. Use several.

The car really drives in this set: the sim trace shows position advancing and
`spd=411E18D0` (~9.9) at the mid-race capture, versus `spd=00000000` throughout the
superseded set.

## Diffing against it

```
py -3.12 re/tools/imgdiff.py verify/librw_ref/<name>.png <librw_capture>.png \
    --out verify/<name>_heat.png --grid 8x6
```

Acceptance for gate D2 is **behavioural parity with documented visual deltas, not
bit-parity** (`re/analysis/LIBRW_SIZING_2026-08.md` §4-E3', risk R2). On the ten
gating shots the noise floor is exactly zero — they are bit-identical across
rebuilds — so *any* difference on those is signal and needs a written cause.

## Pre-existing deltas — record them so librw is not blamed later

- **D-REF-1 — trackside banner text renders mirrored. OPEN.** "SUPERSONIC" and
  "EMPIRE" read backwards (legible, horizontally flipped) in the in-race shots.
  Present on the current D3D9 path. **PAL4 was tested and ruled out** as the cause.
  A UV or winding-order issue in the banner prop path.
- **D-REF-2 — the course is dark** (in-race means ~31-36 vs ~97-111 for menus), so
  whole-image mean-abs flatters in-race diffs. Judge those on per-region grid stats.

## Coverage gap

No isolated particle/weather frame and no isolated pickup-orb frame; both appear
only incidentally in-race. Add capture hooks in `ParticleSystem`/`PickupField`
during E2'c if E3' needs them isolated.

## How this was got wrong twice

Both failures were the *measurement*, not the code, and both times the wrong
conclusion was confident:

1. **"The harness is deterministic (0.01 noise floor)."** Generalised from four
   shots, two of which happened to be stable. Actually up to 36.74.
2. **"The E2'c refactor broke rendering."** An A/B run without an adequate control.
   Rebuilding the *same* source reproduced the identical differing-file list.

The rule that came out of it: **a capture A/B means nothing without a same-source
rebuild control, and one control pair is not enough** — the two controls here
disagree (1 unstable shot vs 3). Run several before believing a difference.
