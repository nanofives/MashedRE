# PREREG d3bB2: D3 (b) regime B car 2, the AI clock at GO (2026-10-17)

Branch `work/d3bB2` from `race/first-frame-parity` `18ddb6ea2`. Model claude-opus-5-5, account2. Written and committed with
the code, before any arm of the new knob runs. Exploratory arms run before this file (baseline, the existing U-9684 / fix B
stacks) are listed in section 1 and are not gates.

## 0. What was measured before this file (evidence, not gates)

1. **Baseline reproduced, 2 of 2.** `B_DEF_K1` on the base exe `cfc75039` (built from `18ddb6ea2`, `MASHED_NO_DEPLOY=1`) twice:
   step dump `80bce410` both times (= dflip's), and `B_DEF_K4` gives the same verdict. Regime B car 2 fails `c0#` 40 [13, 37],
   `c1#` 48 [17, 46], `steer#` 87 [29, 84]. Cars 1 and 3 pass. K0 `a89ff9ef`, K26 `54da2e83` (= dflip).
2. **First divergence, car 2, regime B** (`sbs.py`, original `o_reg_r13`, identical on `o_reg_r15` / `o_reg_r18` for the
   rows read): window call k = 1. The original's `c0` reads 11, 23, 35, 46 on k = 1..4, the port's 0, 0, 36, 47. The same on
   cars 1 (1, 2 vs 0, 0) and 3 (4, 9 vs 0, 0). From k = 3 the two agree within 1. The original's k = 1..3 values fit
   `c0 = err * speed * _DAT_005cd0e8` (0.0030034) with speed 280 / 563 / 861 and err about 13.5 degrees on all three.
3. **Cause of the k = 1..2 zeros, from the bytes.** `FUN_00416250`'s fresh-steer arm (`0x00416648..0x004166b4`, err in (1, 180);
   mirror `0x0041675c..0x004167cf`) runs only when `[0x0089a4ec + v*0x74] == 1` or `DAT_007f0ff4 - [0x0089a4f0 + v*0x74] >= 0xc8`
   (`0x00416608..0x00416621`). Otherwise it writes `c1 = ftol((0xc8 - el) * 0.005 * [0x0089a4f4 + v*0x74])`
   (`0x00416623..0x00416643`), which is 0 with the stamps at 0. The port (`Ai/AiStandalone.cpp:1158-1172`) is the same law. The
   difference is the clock: the exe's `DAT_007f0ff4` reads 50 / 100 / 150 / 200 on k = 0..3 (dump column `clk_0ff4`), the
   original's 43100 at k = 0 on `o_reg_r13`.
4. **The original's clock at GO.** On all 31 labelled original captures (`ORIG_LABELS.txt`) the clock is 0 on the first probed
   `FUN_00416250` call, advances 50 per call, and reads 38850..46400 on the first racing call (665..816 countdown calls before
   it). On `o_reg_r13` it is 37450 on the single state-4 call and 43100 at GO, 113 x 50 later, with no `FUN_00416250` call in
   between (state 5). Writer: `FUN_0040fc00` at `0x0040fe4f..0x0040fe5e`, skipped only in race sub-state 7 (`0x0040fe46`).
   Readers (`datarefs_7f0ff4.txt`, 26 refs): the band timers of `FUN_00416250`, `FUN_00416a30`, `FUN_00417da0` (stamp
   differences), `FUN_004103a0`, `FUN_00410860`, `FUN_00411170`, `FUN_00411600`, `FUN_00411870`, `FUN_004111c0`, and two
   absolute float reads off the race path (`0x00470df2` under `[0x007f0fd0]` in {edi, 2}; `0x00429424` into the formatter
   `0x004987b0`).
5. **The exe's default countdown** (`D3d9Render/TrackRenderer.cpp`, `countdown_ = 3.0f` at :6914, the freeze block after the
   rfx adapter) runs no AI tick and advances no clock. The clock is advanced only on racing frames (`Ai_AdvanceClock`, :5354
   before the edit). `MASHED_SUBSTATE` (default OFF) does advance it on its sub-state frames.
6. **Later divergence (report).** At k = 23 the original's car 2 enters mode 3 (`FUN_00414a70` returns 1, `ret14a70` 1 on k =
   23..38 and 44..45 in all three B captures), the U-9684 brake. On the default build `FUN_00414a70` does not run. The existing
   stack fix B (`MASHED_POS_MODEL`) + `MASHED_ROUND_GRID` + `_ORDER` + `MASHED_PATHPROG` + `MASHED_TGT3` makes the exe's car 2
   enter mode 3 at call 23 as the original, but scores B 0/3 (car 2 `c0#` 61): its contact with the parked player lands one
   physics frame after the original's (between k = 22 and 23 the original's car 2 moves -0.049 in z, the port's -0.109). The
   player's settle writer was confirmed on the original by hardware watchpoints (`o_pw1`, `o_pw2`): `FUN_004704c0` ->
   `FUN_0046f6c0(0)` (`0x00470638`) writes the slot-1 z -1.99903715 -> -1.99894500 at `0x0046fc03`; the first `FUN_0046e9e0`
   (`0x00470ad3`) copies it into slot 0 (`0x0046ea76`). That is fix B part 3's translation (d2posmodel), already ported.

## 1. The change (one knob, default OFF)

`MASHED_AI_COUNTDOWN_CLOCK=1`: on every frame of the default countdown freeze, `DAT_007f0ff4 += units` (units = the same
`dt * 3000 + 0.5` budget as `Ai_AdvanceClock`), behind the same `g_aib.loaded && race spline count > 3` gate as the racing-frame
tick. Only `DAT_007f0ff4` moves. `DAT_007f0ff8` is not touched (the original's `FUN_00418560` mode-5 branch zeroes it on every
state-5 tick, `0x00418598`, and this stand-in runs no AI tick). Code: `Ai/AiStandalone.{h,cpp}` (`Ai_CountdownClockEnabled`,
`Ai_AdvanceCountdownClock`), call in `D3d9Render/TrackRenderer.cpp` countdown block.

Exe: `f0ee223b` (knob build), base `cfc75039`. Both `MASHED_NO_DEPLOY=1`, private root `D:\MashedWT\d3bB2_root`.

## 2. Gates and predictions (written before the runs)

| gate | arm | prediction |
|---|---|---|
| G-NOOP | `f0ee223b`, knob unset: `B_DEF_K{0,1,26}`, `H_DEF`, `H_DEF-2` | byte-identical to the base: K0 `a89ff9ef`, K1 `80bce410`, K26 `54da2e83`; G-CTRL `4c81db89 / aa03f7ae / 7d3af534` twice |
| G-TOOK | knob ON, `B_CC_K1` | first dump row `clk_0ff4` = 50 * (N + 1) with N the countdown frames, N in 179..181 (9000..9100) |
| G-RAMP | knob ON, `B_CC_K{0,1,26}` | on all three cars, window calls k = 1 and 2 carry a non-zero `c0` or `c1` (the original's sign: `c0` on all 9 car-regime pairs below), within 2 of the labelled original's value (A `o_reg_r1`, B `o_reg_r13`, C `o_reg_r19`) |
| determinism | `B_CC_K1` twice, `H_CC` twice | identical |
| (b) | `B_CC_K{0,1,26}` (and K2, K4, K27, K54, K57 as extra phases) | report the per-car verdicts. No numeric prediction for B car 2 is registered: the k = 1..2 commands move every car's line, and the contact frame is not predicted |
| 13 tracks | `run13.py` with the knob | 0 RED; row count and C27 rows reported against the 870 of dflip |
| D2 x3 | `D2_CC_1..3`, alone | in the `d81a8df6` bounds, H1 >= 398/400. The D2 recipe runs the player solo (`MASHED_MEASURE_SOLO`); predicted: same values as `D2_DEF` within the dflip spread (slip 0.1890 / 0.2476..0.2482, driving-median 1912.04..1913.06) |
| G-CTRL knob ON | `H_CC` | changes the trio (report) |
| collateral | `coll.py` style, report | report only |

Report arms (no gate): the knob on top of the full U-9684 chain (`MASHED_POS_MODEL` + `MASHED_ROUND_GRID` + `_ORDER` +
`MASHED_PATHPROG` + `MASHED_TGT3`), K0 / K1 / K26, to see whether the contact frame moves to the original's.

## 3. Decision rule (registered)

Recommend `MASHED_AI_COUNTDOWN_CLOCK` default ON only if all hold: G-NOOP, G-TOOK and G-RAMP pass; no car that passes on the
default (A car 2, B cars 1 and 3) turns to FAIL in any phase; 13 tracks 0 RED; D2 3/3 in bounds with H1 >= 398/400. If B car 2
does not pass, the knob can still be recommended on those terms as a fidelity fix, with B car 2's remaining items named. No
default is flipped in this lane (orchestrator rule): the result is a recommendation.

## Amendment 1 (written before any arm of the two knobs below; the arms of section 2 had run, results in RESULT)

### A1.0 Measured since section 2 (evidence, not gates)

- Section 2's knob passed G-NOOP, G-TOOK (first-row clock 9100 = 50 x 182) and G-RAMP exactly (k = 1, 2 equal to the
  original on all 9 car-regime pairs). Its (b): A 1/3 (car 3 passes, car 2 fails `c0#` 41 [13, 35]), B 2/3 (car 2 `c0#` 39,
  `c1#` 49, `steer#` 87), C 0/3. So the section 3 rule fails (A car 2).
- With the knob on the full U-9684 chain plus `MASHED_GRID_DIR` (FUN_00408b00's spawn-yaw law; without it car 2 spawns
  0.1101 deg off and drifts 0.0022 in x by k = 21), car 2 matches the original to 0.001 through k = 22 and its contact with
  the parked player lands in the original's physics frame. The offline SAT on the original's own records decides that frame
  by a 3e-4 margin (`sat.py`, frame 836: one vertex inside, test value -0.0003).
- Two remaining differences, both from the bytes and the watchpoint capture `o_ov1`:
  1. **The type-4 contact event.** `FUN_00469df0` pushes a type-4 ring entry on every contact unless `FUN_0040e350()` is 1..5
     (`0x0046ae97..0x0046aec3`, `0x0046b04c..0x0046b078`). `FUN_00425a40` drains it with `FUN_00422ba0` right after the physics
     (`FUN_00470c70`), and the type-4 arm calls `FUN_00417370(car)` for each car with `FUN_0040e470(car) != 1`, gated by
     `FUN_0046cbb0`'s first word being 0 for both cars. `FUN_00417370` writes `[0x0089a4fc + v*0x74] = 0xb4 - 0xb4 *
     __ftol([0x0089a360] - 10.0)` = 1440 on Training. Watched on car 2: `0x0046ac8e` (+0xaec = 480), then `0x0041739b` (timer
     1440), then `0x004187ee` counting it down by 50 per AI tick to 0 at `0x004187f6`, 29 ticks. During them `FUN_00418560`'s
     tail replays the stored ctrl bytes. The original's car 2 heading moves +0.147 deg per frame through the coast (c0 = 21
     frozen at grip 0.7), the port's -3.65 (c1 = 255 live). The port omitted the push as "dev-viz only".
  2. **Frame order.** `FUN_00425a40` runs the AI tick (`0x00425a73`) before the physics of every car, player first. The exe
     steps the player before the AI tick (`TrackRenderer.cpp` StepPlayer, then the AI tick), so the tick of the contact frame
     already sees the impulse: the port's car 2 returns 1 from `FUN_00414a70` at window call 22, the original's at 23.

### A1.1 Two knobs (default OFF)

- `MASHED_AI_CONTACT_EVENT=1`: the push (one event per contact, from `g_ccLastContact` at the `0x00470bcd` call site, mode gate
  1..5) and the drain after all physics of the frame, porting the type-4 arm's `FUN_00417370` effect with both gates. Not
  ported: `FUN_004922e0` (hit sound / particles) and the pair tables `0x008995b0..` (no exe reader). Trace:
  `MASHED_AI_CONTACT_EVENT_TRACE=<path>`.
- `MASHED_AI_TICK_FIRST=1`: the AI tick phase (clock, `AiBridgeSnapshot`, `Ai_Standalone_Tick`, step dump, gates dump, heading
  assist, sub-state tail) runs before the player's physics instead of after it.

Exe: the build of this amendment's commit (sha8 recorded in RUNLOG at the first run).

### A1.2 Gates and predictions

Stack `S` = `MASHED_AI_COUNTDOWN_CLOCK` + `MASHED_POS_MODEL` + `MASHED_PATHPROG` + `MASHED_ROUND_GRID` + `_ORDER` + `MASHED_TGT3` +
`MASHED_GRID_DIR` + both new knobs. `S-E` = `S` without `MASHED_AI_CONTACT_EVENT`, `S-T` = `S` without `MASHED_AI_TICK_FIRST`.

| gate | arm | prediction |
|---|---|---|
| G-NOOP2 | new exe, no knob: `B_DEF_K{0,1,26}`, `H_DEF` | K0 `a89ff9ef`, K1 `80bce410`, K26 `54da2e83`; G-CTRL `4c81db89 / aa03f7ae / 7d3af534` |
| G-NOOP2b | new exe, section 2's knob alone: `B_CC_K1` | `5257c915` (= the section 2 exe) |
| G-EVT | `S` K1 with the trace | car 2 gets timer 1440 at its first contact with the player; the trace's first car-2 event follows the first `i=0 j=2 ret=1` |
| G-ORDER | `S` vs `S-T`, K1 | car 2's first `ret14a70 == 1` at window call 23 under `S`, at 22 under `S-T` |
| G-COAST | `S` K1 | car 2's heading change per frame over window calls 23..31 (dump rows) within +0.147 +- 0.03 deg (original +0.1475) |
| (b) | `S`, `S-E`, `S-T` on K0 / K1 / K26 | report per car. No pass is predicted for B car 2 |
| determinism | `S` K1 twice | identical |

Decision rule: unchanged from section 3, applied to each new knob and to the stack. A stack that loses a passing car is not
recommended for default ON. Every knob here stays default OFF in this lane.
