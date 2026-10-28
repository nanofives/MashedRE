# PREREG D5-PARTMORPH-PRODUCER: a producer of record state 0 (rec+0x298 == 0) in the standalone

Branch `orch/D5-PARTMORPH-PRODUCER` from `orch/integration` 0d1285dc7. Committed alone, before any arm.
Input: `RESULT_D5-PARTMORPH.md` section 4 (the law behind `MASHED_PARTMORPH` is dormant: the port's
`LibRw::CarLodState` returns only 1 or 2). Standalone only: no original launch (no audio endpoint since
19:07Z). All addresses below are from `MASHED.exe.unpatched` (SHA-256 BDCAE093...), decomp through
`re/tools/decomp_pc.py` (pool clone) and capstone on the file bytes.

## 1. Static decode

### 1.1 The writers

`FUN_0041ef60` @ `0x0041ef60` is the only setter besides car load (`FUN_00420420`, `0x00420482` = 0):
`*(int*)(0x0063dc78 + v*0x2ac) = state`. Its five call sites, with the pushed state read from the bytes
(the decomp drops the arguments):

| Site | Push | Where |
|---|---|---|
| `0x004486d9` | `ebx` = 0 (`0x00448279 xor ebx,ebx`, no later write before `0x004486d7`) | `FUN_00448220` tail, loop v = 0..3 |
| `0x00446547` | `0x00446541 push 0` | `FUN_00446520` entry, loop v = 0..3 |
| `0x0044696b` | `0x00446965 push 1` | `FUN_00446520`, `fld [ebp-0x1ac]; fcomp [0x005ce1d0]` taken branch (the port's `CarLodState`) |
| `0x0044697b` | `0x00446975 push 2` | the same compare, other branch |
| `0x004469ae` | `0x004469aa push 0` / `0x004469ac push 0` (car 0, state 0) | `FUN_00446520`, after the 1/2 loop |

### 1.2 When each runs (one director call per sim tick)

- `FUN_0040fc00` (the sub-state tick, called after every `FUN_004111c0` sub-state handler: case 6
  `0x004112cd FUN_00411170` then `0x004112d5`, case 7 `0x004112dd FUN_00410860` then `0x004112e6`) runs
  `FUN_0040da50` at `0x0040fc75` (per car `FUN_00420870` at `0x0040dad0`, which reads rec+0x298 and runs
  `FUN_0041f770`) and then `FUN_0040d470(0)` at `0x0040fc7c` (the director, which writes rec+0x298). No
  branch skips either call between `0x0040fc00` and `0x0040fc7c`. So the state a car is drawn with on
  tick N is the one the director wrote on tick N-1.
- `FUN_0040d470`: returns at once when `DAT_0063ba8c == 3 && FUN_00405430() != 0`. `FUN_00405430`
  (`0x00405430`) returns 0 when `DAT_00639d78 == 0`, which it always is in the standalone (no
  camera-path clip module, `Race/CameraEntries.cpp:29`). Then: `(DAT_007f10a1 == 0 || DAT_007f1561 != 0)`
  and `DAT_0063ba88 == 0` reach `FUN_00448220`. The standalone writes none of the three (image pad, 0), so
  the director always reaches `FUN_00448220`. The `FUN_004414b0` path (no state writes) is not reachable.
- `FUN_00448220`: `DAT_007f1a50 == 1` returns `FUN_00441990` (no state writes; 0 in the standalone).
  `s = FUN_0040e350()` (= `DAT_0063ba8c`).
  - `s == 6`: `n` = count of v in 0..3 with `FUN_0040e370(v)` (`*(PTR_005f2770 + 0x34 + v*4) != 0`),
    `FUN_0046c7b0(v) == 1` (record +0x004) and `FUN_0046cbb0` first out (record +0x9f0) `== 0`.
    `n != 0`: `FUN_00446520`. `n == 0`: the tail.
  - `s == 7` or `s == -1`: the tail.
  - any other `s`: `FUN_00446520`.
  - Tail: `0x004486d7..0x004486e5`, all four cars state 0.
- `FUN_00446520`: all four cars 0 (`0x00446541`). Then
  - `DAT_007f0fd0 == 5 && FUN_00405890() != 0` (`0x00405890`, collect total != 0 and done == total): no
    further write, all four stay 0.
  - else when `(s == 6 && FUN_0042f6a0() != 0xb) || (FUN_004464c0(), cam[7] == 0)`: the 1/2 loop, then
    car 0 = 0 when `FUN_0042f6a0()` (`DAT_0067e9fc`) `== 2` or `DAT_007f0fd0` is 5, 10 or 9 (`0x00446985..0x004469a8`).
  - else: all four stay 0. In the port `cam[7]` (`DAT_00897ffc`) is 0 (`FUN_00448700(0,0)`,
    `TrackRenderer.cpp` SubStateCameraChain note), so this branch is not reachable and the 1/2 loop always runs.

### 1.3 The producer, as the port can run it

Per sim tick, at the end of `TrackRenderer::UpdateCar` (after the late `UpdateRace`, which raises the
round-end sub-state 7 as `FUN_00411170` does before `FUN_0040fc00`):

```
s   = aib_game_sub_mode()                (the port's FUN_0040e350 host: 7 under the round-end flag, else 6)
pre = countdown_ > 0                     (pre-GO: the original's 3/4/5, which never take the count test)
tail = (s == 7 || s == -1) || (s == 6 && !pre && n == 0)
all0 = tail || (rule == 5 && CollectAllDone(rulep_))        rule = live DAT_007f0fd0
car0 = !all0 && (mode == 2 || rule == 5 || rule == 10 || rule == 9)   mode = live DAT_0067e9fc
```

The draw (the carvis callback in `RaceSubmit`) uses the decision of the PREVIOUS tick: `all0` gives
state 0 for every slot, `car0` gives slot 0 state 0, otherwise the existing `CarLodState` 1/2.
Before the first latched decision the draw keeps `CarLodState`.

## 2. Port (one knob, default OFF)

- `MASHED_CAR_STATE0=1`: the producer above. Off: no read, no write, the code path is skipped.
- `MASHED_DBG_CAR_STATE0=<csv>` (diagnostic, default unset): `L` rows (one per tick: id, s, pre, n,
  rule, mode, collect-done, all0, car0) and `D` rows (one per car draw: the decision id used, slot,
  `CarLodState` value, final state).
- `MASHED_DBG_PARTSTATE0` stays a diagnostic force applied after the producer.

## 3. Arms (exe built from this branch, `MASHED_NO_DEPLOY=1`, runner `run_arms_s0.py`)

| Arm | Recipe | Knobs |
|---|---|---|
| H_DEF, B_DEF_K0, B_DEF_K1, B_DEF_K26 | the merge-train gate recipes | none |
| H_S0 | G-CTRL recipe (Training, mode 10, rule 0, 14400 frames) | `MASHED_CAR_STATE0`, its log, `MASHED_PARTMORPH` + its log |
| B_S0_K0 | B recipe, ring skip 0 | the same |
| B_M2_K0 / B_M2S0_K0 | B recipe with `MASHED_GAME_MODE=2` | none / the same as H_S0 |

## 4. Gates (fixed before any arm)

- **P1 default unchanged**: H_DEF and B_DEF_K* hashes = `merge_train.json` refs (G-CTRL 5943c5f1 /
  364350ae / 52052890, B 3a03670c / 9fcdbd3a / 53778664).
- **P2 the knob does not move the sim**: H_S0 step / gates / ptrace = H_DEF; B_S0_K0 step = B_DEF_K0;
  B_M2S0_K0 step = B_M2_K0.
- **P3 decision replay**: an independent Python model of section 1.3 over the logged L inputs gives the
  logged all0 / car0 on every L row (0 mismatches), and every D row's final state = 0 when its decision
  is all0, 0 for slot 0 when car0, else its `CarLodState` value (0 mismatches). D rows use the decision
  id of the tick before the last tick that ran before the draw.
- **P4 reachable in default play**: H_S0 has at least one round-end window (L rows with s = 7), all four
  slots draw with state 0 on the frames after it, and `law_check.py` (D5-PARTMORPH G4) passes on H_S0's
  `pm.csv`.
- **P5 the car-0 branch**: B_M2S0_K0 has car0 on every post-countdown L row and slot 0 draws with state 0
  on every D row whose decision is car0, slots 1..3 never 0 there. `law_check.py` passes on its `pm.csv`.

A RED on P4 or P5 that is the recipe's reach (no round end, mode 2 not taken by the route) is reported as
that, not as a law failure. Raw data: `D:/MashedWT/D5-PARTMORPH-PRODUCER_data`.

## 5. Named gaps (not ported here)

- Car load sets state 0 (`0x00420482`). Draw frames between car load and the first director call keep
  `CarLodState` here. [UNCERTAIN] whether the original draws any car between those two points (state 3's
  entry block calls `FUN_0040d470(1)` at `0x00410313` before its first `FUN_0040fc00`; rounds 2+ enter at 4).
- The result camera's own motion (`FUN_00448220` tail, `0x0044835d..0x004486c8`) is not ported. Only the
  state it writes is.
- Match end (`FUN_00410510` != 0): the port keeps sub-state 6 and uses the count test. [UNCERTAIN] the
  original's sub-state on those frames was not decoded here.
