# RESULT u9749: the first A4 frame after a contact skip (2026-10-17)

Branch `work/u9749` from `work/a4order` `99d6e17d5`. Model claude-opus-5-5, account2. Exes built with `MASHED_NO_DEPLOY=1`,
snapshots on `D:\MashedWT\u9749_data\exe`: `r1_7369a149` (record dump only), `r2_ba366371` (= `081ea1672`, dump + knob).
Private roots `D:\MashedWT\u9749_root` / `_root2` / `_root13`. Original references: d3bB2's `o_pw1.msd` (player record) and
`o_c2r1` (car 2), no new original-side capture. PREREG `PREREG_u9749.md`, committed in `081ea1672` before any knob arm.

## 0. Verdict

**U-9749 is resolved: nothing in A4 / A5 / A6a differs.** The player's heading diverged during its coast because the port
does not run the steer-0 heading assist `FUN_0046c7d0` on car 0 (U-9743). The "-23.81 against -6.56" of a4order compared
`+0x9e4`, which the original rewrites after A4 every frame (`0x004710a1..0x004710da`) and the port by default does not; the
velocity vectors agree to 0.3.

Ported behind **`MASHED_HEADING_ASSIST_PLAYER=1` (default OFF)**. On its own, on the plain default, it passes the default-ON
rule: (b) A 2/3 B 2/3 C 0/3 against 1/3, 2/3, 0/3, no passing car turns to FAIL; D2 3/3 in bounds; 13 tracks 866 rows 0 RED.
**Recommendation: default ON (user decision).** On the `S` + A4-order stack it brings back the original's second player<->car 2
contact (car 2 coasts 1340.509 on calls 61..69, original 1341.140).

## 1. How it was found

New diagnostic `MASHED_RECDUMP=<path>` / `MASHED_RECDUMP_A4=<path>` (`VehiclePhysicsRun.cpp`, default OFF): MSD1 dumps of one slot
(`MASHED_RECDUMP_SLOT`), once per race frame after every car's physics (the analogue of the original capture's render-tick sample,
`FUN_004c1be0`) and at the A4 site. With it on, `B_SA_K1` keeps `75255391`. Tools: `recdiff.py` (every differing dword at chosen
calls), `fieldtrace.py` (chosen fields per call); frames aligned on the first frozen coast (original k = 23 1237.908, port dump
index 23 1237.921, shift 0).

- k = 31 (last coast frame, `recdiff_player_31_32.txt`): position and velocity match (x 0.3035), the forward vector does not:
  `+0x9d4` -0.05019 / -0.07133, `+0x9dc` -0.99879 / -0.99745.
- `trace_player_heading.txt`: the yaw rate the contact left (`+0x148`) is bit-identical on k = 22..33 (0.09344, x0.9345 per
  frame); `+0x9bc..+0x9c4` are 0 on both sides; the first heading step after the contact is equal (-0.00888 on forward x), then
  from k 23 -> 24 to 30 -> 31 the original turns a constant 0.00264 (+-0.00002) per frame less, about 0.151 deg per frame.
- A constant-rate rotation with an unchanged yaw rate is `FUN_0046c7d0`'s form (rotation of the live matrix about (0,1,0),
  `0x0046cb10` -> `FUN_004c4d20`), run by `FUN_00424eb0` for cars 0..3 between `0x00425a73` and `0x00425a8d`.

## 2. Code (`081ea1672`)

`Race::HeadingAssist_Player(dt, steer0, steer1)`: mask bit 0x2 (`MASHED_HEADING_ASSIST_PLAYER=1`), `FUN_0046c7d0(0, dt)` with the
AI loop's gates (`FUN_0040e370`, `FUN_0046c7b0`) and the player's steer bytes of the frame (the bytes `StepCar` maps `io.steer`
to), called right before `VehiclePhysics_StepPlayer`. `AssistCar` is the verbatim AI-slot transcription with an optional input
block for car 0.

## 3. Gates

| gate | measured | verdict |
|---|---|---|
| G-NOOP | knob off on `ba366371`: `B_SA_K1` `75255391`, `B_DEF_K1` `5257c915`, `H_DEF` `490c4b2d / 1fdd0ea6 / 46088ac4` | PASS |
| G-HEAD | `B_SAHR0_K1` player forward x k = 24..31 within 3e-5 per frame of the original; `+0x9d4` k = 31 -0.05022 (orig -0.05019); through k = 36 within 5e-5 | PASS |
| G-SPEED1 | `+0x9e4` k = 32 1231.288 against 1214.104: FAIL as registered. The velocity vector matches: \|v\| 1214.038 / 1214.019 at k = 32, within 0.3 through k = 40 (`+0x9e4` is not the same quantity on the two sides, section 0) | FAIL as registered, explained |
| G-COAST2 | `B_SAH_K1` car 2: 1340.509 on calls 61..69 (original 1341.140 on 61..69); position within 0.002 through call 79 (`SBS_SAH_K1_car2.txt`) | PASS |
| determinism | `B_SAH_K1` = `B_SAHR0_K1` `c517e79f`; `D2_P_1` = `D2_P_3` `0413e529` | PASS |

## 4. D3 (b) (`arms/b/SCORE_B.txt`; DEF / A / SA rows from a4order on the same source minus this knob)

| arm | A | B | C | note |
|---|---|---|---|---|
| DEF | 1/3 | 2/3 | 0/3 | A: car 3 passes |
| **P** (knob alone) | **2/3** | 2/3 | 0/3 | **A: cars 2 and 3 pass**; B car 2 `c1#` 55 `steer#` 89 (DEF `c0#` 39 `c1#` 49 `steer#` 87) |
| A (a4order) | 1/3 | 2/3 | 0/3 | A: car 2 |
| AHP (A4 order + P) | 1/3 | 1/3 | 0/3 | A car 3 and B car 3 fail |
| SA (a4order) | 0/3 | 0/3 | 0/3 | |
| SAH (SA + P) | 0/3 | 0/3 | 0/3 | B car 2 `c1#` 64 `steer#` 101 `abs_steer~` 22.5 |

Default-ON rule (no car that passes on DEF turns to FAIL): **P qualifies**; AHP does not.

## 5. G-CTRL, D2, 13 tracks (knob alone)

| item | measured |
|---|---|
| G-CTRL `H_P` | `a34714b4 / 0f8ef6df / 852a255d` |
| D2 `D2_P_1..3` (each alone, `arms/d2/SCORE_D2_u9749.txt`) | 1627 / 1626 / 1627 lines; slip 0.1890 / 0.2480 x3; driving-median 1909.37 x3 (n 1417 / 1416 / 1417); H1 398/400 x3. 3/3 in bounds |
| 13 tracks (`run13.py`, `cites_rres.json`) | 39 frames, 866 rows, 0 RED, 39/39 CITED. One change against the default: Training 884 loses its `C27-CAR-PART-OFFSET` row (21 -> 20 rows, 213 -> 215 draws matched) |

## 6. Residual (recorded on U-9745)

With the second contact in the original's frame, car 2's steer during that override drifts (6, 4, 2, then 7 on the other byte)
where the original holds 6 on calls 61..65, and the port's override timer samples 50 lower than the original's on both
contacts (1390 / 1440 at calls 23 and 60). That is the next divergence on this stack.

## 7. Tracker transaction (re-classify, no C-level change; `apply_trackers.py`)

U-9749 RESOLVED; notes on U-9743 (ported behind the knob, measured) and U-9745 (residual of section 6); hooks.csv notes on
`0046c7d0`, `00424eb0`; one CHANGELOG line. No new U-id.

## 8. User decisions needed

1. `MASHED_HEADING_ASSIST_PLAYER` default: recommended ON (opt-out to be added at the flip). It is the original's behaviour
   (`FUN_00424eb0` covers car 0), it passes the default-ON rule, and D2 stays in bounds.

## 9. Follow-ups

- After the flip: the override-timer phase and the steer drift during the second override (section 6, U-9745).
- `MASHED_RECDUMP` is a general tool now: any port record against any `scenario_launch --statediff-out` capture.

## 10. Provenance

RUNLOGs: `arms/RUNLOG.txt` (record-dump arms on `7369a149`), `arms/b`, `arms/h`, `arms/d2` (exe sha8, knobs, pid, exit, output
sha8); every line read back for the knobs its tag names. In git: PREREG, this RESULT, scripts, scores, `B_SAH_K1.step.csv`,
`B_P_K0.step.csv`, the D2 `motion_diag` logs (force-added), `recdiff_player_31_32.txt`, `trace_player_heading.txt`,
`SBS_SAH_K1_car2.txt`, `tracks_p_scores_diff.txt`. Everything else (the `.msd` dumps included) is on `D:\MashedWT\u9749_data\verify`
(`MOVED_OFF_GIT.txt`, sha8 + size); 13-track outputs on `D:\MashedWT\u9749_data\tracks\p`.
