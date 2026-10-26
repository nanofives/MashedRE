# RESULT D3-AI-VEHICLES (2026-10-26)

**Verdict: DECISION.** Knob-OFF part GREEN: `MASHED_AI_VEHICLES` lands default OFF and the six default gates equal
`re/orchestrator/merge_train.json` refs. Knob ON: all six PREREG predictions hold. G-CTRL, B, D2 and the 13-track
table show no change. The audio wire's cars 1..3 move from engine class 0 (`eng1`) to car 0's class (`eng2` for
ADVANTAGE). Flipping the default is a decision row (L-30 below).

PREREG: `verify/aiveh_20261026/PREREG_D3-AI-VEHICLES.md` (commit `b7bc457b7`, committed alone before code and arms).
Code: `4c51f4dfe` (`Race/RaceSession.cpp`, `AiVehiclesOn()` + 6 lines in `RaceSession::Begin`). Runners: `6204a5217`.
Exe: `mashedmod/build/mashed_re.exe` sha256 `2099d36a0c92b39d...`, built with `MASHED_NO_DEPLOY=1` (no `.asi` deploy).
Private roots `D:/MashedWT/D3-AI-VEHICLES_root` and `_root13` (robocopy of `original\`). Every boot ran under
`mashed_lock.py run --label D3-AI-VEHICLES`, and the D2 arms ran under `run --solo`. Only PIDs this lane spawned were
killed (wall-kills by `run_port.py` / `run_arms.py`).

## The original's selection site (`FUN_0040d110`, capstone on `MASHED.exe.unpatched`)

- group = (id / 6) * 6, where id = dword +4 of the event record `param_1[param_2]` (`0x0040d143`, `0x0040d162`,
  `/6` at `0x0040d165`..`0x0040d173`, `*6` at `0x0040d1bf`..`0x0040d1c2`). Modes 10 and 6 re-read the record index
  from `FUN_00431d70` (`0x0040d175`..`0x0040d1bd`).
- 4-slot loop `0x0040d1de`..`0x0040d20d` (`ebp` = `0x007f1a1c` to `0x007f1a5c`, stride `0x10`, no participant gate):
  car i id = `DAT_007f1a1c[i*4]` + group (`0x0040d1e3`..`0x0040d1e6`), or i + group in mode 0xb (`0x0040d1f2`), then
  `FUN_00420420(i, id)` (`0x0040d1fb`).
- So the vehicle type is one per race and every car is in car 0's 6-livery group. The port's `carIndex` is the type
  (`kVehicles`), and the livery is already per car (`TrackRenderer::ResolveCarLiveries`). The knob sets
  `numCars = 4` and `cars[1..3].carIndex = cars[0].carIndex`.

Readers of the two fields: `WireRaceBegin` only (`RaceSession.cpp`, `AudioWire.cpp:260-273`). No sim code reads them.

**Arm coverage.** The G-CTRL, B and 13-track recipes use `MASHED_CAR=1` (`run13.py:79`, the H_/B_ env). That path
loads the car directly (`exe_main.cpp:9506`) and never calls `RaceSession::Begin` (sole caller `GameFlow.cpp:299`),
so those arms cannot exercise the knob. Their equality follows from structure, and they are scored to show it. The menu
path (W, and D2 through the same `MASHED_GOTO=6` launch) does run `Begin`, and W1's 4 `engine_class` rows show the knob
live there. `RaceSession`'s own `[race]` lines go to `log/mashed_re.log`, a folder the private roots lack, so they are
not a usable witness.

## Scores

| # | prediction | result |
|---|---|---|
| 1 | G0 (knob OFF) = refs | **PASS 6/6**: G-CTRL `5943c5f1/b493262b/09551e6a`, B `3a03670c/e85ea6fb/53778664` |
| 2 | G1 (knob ON) = refs | **PASS 6/6**: the same six hashes (H_AV / B_AV_K0/K1/K26) |
| 3 | W0: 1 `engine_class`, `wire_race` 1, cars 1..3 `eng1` v1..v3 + `eng1d` v5..v7 | **PASS** exact |
| 4 | W1: 4 `engine_class ADVANTAGE -> eng2`, `wire_race` 4, cars 0..3 `eng2` v8..v11 + `eng2d` v12..v15, no `eng1` | **PASS** exact |
| 5 | D2 x3 (knob ON) PASS 3/3 in band | **PASS 3/3**: each arm slip1500 0.1942, slip2000 0.2492, drvmed 1896.74, H1 398/400 median 1333.9 |
| 6 | 13 tracks (knob ON): 0 RED, gates PASS, verdicts = reference | **PASS**: 39/39 rows CITED, 0 RED, `INSTRUMENT GATES: PASS`, every row byte-identical to `verify/colllut_20261026/tracks_score.txt` |

D2 delta vs the lead's default reference (0.1942/0.2492/1896.74): 0 / 0 / 0. All three arms are over 1600
`motion_diag` lines (1635 / 1631 / 1631), so none was re-run. Arm 1's log hash differs from arms 2 and 3 (`e55c5708` vs
`c9d721d7`, 4 more lines, the wall-clock bound) with the same scores.

W0 and W1 match outside the engine rows: same `threetwoone` v65, `go` v95, 2 pickups on v100, `guard_closed_total`
barrier 5 / other vehicle 1, `pod_count` 2. This closes the D4-WSJ-WIRE PREREG prediction 4c residue under the knob:
the class for cars 1..3 is now the class of the race's vehicle.

Small extracts committed here: `tracks_score_AV.txt`, `d2_score_AV.txt`, `W0.audio_events.csv`, `W1.audio_events.csv`.

## Expected ON hashes (for the flip merge)

G-CTRL `5943c5f1/b493262b/09551e6a`, B `3a03670c/e85ea6fb/53778664`, the current refs unchanged. The flip does not
move `merge_train.json`.

## Residual (named, out of scope)

Car 0's vehicle type in the port comes from `MASHED_CAR_SEL` (default 0, ADVANTAGE). The original takes it from the event
record dword +4 (`0x0040d162`), with the mode 10 / 6 re-read through `FUN_00431d70`. This step copies car 0's type to
cars 1..3. It does not port the record source.

## Decision (L-30, filed)

Flip `MASHED_AI_VEHICLES` to default ON? (a) flip: gates, D2 and the 13-track table are unchanged, and the audio wire's
cars 1..3 get the race vehicle's engine class (FUN_0040d110 4-slot loop). (b) keep OFF. Recommend (a).

## Raw data (`D:/MashedWT/D3-AI-VEHICLES_data`, SHA-256)

| file | sha256 |
|---|---|
| arms/H_DEF.step.csv | 5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415 |
| arms/H_DEF.gates.csv | b493262bed8ea6ecad4399f0ebaa53b902965423e74bfa07359188559fff479f |
| arms/H_DEF.ptrace.log | 09551e6a5b1b456b3421a0e1aac768ced7851c072c99a9341a22ca966cdfe3e1 |
| arms/B_DEF_K0.step.csv | 3a03670c3991b702af9fd44b1d49dbdb4f91d6be00876723cb1ba7891895b83f |
| arms/B_DEF_K1.step.csv | e85ea6fb266223f9eed3a90bf9aa30ea50291fd552ac3881d3bfbea843a759f8 |
| arms/B_DEF_K26.step.csv | 5377866478b33a9827ae5c343b8861189f8f0a838785bf2b83da5f6118931e2d |
| arms/H_AV.step.csv | 5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415 |
| arms/H_AV.gates.csv | b493262bed8ea6ecad4399f0ebaa53b902965423e74bfa07359188559fff479f |
| arms/H_AV.ptrace.log | 09551e6a5b1b456b3421a0e1aac768ced7851c072c99a9341a22ca966cdfe3e1 |
| arms/B_AV_K0.step.csv | 3a03670c3991b702af9fd44b1d49dbdb4f91d6be00876723cb1ba7891895b83f |
| arms/B_AV_K1.step.csv | e85ea6fb266223f9eed3a90bf9aa30ea50291fd552ac3881d3bfbea843a759f8 |
| arms/B_AV_K26.step.csv | 5377866478b33a9827ae5c343b8861189f8f0a838785bf2b83da5f6118931e2d |
| arms/D2_AV_1.motion_diag.log | e55c5708939a56d43affb0a93298524ffc45ba69324491be8fcd2b62fca0b34b |
| arms/D2_AV_2.motion_diag.log | c9d721d7ac717523f69558983ca147fd932ca23cd6570bb0b14cfd2edde6e37c |
| arms/D2_AV_3.motion_diag.log | c9d721d7ac717523f69558983ca147fd932ca23cd6570bb0b14cfd2edde6e37c |
| port/W0.audio_events.csv | d556c6487e46096c073dbd6275cd5141c59126d114413c40388aa63b491bcb87 |
| port/W1.audio_events.csv | 4e8175aae5adb04ed3605b186bfa90cc73e48360c05497190631a811db443c34 |
| port/W0.log | 0868c2c27ebf7b5b590f7a198f5eb45a6b9c0d1f51f2be370854fc089afa07ae |
| port/W1.log | 84723fcd6d2ec0ed53c7988dcdba847bcab9f2d222560f75f30cf5cfb4913718 |
| arms/RUNLOG.txt | 5ebaa60176db981df77bf47564cbabf545775ac3d1257eb5a6c7d650c4802108 |
| port/RUNLOG.txt | 9e47d786d6fc6c02cb9434da6161da58866a2fed8e9eafb6288212587a463f5f |

13-track captures: `run13/<NN_Track>/` plus `run13_status.json`. The scored table is committed as `tracks_score_AV.txt`.
Process note: the first G0 attempt died copying `H_DEF` (the `arms/` output folder did not exist). It was re-run in
full, and only the re-run is scored.
