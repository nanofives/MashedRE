# RESULT D5-DEAD-COUNTDOWN-STANDIN (2026-11-01)

Verdict: **PENDING (gates queued)**

Step: delete the countdown stand-in in `TrackRenderer::UpdateCar` (the ledger title says `TrackRenderer::Update`, but the
block is in `UpdateCar`, `mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:5114`). It became unreachable when
D5-KNOBS-E5-REST deleted MASHED_SUBSTATE (`verify/d5knobse5rest_20261031/RESULT_D5-KNOBS-E5-REST.md` section 5, "Residue").
Branch `orch/D5-DEAD-COUNTDOWN-STANDIN` from `orch/integration` at `ca02bde58`. Source commit `20efede42`: 1 file,
8 insertions, 111 deletions. Source only. No knob added, no default flipped, no tracker row touched.

## 1. Static proof the block was dead

Line numbers are from `TrackRenderer.cpp` at the base `ca02bde58`. Both blocks are in `TrackRenderer::UpdateCar` (5114), at
the function's top level (4-space indent), and `UpdateCar` has no `goto` or label.

1. **Guard A, `:5208` `if (countdown_ > 0.f) {`** opens the sub-state block (FUN_004111c0's phases). It has no env or knob
   test since D5-KNOBS-E5-REST removed `SubState_Enabled()` from it.
2. **Block A returns on every path.**
   - `ba8c == 0` (state-2 tail, `:5259`): both arms of `if (s29b8 == 100000)` fall to `return;` at `:5271`.
   - `ba8c == 3 / 4 / 5 / any other value`: the `if / else if` chain at `:5273..5309` has no `return`, `break` or
     `goto`, so control reaches the common tail `:5310..5338` and `return;` at `:5339`, the block's last statement.
   The lambdas defined in the block (`h.ai_tick`, `h.player_accel_byte`, `h.player_settle`) are host callbacks. They do
   not transfer control out of `UpdateCar`.
3. **Nothing between the two guards writes `countdown_`.** `:5340` is block A's `}`. `:5341..5345` are blank or comment
   lines.
4. **Guard B, `:5346` `if (countdown_ > 0.f) {`**, the stand-in, has the same expression as guard A. With
   `countdown_ > 0.f` true, block A returned. With it false (NaN included), guard B is false too. `countdown_` is a plain
   `float` member (not `volatile`) and no statement between the two guards writes it. So the body `:5347..5447` never
   executes.

The stand-in held: `countdown_ -= in.dt`, the pre-GO `Ai_AdvanceCountdownClock`, `LaunchRev_PreRaceTick`, the
MASHED_COUNTDOWN_SPAWN pieces (`Ai_CountdownAlivePoke`, `VehiclePhysics_SeedSpawnPose`), the GO-frame SFX + spawn settle +
`LaunchRev_Release`, `PathProg_FrameTick`, the pre-GO `RaceCamUpdate`, `Ai_RaceTimerCountdownTick`, `CopterTick` and
`PreGoRegistryTick` behind `CamReplayRowRunsFc00`. Block A already does its own form of each (sub-state host settle, step
store, `Ai_AdvanceCountdownClock`, `Ai_RaceTimerCountdownTick`, `CopterTick`, `PreGoRegistryTick`, GO SFX on the exit frame).

## 2. Change (commit `20efede42`)

- `:5342..5448` (the knob-OFF HUD adapter note, the stand-in comment and block B) replaced by a 4-line comment naming what
  was deleted and why.
- Three comments in block A that pointed at "the stand-in below" reworded (`:5198`, `:5231`, `:5328`). No code in block A
  changed.

Left in place (not deleted here, so the diff stays one dead block):
- **Callees with no remaining caller** (grep over `mashedmod/src/mashed_re`, `*.cpp` / `*.h`):
  `Ai::Ai_CountdownAlivePoke` (`Ai/AiStandalone.cpp:2089`), `Vehicle::LaunchRev_PreRaceTick` / `LaunchRev_Release`
  (`Vehicle/LaunchRevCharge.cpp:268` / `:281`), `Vehicle::VehiclePhysics_SeedSpawnPose`
  (`Vehicle/VehiclePhysicsRun.cpp:844`), `CamReplayRowRunsFc00` (`TrackRenderer.cpp:8403` at the base). Each one needs its
  own check (hooks.csv row, .asi use, dual-copy allowlist) before deletion. Candidate follow-up step.
- **Env names gone from the exe**: `MASHED_NO_COUNTDOWN_SPAWN` (its only reader was in block B), 0 hits in
  `mashed_re.exe` and `mashed_re_dev.asi` (Python `bytes.count` of `MASHED_NO_COUNTDOWN_SPAWN\0`). It already had no effect.
  `MASHED_NO_RACE_CAM_BASIS` keeps its other reader (`TrackRenderer.cpp:9370` at the base) and 1 hit in the exe.

## 3. Build and lint

- `cmd /c "set MASHED_NO_DEPLOY=1&& mashedmod\build.bat"` from the worktree: `=== Build OK ===`, 0 `error C` / `LNK` error
  lines. The shared `.asi` was not deployed. `mashed_re.exe` sha256 `c51c6da7`, `mashed_re_dev.asi` `c90a492d`.
  `TrackRenderer.cpp` is not in `mashedmod/asi_sources.rsp` (0 hits), so the .asi is unaffected by this TU.
- rva-lint (`scripts/lint_rva_bodies.py`, the lint E5 ran): rc 0,
  `47 finding(s): CROSS-TARGET=33, DUP-IN-TARGET=14 | allowlisted=47 NEW=0`. The base `ca02bde58` (orch-integration
  worktree) gives the same line, so the count moved from E5's 48 / 33 / 15 before this branch, not in it.

## 4. Gates

Pending. `gates.sh` (H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26 under mashed_lock) was queued at 14:05 and killed by the LIVING-PC
reboot at 14:23 before the lock was granted. Resubmitted 14:52 with exe `c51c6da7`, waiting behind a D4 SOLO window and a
merge-train hold. Refs (`re/orchestrator/merge_train.json`): G-CTRL `8284be46/ba407a16/31fe043e`, B `1d5bf4f9` x3.

## 5. Raw files (not committed)

Folder `D:/MashedWT/D5-DEAD-COUNTDOWN-STANDIN_data/` (sha256):

Pending (no arm has run yet).
