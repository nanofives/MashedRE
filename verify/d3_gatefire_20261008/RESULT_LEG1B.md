# RESULT: D-11073 leg 1b (race sub-states 4 and 5), behind default-OFF `MASHED_SUBSTATE`

Date 2026-10-09. Brief: `re/BRIEF_D11073.md` §2 "Leg 1b". Pre-registration: `PREREG_LEG1B.md`
(§1 `42ac6c01`, §2 `777bebc2`, §2.5-§5 + addenda A/B committed before the registered knob arms).
Code: `0245ba94` + this commit. **The knob stays default OFF. No default flip.**

## 0. Verdict first

> **Leg 1b is done. Every pass gate PASSES, and the default build is byte-identical.**
>
> - **G-CTRL:** knob OFF, step `c741a4c5`, gates `642af0ac`, ptrace `05984e30`. Checked after step 1
>   (`L1B_S1`) and on the final exe (`L1B_CTRL`).
> - **G-PHASE5 / G-SEED / G-HOLD:** pass on **every round's pre-race in every knob arm**
>   (SUB 4/4, M5 3/3, M5CD 2/2). Each round shows 1 frame of state 4, then 112 frames of state 5,
>   then 6. The handler's exit read is `0x3feeeee2` (1.8666651) on 9/9 exits, the original's value.
>   `DAT_005f29b8` runs 12000 -> 6450 at -50 per frame, then 0. Every car's position is bit-identical
>   through the hold.
> - **G-EXIT:** with the one missing input supplied (`MASHED_SUBSTATE_CD=3`, the original's measured
>   value), the exit arm writes **1300 / 1 on all three AI cars and 0 / 0 on the player, on both
>   rounds**. Before that, AI accel is 255 on frames 107..112 and the charge reads 50..250 on the
>   last five rows, matching the original's phase-5 samples (100/150/200/250, accel 255).
> - **G-PATH:** `0x0040dbd0` run_diff GREEN 5/5 + path2 PASS. `0x004103a0` run_diff GREEN 5/5 +
>   path2 E9 PASS (call-through not run: EAX argument). Both are non-degenerate (5/5 distinct
>   fingerprints). The three step-1 bodies stay GREEN. Both handlers are now **C3**.
>
> **Main finding: U-D3-BOOST-ARM's writer is located, and it reproduces the port's measured seed
> exactly.** The original arms the AI start boost through this chain, all driven by `FUN_004103a0`:
> 1. `FUN_00414180` (state-2 tail) rolls the mode-5 countdown cd into `0x0089a4f8+v*0x74`.
>    The original reads 3 on every car.
> 2. `FUN_00418560` Branch A presses accel on frames 107..112.
> 3. `FUN_0046d7f0` charges the car to 300.
> 4. `FUN_0046d780` arms it: `+0xbf4` = 1300, `+0xbf8` = 1.
>
> 1300 is the constant `VehiclePhysicsRun.cpp`'s start-boost arm seeds from captures. The only
> unported link is `FUN_00414180`, a 154-byte roll whose RNG the port only has as a stand-in.
>
> **Not met, reported only (not pass gates):** symptom (a), the `bias374` sawtooth, is NOT
> reproduced (0 reversals in every arm). Symptom (b), the mode-5 branch, IS non-inert now. There is
> one **D2 REOPEN CANDIDATE** (§5).

## 1. What changed

| file | change |
|---|---|
| `Race/SubStateCallees.cpp` (E+A) | step 1: `0x0046baa0` (87 stores), `0x0046c750`, `0x0046c730` address the port's records via `VehRecord` under `MASHED_STANDALONE` (`SSC_REC`); the asi keeps the absolutes. Named exe-only null-base guard. `SubState_InputsDump` (`MASHED_L1B_INPUTS`, measurement only) and `SubState_SeedCd` (`MASHED_SUBSTATE_CD`, measurement only) |
| `Race/SubStateMachine.cpp/.h` (new; E+A) | `0x0040dbd0` `SubStateEnter4`, `0x004103a0` `SubStateHeldCountdown5` (naked EAX shim) + `_C` body; one body per RVA, `RH_ScopedInstall`. Exe arm binds `FUN_00418860` / `FUN_0046c7b0` / the player accel byte through `SubStateHost`; `FUN_00426c90` is skipped there (named PARTIAL) |
| `D3d9Render/TrackRenderer.cpp` | `aib_game_sub_mode()` returns `DAT_0063ba8c` under the knob (else 6). Under the knob, the countdown block runs the state-2 tail, then state 4, then state 5 in `FUN_004111c0`'s order, in place of `countdown_ -= dt` / `LaunchRev_PreRaceTick` / `_Release`. `StartRound()` resets `DAT_0063ba8c = 0` under the knob (addendum B). Knob OFF: untouched |
| `re/frida/diff_template.js`, `run_diff.py`, `ARG_TYPES.md` | new arg_type `eax_state_machine_observe` |
| `re/frida/hooks_registry.py` | `sub_state_enter4_obs`, `sub_state_held_countdown5_obs` |
| `re/tools/orig_rampwatch.py` | `--leg1b` columns |
| trackers (re-classify) | `0040dbd0` C2->C3 `SubStateEnter4`; `004103a0` C2->C3, **renamed** from `TimeTrial::LapFinishProcessor` (the name overclaimed: it is the phase-5 handler); U-9197 (coverage, blocks nothing); the `0040dbd0` note's mis-filed `U-3640` corrected |

`rva-lint` NEW=0 (118 known). Builds clean.

## 2. Gates (PREREG §5, addenda A/B)

| gate | result | evidence |
|---|---|---|
| G-CTRL | **PASS** | `L1B_S1_RUN.txt`, `L1B_ARMS_RUN.txt` (`L1B_CTRL`) |
| G-PHASE5 | **PASS** 9/9 rounds | `L1B_GATES_SCORE.txt`; `[substate] exit d584=112 d588_read=1.86666512 (0x3feeeee2)` x9 in `L1B_{SUB,M5,M5CD}.log` |
| G-SEED | **PASS** 9/9 | same |
| G-HOLD | **PASS** 9/9, tolerance exact | same (`tx/ty/tz`, all 4 cars, all pre-race rows + first state-6 row) |
| G-EXIT | **PASS** on `L1B_M5CD` (1300/1 x3, 0/0 player, both rounds). `L1B_M5`: 0/0 everywhere (cd = 0, predicted). `L1B_SUB`: 3000/2 (§3) | derived witness: the first state-6 row's `+0xbf4/+0xbf8` (the exit frame returns before physics, so nothing writes the record between the copy and that row). The original's first phase-6 sample reads 1276/1, which is 1300 - 24: [UNCERTAIN] the per-frame writer of `0x007f0a04` (not located; no exe reader) |
| G-PATH | **PASS** | `L1B_PATH1.txt`, `L1B_PATH1_held5_run2.txt`, `L1B_PATH2.txt`, `L1B_S1_PATH.txt` |

What the gates cannot see:
- path1/path2 run the asi arm. The exe arm's witnesses are the knob runs.
- The `FUN_00422b50` PRNG arm and the `e9fc == 2` arms are untested (U-9197).
- The original-side capture covers one round only. "Every round re-enters phase 4/5" rests on the
  `FUN_004111c0` listing (case 8 -> 2 -> tail, `!= 100000` arm), not on a capture.

The STOP check (PREREG §3) did not fire. In every arm the round-1 launch keeps the measured
1100/900/700 trajectory: the seed arms on the first physics frame, and in M5CD the chain wrote the
same value first.

## 3. Arm findings outside the gates

- **`MASHED_SUBSTATE` without `MASHED_MODE5_RESET` is not a faithful arm and must not be used
  alone.** Without Branch A, ControlStep runs in sub-mode 5 and holds accel 255. The charge
  saturates at 3000 and `FUN_0046d780` sets `+0xbf8 = 2` (the over-rev bog). In round 1 the seed
  overwrites it. In rounds 2+ the bog persists (`L1B_SUB.inputs.csv` rows 1697-1699: 3000/2, 2800/2,
  2600/2), because `g_startBoosted` is one-shot per race (`VehiclePhysicsRun.cpp`). Predicted in §2.3.
- **Rounds 2+ boost only in M5CD.** The start-boost seed fires once per race, so today's default
  (and `L1B_M5`) gives no AI boost in rounds 2+. The original's per-round path re-runs
  `FUN_004103a0`'s exit arm, and M5CD re-arms 1300/1 at round 2. That is a pre-existing default-build
  gap in rounds 2+. [UNCERTAIN] it is shown from the listing and the port; no multi-round original
  capture exists.
- Round count per 14,400-frame run differs by arm (SUB 4, M5 3, M5CD 2 pre-race segments; CTRL has
  its own), because race outcomes move once the pre-race shortens from 181 to 114 frames.

## 4. Symptoms (report only)

- **(a) `bias374` sawtooth: NOT reproduced.** 0 reversals in every arm. Post-GO histograms:
  CTRL `{0:661,1:601,2:1201,3:1201,4:9834}`, SUB `{0:601,1:601,2:1087,3:11653}`,
  M5 `{0:660,1:601,2:4137,3:1201,4:7457}`, M5CD `{0:660,1:601,2:2670,3:1201,4:9038}`.
  The histograms move, but band 0/1 counts stay at CTRL's (660/601 vs 661/601), so the data do
  **not** show the ladder restarting per round. [UNCERTAIN] why the 2/3/4 split moves: `0x007f0ff8`
  is not in the dump, so Branch A's per-round re-zero (`0x00418598`) is shown statically only. The
  original re-zeroes it about every 8 s (`RESULT_RAMP.md`). RESULT_M5 recorded 5/4/6/7/0/11/9/10
  cycling, and this leg ports only the pre-race 4 -> 5. Next probe: add `0x007f0ff8` and `bias374`
  to the inputs dump.
- **(b) mode-5 branch: NON-INERT now.** Phase-5 accel is 255 on SUB versus 0 on M5 (same build,
  `MASHED_MODE5_RESET` the only difference), and 255 on exactly frames 107..112 on M5CD. RESULT_M5's
  "the port never enters sub-mode 5" is superseded under the knob.
- **(b) criterion, this recipe, report only** (`L1B_B_SCORE.txt`, `ai_ctrl_window.py --check` on the
  run_h4 recipe, NOT the (b) gate recipe): CTRL fails 5 bands, all on car 2. SUB, M5 and M5CD each
  fail 5, split 3 + 2 over cars 2 and 3.
- Windows: every step / gates / ptrace dump starts at the first post-GO physics frame, so all of the
  above are anchored on the sub-state-6 entry, never on frame 0 (BRIEF §3).

## 5. D2 REOPEN CANDIDATE (report only; no D2 code changed)

| finding | evidence | D2 re-pickup condition it touches |
|---|---|---|
| Under the knob, the player's post-GO speed departs from the default build: f=31 1607.95 vs 1627.64 (-1.2%), f=121 -0.6%, f=301 -0.86%, f=601 -0.5%. Same in every knob arm. On f=1 the only differences are basis residues: the default record carries a `2.7e-8` x/z residue in the up vector (`n9c8`, `r0up`) and in `bodyfwd.y`; under the knob they are exactly `(0,1,0)` / `0`, after the per-frame `FUN_0046baa0` re-init of the player record. The original's up vector is exactly (0, ~1, 0) on 2333/2333 frames of `orig_solo3.msd` (as quoted at the `n9c8` trace site in `TrackRenderer.cpp`) | `L1B_{CTRL,M5}.ptrace.log` | "player-car speed on Training". The direction (-1.2%) widens D2's driving-median gap (already 1.3% under). [UNCERTAIN] whether the residue or another reset field carries it; not bisected. Measured on the 4-car run_h4 recipe, not D2's solo reference |

## 6. Open items (tracked, not worked)

| item | status | next concrete step |
|---|---|---|
| **`FUN_00414180` (cd producer)** | NEW top item. It is the only missing link for the original's AI start-boost arming. 154 B, C2, no body | port it at its RVA behind the knob. Predict first: its `FUN_00472650` RNG is a stand-in in the port, so pin what the port's stream gives for `idx = ftol(DAT_0089a360)` (rows (0,3)..(4,8)) before writing it. It also zeroes `DAT_0089a368/370` (the AI one-shot roll), so register its blast radius on (b) |
| D-11073 leg 2 (phase 3, camera hold) | unchanged | gate on the release rule (entry 0 within 0.02 of target, `FUN_00445aa0`), route `FUN_00442600` through `aib_track_index()` |
| Knob default flip | NOT done | user decision. Needs `MASHED_SUBSTATE` to imply Branch A (or both flipped together), the start-boost seed reconciled with `FUN_004103a0`'s arming (one writer), and scorers re-anchored |
| U-9186 sources | unchanged: 64-call branch (`FUN_004148b0`/`FUN_00416060` exe homes, LeaderTimer .bss inputs), mode 7 (`FUN_00484c70`), `FUN_00414a70` | re-measure the 149-call split with the knob ON once cd is produced |
| D-11072 | unchanged | monotone arc-length progress + `FUN_00442a60` |
| U-9196 | RESOLVED 2026-10-06 (PARTIAL); `0x00469df0` stays C2 | cross-side behavioural diff of the call site |
| U-9195 | open | participant-count caveat on the ring publication |
| ROADMAP follow-ups 1, 2, 4 | open (QoL tracker row; collision-FX re-measure; `log/` citation sweep) | housekeeping session |
| D1-residue (R1-R3) | open, must close before D5 | unchanged |
| D2 | PARKED; 1 REOPEN CANDIDATE (§5) | user decision |
| `0x007f0a04` decrement (1300 -> 1276) | [UNCERTAIN], no exe reader | locate the writer when a consumer appears |

## 7. Raw

`PREREG_LEG1B.md`, `leg1b_decomp.txt`, `leg1b_decomp2.txt`, `o_leg1b.csv/.log` (original, `--leg1b`),
`sa_l1b_inputs.csv`, `L1B_INPUTS_SCORE.txt`, `sa_l1b_smoke.csv` (unregistered smoke, addendum A),
`L1B_{S1,CTRL,SUB,M5,M5CD}.*`, `L1B_*.inputs.csv`, `L1B_GATES_SCORE.txt`, `L1B_B_SCORE.txt`,
`L1B_*PATH*.txt`, `L1B_diff_*.csv`. Scorers: `score_leg1b_inputs.py`, `score_leg1b_gates.py`.
Every MASHED / mashed_re PID spawned this session was its own and exited or was killed by its own
PID (one aborted arm, pid 27796, addendum B).
