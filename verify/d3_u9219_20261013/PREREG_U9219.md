# PREREG: U-9219, the mode-7 refdist bridge (`Race/SpectatorDistances.cpp`, `0x00442a60`)

Written and committed before any new capture or code change. Branch `work/u9219`, base `0bc7ccd0`.

## 0. What was read before writing this (exploratory, committed data only)

These reads shaped the gates, so they are listed. They are NOT the confirmatory measurement.

1. Bytes (`re/tools/decomp_pc.py`, pool slot 1, read-only): `FUN_00442a60` zeroes `0x008989b0..bc` at entry,
   takes the pair from `FUN_0040e180` (`out1` = inner index, `out2` = outer, `<=` so the later pair wins ties,
   3-D distance via `FUN_004c3ac0`), reads race_pct `FUN_00408ad0` (`0x008a96ec + v*0x30c`) for both, applies the
   wrap rule (`_DAT_005cc730` 80.0, `_DAT_005ccd6c` 20.0, `_DAT_005cc568` 100.0), takes the pair member with the
   higher race_pct as reference when its `FUN_0046cbb0` out1 (`0x00881f90 + v*0xd04`) is 0, else the other when
   its out1 is 0, else no store. Stores `0x008989a8` (reference) and `0x008989c8` (other), then for every car with
   `FUN_0040e370` (`*(*0x005f2770 + 0x34 + 4v) != 0`) and `FUN_0046c7b0` (`0x008815a4 + v*0xd04`) `== 1` writes
   `0.8 (_DAT_005cc9bc) * |XZ(ref) - XZ(v)|` from the `FUN_0046d4a0` matrix (`0x00881ec8 + v*0xd04 +
   [0x00881f48 + v*0xd04]*0x40`, `+0x30` / `+0x38`). Its only caller is `FUN_00446520` at the `0x00446520` race
   branch (`FUN_00442a60()` after the `FUN_0041ef60` loop), itself called from `FUN_00448220`.
2. The port (`Race/SpectatorDistances.cpp:150-205`, `Race/RaceCamera.cpp:182-236`) transcribes the same selection,
   with `std::sqrt` for `FUN_004c3ac0`, race_pct from the D-11072 bridge (`TrackRenderer.cpp:5858-5869`, written
   AFTER `RaceComputeDistancesTick()` at `:5836` in the same frame), and called every frame from `UpdateRace`.
3. `o_c30a` / `o_c30b` (RESULT_M4): the 890 "pre-freeze" car-2 calls include **734 phase-3 calls**
   (`DAT_0063ba8c == 3`) where the cars are parked and the reference is car 0. The driving phase (`ba8c == 6`)
   is ~175 calls per car. The two runs are identical over it.
4. Inferring the reference per consumer call from `cars958` positions and `refdist` (`refdist[v] == 0.8 *
   |XZ(c) - XZ(v)|`): in both sides the reference is a member of the bytes-rule pair on every driving call; the
   original's reference is car 1 on 159 / 175 of car 1's calls, car 2 on 4 / 175 of car 2's; the exe `M4_ON`'s
   reference is car 2 on 46 / 175. Car 2 is the AI car farthest from the player on 4 / 175 original calls and
   41 / 175 exe calls.

So the exploratory reading says the selection logic matches and the inputs differ. That is a diagnosis to be
tested, not a result. The gates below test it on new captures and on the port's own compiled code.

## 1. Instruments

- **Original:** `scenario_launch.py`, the `o_c30a` argv unchanged (`--track 0 --mode 10 --cars 4 --car 0
  --poke-ctrl-slots --statediff-out ... --statediff-car 1 --statediff-aistep --c30-probe 20000 --hold 60`) plus a
  NEW read-only `--refdist-probe` (entry + leave hook on `0x00442a60`, ~60 calls/s, below the hot-path rule).
  Entry: clock `0x007f0ff4`, `DAT_0063ba8c`, per car `act` (`*(*0x005f2770 + 0x34 + 4v)`), `alive`
  (`0x008815a4 + v*0xd04`), `st` / `st2` (`0x00881f90` / `0x00881f94 + v*0xd04`), race_pct (`0x008a96ec +
  v*0x30c`), the `FUN_0046d4a0` matrix translation x, y, z. Leave: `0x008989a8`, `0x008989c8`, `refdist[0..3]`.
  No new forced input. Two runs, `o_rd1` / `o_rd2`. Piz-collision AVs (`+0x1509b4`, `+0xe66db`) are re-run.
- **Port replay:** `MASHED_REFDIST_REPLAY=<o_rd*.tsv>` feeds each captured call's inputs to the port's own
  selection + distance code and writes the comparison (`MASHED_REFDIST_REPLAY_OUT`). This needs the bridge's
  body split into a pure core plus the live input fill. The split must not change any value: **G-CTRL** below.
- **Window W:** original, from the first `0x00442a60` call with `DAT_0063ba8c == 6` until the first call where any
  car's `st != 0` or `ba8c` leaves 6 (the round-end freeze). The port replay scores every call in W.

## 2. Gates and the decision each outcome triggers

| gate | statistic | pass rule |
|---|---|---|
| **G-ORIG-DET** | `o_rd1` vs `o_rd2`: per-car count of in-W calls with `refdist[v] == 0` | equal within 2 per car; otherwise that spread is the noise floor used below |
| **G-PROBE** | the probe's own `refdist[v]` at `0x00442a60` leave vs the `c30` trace's `refdist` at the next consumer call of v (same run) | equal bits on >= 95 % of in-W consumer calls (proves the probe reads the array the consumer reads) |
| **G-REPLAY-SEL** | port core on the original's inputs: reference and other index vs `0x008989a8` / `0x008989c8` | equal on >= 99 % of in-W calls, both runs |
| **G-REPLAY-DIST** | port core: `refdist[v]` vs the original's | `|d| <= 0.01 + 0.08 * ref` on >= 99 % of in-W car-calls (8 % = the registered RW-sqrt magnitude gap, hooks.csv `0x0040e180` demotion) |
| **G-REFDIST** | per-car in-W count of consumer calls with refdist 0: exe (M4 knob-ON stack) vs original | `|exe - orig| <= max(3, 2 * G-ORIG-DET spread)` for cars 1, 2, 3 |
| **G-MODE7** | per-car `FUN_00414c30 == 2` count, exe M4 stack vs original 49 / 0 / 0 | car 2 and car 3 `<= 5`; car 1 report-only (RESULT_M4 finding 2 is a separate mechanism) |
| **G-CTRL** | default build `run_h4` recipe hashes | step `ae67b13a`, gates `c8317ce2`, ptrace `31db9384`, and `M4_ON` (`d57dd28d / b18f69f2 / 803b179c`) unchanged by the refactor |
| (b) regime A | ctrl-byte bands | report-only, knob-ON stack and default |

Decisions (pre-committed; the mechanism is NOT pre-committed, see memory pre-register-the-decision):

1. **G-REPLAY-SEL and G-REPLAY-DIST pass:** the bridge computes the original's function on the original's inputs.
   No bridge change is made. U-9219's cause is upstream of the bridge; it is re-scoped to the input that differs,
   named from the data (§3 attribution). G-REFDIST and G-MODE7 are then expected to fail unchanged; that does not
   reopen the bridge.
2. **G-REPLAY-SEL or -DIST fails:** the bridge is defective. The failing calls are classified by cause; the fix is
   made in the bridge behind the existing `MASHED_MODE7` path only (default unchanged, G-CTRL must hold), then
   G-REFDIST, G-MODE7, (b) and collateral are scored on 3 exe runs. U-9219 closes only if G-REFDIST and G-MODE7 pass.
3. **G-PROBE fails:** the probe does not read what the consumer reads; no replay verdict is taken from it.
4. **G-CTRL fails:** the refactor changed behaviour; it is reverted and the replay is not used.

## 3. Attribution (report-only, run only under decision 1)

Per in-W call, which input decides the reference: (a) the pair (positions), (b) the race_pct order within the pair,
(c) the `st` gate. Counted on both sides (original from `o_rd*`, exe from `M4_ON.gates.csv` + `M4_ON.c30.tsv`).
Report the car-2 pace gap (car 2 vs car 1 progress along the opening straight) as numbers, no cause beyond what the
captures show.
