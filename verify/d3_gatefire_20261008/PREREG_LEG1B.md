# PRE-REGISTRATION: D-11073 leg 1b (phases 4 + 5), behind default-OFF `MASHED_SUBSTATE`

Date 2026-10-09. Brief: `re/BRIEF_D11073.md` §2 "Leg 1b" and "Facts found after leg 1a".
Base: `29693063` (leg 1a `c4fb5ea6` + boot-patch guard). Each section below is written and committed
BEFORE its step runs. A later section may amend an earlier one only by an explicit dated addendum.

Listings used: `leg1b_decomp.txt` (`decomp_pc.py` on a read-only pool clone: `0x0040dbd0`,
`0x004103a0`, `0x0046d7f0`, `0x0046d780`, `0x0046c7b0`, `0x004111c0`) and a capstone pass over
`original/MASHED.exe.unpatched` for `0x004103a0` (360 B, `0x004103a0..0x00410507`).

## 1. Step 1: car-record routing of three leg-1a bodies

### 1.1 What changes

`Race/SubStateCallees.cpp` (E+A). Three bodies use the original's absolute record addresses:

| RVA | export | absolute(s) | record offset |
|---|---|---|---|
| `0x0046baa0` | `VehiclePhysicsStateReset` | 66 stores in `0x008815a0..0x008820d0+` and two loops | addr - `0x008815a0` |
| `0x0046c750` | `EntityVelocityCounterGet` | `0x00882194 + v*0xd04` | `+0xbf4` |
| `0x0046c730` | `EntityDamageStateGet` | `0x00882198 + v*0xd04` | `+0xbf8` |

Under `/DMASHED_STANDALONE` (exe only) each absolute becomes `VehRecord(v) + (addr - 0x008815a0)`
through the existing `Vehicle/VehicleRecordBase.h` (the same base `LaunchRevCharge.cpp:77-83`
`LrcVehBase()` and `VehicleRecordPtr.cpp` use). The asi arm keeps every absolute: its compiled
arithmetic does not change. In the exe, a null base (array not allocated yet) returns 0 from all
three with no store. That is a named exe-only guard, not in the original (which has no such state).

### 1.2 Predictions (written before the build)

- **P1.1 G-CTRL:** knob OFF, `run_h4.ps1 L1A_A` recipe on the new build: step `c741a4c5`, gates
  `642af0ac`, ptrace `05984e30`. Reason: the leg-1a census found no exe caller of any of the three
  bodies, and step 1 adds none.
- **P1.2 path1 (asi):** unchanged GREEN. `run_diff.py vehicle_physics_state_reset_obs` 7/7 with the
  same per-slot fingerprints as `L1A_PATH1.txt`; `early_window_leaf_diff` table-seed keys
  `table_882194_get` / `table_882198_get` 10/10.
- **P1.3 path2:** `run_verify_hook.py` E9+rel32 to the export at all three RVAs, call-through PASS for
  `vehicle_physics_state_reset` (4/4, returns 1,1,0,0), `table_882194_get`, `table_882198_get`.
  Full auto-hook set (the boot-patch guard `29693063` fixed the boot AV).
- **P1.4 exe value (the brief's ask):** from the exe, `EntityVelocityCounterGet(v)` returns the same
  int as `LrcCharge(v)` (record + `0xbf4`) and `EntityDamageStateGet(v)` the same as `LrcState(v)`
  (record + `0xbf8`), i.e. what `0x00882194/98 + v*0xd04` hold in the original. Witnessed in step 2's
  live poll (the poller reads the record through the same base and prints both).

### 1.3 What these gates can and cannot see

path1/path2 run in the asi, where the arithmetic is unchanged, so they cannot see the exe arm.
G-CTRL proves the default build does not call the bodies. The exe arm's first behavioural witness
is step 5 (G-EXIT, G-HOLD), where `FUN_004103a0` calls them.

## 2. Step 2: live inputs, predictions, censuses (written BEFORE the read)

### 2.1 Instruments

- **Standalone:** new default-OFF `MASHED_L1B_INPUTS=<csv>` (`SubState_InputsDump`,
  `Race/SubStateCallees.cpp`, called once per frame at the top of the countdown section of
  `TrackRenderer::Update`, before the countdown block). Measurement only: reads, writes its own file
  and one `[l1b-inputs]` log line. Columns: `0x0063ba8c`, `DAT_0067e9fc`, `DAT_0067ea64`,
  `DAT_007f0fd0`, `DAT_007f1018`, `DAT_007f0ff4`, `DAT_007f1014`, `DAT_005f29b8`, `DAT_0063ba78`,
  `DAT_0063d7e4`, `d584`/`d588`/step, records `0x0063c8d0+k*0x74` `+0x64`/`+0x50`, and per car:
  `aib_alive(v)`, the slot table `*(*0x005f2770+0x34+v*4)`, `+0xbf4`/`+0xbf8` through BOTH the getter
  and the record base (this is P1.4's witness), the AI mode-5 countdown `0x0089a4f8+v*0x74`, the
  accel byte `0x007f103c + [0x007f1a14+v*16]*0x4c`, and record position `+0x958/+0x95c/+0x960`.
  Recipe: `sa_substate_poll.py`'s env (Training, ROUND, RULE 4, DETERMINISTIC), 600 frames, knob OFF.
- **Original:** `orig_rampwatch.py --cam --leg1b` (new columns: `DAT_007f1018`, `DAT_007f1014`,
  `DAT_0063ba78`, `DAT_0063d7e4` and `[DAT_0063d7e4+0x40]`, `DAT_008991bc`, `DAT_0067ed6c`, the
  mode-5 countdown, the accel byte, `0x007f0a04/08+v*12`, record position `0x00881ef8+v*0xd04`),
  `--statediff-out` given a `.msd` FILE path, 60 Hz, leg-1a scenario args.

### 2.2 Predicted standalone inputs (from RESULT_LEG0 §3, knob OFF)

`e9fc` 0, `ea64` 0, `fd0` 0, `1018` 0, `ba78` 0, `d7e4` 0, `29b8` 0, `ba8c` 0, records `+0x64` 0 for
all four, slot table 1/2/2/2, mode-5 countdown 0 for all four (BSS, no exe writer: census below),
`bf4g == bf4r` and `bf8g == bf8r` on every row (P1.4). AI `+0xbf4/+0xbf8` 0 during the countdown;
player `+0xbf4` follows `LaunchRev_PreRaceTick` (0 unless the recipe feeds throttle).

### 2.3 Predicted handler outputs on those inputs (faithful transcription)

`FUN_0040dbd0` (`0x0040dbd0..0x0040dc4a`): `FUN_0041d910` zeroes d584/d588; `FUN_0042f6a0()` = 0 takes
`default`; `FUN_0042f500()` = 0 falls to `FUN_0041b520`; `DAT_0063ba8c = 5`. Original (leg 1a §3.3):
`e9fc` 10 -> case 10 -> `ea64` 0 -> `FUN_0041b520`. **Same helper on both sides.** The helper copies
row `+0x64` of `0x005f3304` into each record: port +0x64 = 0 for all four -> row 0 four times; original
k = 0..3. Named deviation (writer `FUN_0041b450` unported). Exe readers of the records: census 2.4.

`FUN_004103a0` (`0x004103a0..0x00410507`), one call per phase-5 frame, EAX = frame units (50):
- `0x004103ae..0x004103ca`: `FUN_0040e560` only if `FUN_0042f6a0() == 2`; 0 (port) / 10 (original),
  so never called on either side. Same for `0x004104ca` (`FUN_00429840(1)`, `FUN_00411d60`) and
  `0x004104ed` (`FUN_00411ae0(0,0)`).
- `0x004103f0`: `DAT_005f29b8 -= 50`; with the 12000 seed: 11950 on phase-5 frame 1, `12000-50j`.
- `0x004103f6`: `DAT_007f1014 = (DAT_007f1018 < DAT_007f0ff4)` = 1 in the port.
- `0x00410406`: `FUN_00426c90` -> `FUN_0041ea80` reads `[DAT_0063d7e4]+0x40`, and if non-zero
  `FUN_0041e960` calls through it. `DAT_0063d7e4` is 0 in the port, so a transcription AVs at
  address 0x40. **Predicted: cannot be transcribed; skipped as a named PARTIAL**, [UNCERTAIN] what
  the original's `[DAT_0063d7e4]+0x40` is (the original poll records it).
- `0x0041040e`: `FUN_00418860` = the port's `Ai_Standalone_Tick`, now with `game_sub_mode` = 5.
- `0x00410420..0x00410450`: per car v=0..3 with a non-zero slot-table entry and alive == 1:
  `FUN_0046baa0(v)`, `FUN_0046d7f0(v, 50)`.
- `0x00410457..0x0041046e`: exit when `d588 >= 1.86f` (`_DAT_005ccdf4`): on phase-5 frame **112**
  (d588 = 112 steps = 1.8666651 when read; the tail of the phase-4 frame supplies step 1).
- Exit arm `0x00410470..0x004104e3`: per alive car `FUN_0046d780(v)`, then
  `0x007f0a04+12v = FUN_0046c750(v)`, `0x007f0a08+12v = FUN_0046c730(v)`; `DAT_005f29b8 = 0`;
  `DAT_0063ba8c = 6`.

**Prediction for the AI charge, and the STOP check it triggers.** `FUN_0046d7f0` adds 50 only on
frames whose accel byte exceeds 160. In sub-mode 5 the port's `VehicleStep` (FUN_00418560 Branch A,
`AiStandalone.cpp:1740-1780`, behind `MASHED_MODE5_RESET`) writes `ctrl[4] = 0xff` only while
`elapsed < (cd + 0x40) * 100` (cd >= 0). With the port's cd = 0 that is `12000-50j < 6400`, false for
every j <= 112. **So a faithful phase 5 fed the port's inputs leaves every AI charge at 0, and
`FUN_0046d780` arms nothing.** The original arms all three AI cars (charge 250 on the last
phase-5 sample, `+0xbf8 = 1` from the first phase-6 sample: leg 1a §3.3). Today the port gets that
launch from the measured seed (`VehiclePhysicsRun.cpp`, start-boost arm, `slot != 0`), which is the
USER DECISION 2026-10-03 and gives (e) 3/3. Without `MASHED_MODE5_RESET`, ControlStep runs in
sub-mode 5 and its accel byte is unpredicted here; if it holds > 160 for 60+ frames the charge
saturates at 3000 and `FUN_0046d780` sets `+0xbf8 = 2` (the over-rev bog), a regression. Both cases
are settled by the read plus §3's decision, before any code.

### 2.4 Census (static, exe build = `exe_sources.rsp`; done 2026-10-09 before this section)

| target | exe readers / writers | consequence |
|---|---|---|
| records `0x0063c8d0..0x0063caa0` | `Update41b520` (writer, uncalled) and `HudSlotLoopB630` (reader). `HudSlotLoopB630`'s only caller is `HudIngameDispatch` `0x0040dfc0`, which has **0 exe callers** | the helper's row-0 deviation is unobserved in the exe |
| `0x007f0a04/08 + 12v` | **none** | exit-arm copies unobserved in the exe; G-EXIT reads them directly |
| `0x007f1014` | **none** | the `0x004103f6` store is unobserved |
| `0x005f29b8` | `AiStandalone.cpp:1766` (Branch A `elapsed`, behind `MASHED_MODE5_RESET`) | the 12000 seed reaches exactly one reader |
| `0x0063ba8c` | `HudIngameDispatch` only (0 exe callers) | the port's sub-state model goes through `aib_game_sub_mode` |
| `game_sub_mode` | `AiStandalone.cpp:891` (ControlStep: branch 2 needs `== 6`), `:1091`, `:1210` (M49/M8, unused on fd0 = 0), `:1656` (rubber band returns unless `== 6`), `:1762` (Branch A, `== 5`), `TrackRenderer.cpp:4604` (diagnostic) | in phase 5 only the AI tick reads it |
| `0x0089a4f8 + 0x74v` (mode-5 cd) | `AiStandalone.cpp:1767` reader; **no exe writer** | cd reads 0 in the port (prediction above) |
| `countdown_` | `TrackRenderer.cpp:3189-3205` (the stand-in), `:4995` (seed 3.0), `:5379` race clock, `:5631`, `:5659`, `:5707`, `:5714` (elimination/round gates `countdown_ <= 0`) | §3 must keep `countdown_ > 0` exactly while the sub-state is < 6 |
| `g_launchAccelByte` / `LaunchRev_*` | `TrackRenderer.cpp:3200/3203` only | the stand-in has one call site |

## Addendum A (2026-10-09, after steps 3-4, BEFORE any registered knob-ON arm)

- **G-PHASE5 bit value corrected:** the float32 sum of 112 steps of `0x3c888888` is `0x3feeeee2`
  (1.8666651), computed with numpy before any run. Section 5 wrote `0x3feeeeef` by mistake; the gate
  value is `0x3feeeee2`. Nothing else in the gate changes.
- **One unregistered smoke run happened before this addendum** (`sa_l1b_smoke.csv`, 400 frames,
  SUBSTATE + MODE5_RESET + SUBSTATE_CD=3, `sa_substate_poll.py`): it was a liveness check of the new
  block, and it is reported as such in RESULT, never as gate evidence. The registered arms below run
  on the same build with the full recipe.
- **G-PATH as run:** `sub_state_enter4_obs` run_diff GREEN 5/5, 5 distinct fingerprints;
  `sub_state_held_countdown5_obs` first run RED 2/5 on the `FUN_00422b50` counters only (that callee
  draws `FUN_00472650` when `FUN_0040e470(car) != 1`, and the PRNG is not restored between sides);
  exit vectors changed to slot words 1, re-run GREEN 5/5, 5 distinct fingerprints. The RNG arm of
  `FUN_00422b50` is not vectored. path2: `0x0040dbd0` E9 + call-through PASS; `0x004103a0` E9 PASS,
  call-through not run (EAX argument; the verify template has no EAX arg_type).
- New arg_type `eax_state_machine_observe` in `diff_template.js` (+ `SEEDED_ARG_TYPES` in
  `run_diff.py`); `ARG_TYPES.md` regenerated.

## Addendum B (2026-10-09, BEFORE any completed knob-ON arm)

The first launch of the knob arms (`L1B_SUB` only, own PID 27796) was STOPPED before it finished and
none of its output is used: `StartRound()` (`TrackRenderer.cpp`, the `countdown_ = 3.0f` re-arm,
called per round at the "re-grid for the next round" site) left `DAT_0063ba8c` at 6, so rounds 2+
would have skipped their hold. Fix: under the knob `StartRound()` writes `DAT_0063ba8c = 0`, so each
round's first countdown frame performs the state-2 tail again. Original rationale: the next round goes
case 8 (`FUN_00414220`) -> 2 -> tail; `DAT_005f29b8` is 0 after `0x004104c0`, so the tail takes the
`!= 100000` arm: state 4 with 12000, no phase 3. `MASHED_SUBSTATE_CD` re-seeds per round, where
`FUN_00414180` runs per round. Added gate detail: **G-PHASE5 is scored on EVERY round's pre-race**
(each must show 1 x state 4 then 112 x state 5); G-CTRL is re-run on the rebuilt exe.
