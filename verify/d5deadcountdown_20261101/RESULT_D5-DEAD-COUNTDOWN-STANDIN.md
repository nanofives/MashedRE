# RESULT D5-DEAD-COUNTDOWN-STANDIN (2026-11-01)

Verdict: **GREEN (gates unchanged)**

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

`gates.sh` ran `verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` under `mashed_lock.py run`, exe
`c51c6da7`, private root `D:/MashedWT/D5-DEAD-COUNTDOWN-STANDIN_root`. The first submission (14:05) was killed by the LIVING-PC
reboot at 14:23 before the lock was granted, so it ran no arm. It was resubmitted at 14:52, got the lock at about 15:40 and
finished at 15:57:06 with lock/arms rc 0. Every arm exited by itself with exit 0 (H_DEF 280 s, B arms 12 to 14 s).

| Gate | This branch | Ref (`re/orchestrator/merge_train.json`) | |
|---|---|---|---|
| G-CTRL (H_DEF step / gates / ptrace) | `8284be46/ba407a16/31fe043e` | `8284be46/ba407a16/31fe043e` | unchanged |
| B (K0 / K1 / K26 step) | `1d5bf4f9/1d5bf4f9/1d5bf4f9` | `1d5bf4f9/1d5bf4f9/1d5bf4f9` | unchanged |
| (b) `vsum.py` | 3/3 on all three B arms (car1..3 A PASS) | | |
| (e) `ai_speed_env.py --check` B_DEF_K0 | v1 / v2 / v3 PASS | | |

Line counts: H_DEF.step.csv 13962, H_DEF.gates.csv 54085, H_DEF.ptrace.log 27042, each B step.csv 762.
The arms run only `mashed_re.exe`, not the original, so no original-side launch happened and the runner emitted no audio
check line.

## 5. Raw files (not committed)

Folder `D:/MashedWT/D5-DEAD-COUNTDOWN-STANDIN_data/` (sha256):

| file | sha256 |
|---|---|
| `arms/H_DEF.step.csv` | `8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198` |
| `arms/H_DEF.gates.csv` | `ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb` |
| `arms/H_DEF.ptrace.log` | `31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3` |
| `arms/B_DEF_K0.step.csv` | `1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457` |
| `arms/B_DEF_K1.step.csv` | `1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457` |
| `arms/B_DEF_K26.step.csv` | `1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457` |
| `vsum_b.txt` | `6b0c8feada572b7600d357e6cc3ee1302fa5bb25d59b34fbb6d4f01a9ee2956a` |
| `e_check.txt` | `7ab3355e97ac453609feb1aa98b74c9267e31e8941016f5e364d5febaa8d5051` |
| `build.log` | `22b7dc8b81e96219ac799c03ebd7fc63deeec63ab80478a3eddf39720418b0c3` |
| `lint.txt` | `74fada50805d9dc79713be62c0b4b501b314dce16fad6a2ae5c12cc1133f4d77` |

## 6. Follow-up

The five callees left without a caller (section 2) are a candidate step: check each one's hooks.csv row, .asi use and
dual-copy allowlist entry, then delete.

