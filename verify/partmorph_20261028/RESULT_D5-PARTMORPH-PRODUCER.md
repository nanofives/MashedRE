# RESULT D5-PARTMORPH-PRODUCER: a producer of record state 0 in the standalone

Verdict: **GREEN** on the PREREG gates P1..P5. Behind the new knob `MASHED_CAR_STATE0=1` (default OFF), the
standalone now writes record state 0 where the original's director does. With `MASHED_PARTMORPH=1` the
part 0x12 morph / part 0x14 wobble law runs in default play. On the G-CTRL recipe that is from the round-1
end (tick 399) on, for all four cars. Neither knob moves the sim.

Branch `orch/D5-PARTMORPH-PRODUCER` from `orch/integration` 0d1285dc7. PREREG `087f35a96` (committed alone,
before any arm). Code `7b911e62c`. Exe `mashedmod/build/mashed_re.exe` SHA-256
`ee598fe477be2cf0b734ee449153f497b2c551086bdec7a186789c6b023516f9` (worktree build, `MASHED_NO_DEPLOY=1`,
rva-lint NEW=0). No original launch (no audio endpoint). Original-side evidence is the static decode only.

## 1. Decode (full detail with an address for every claim: PREREG section 1)

- The pushed state at each of `FUN_0041ef60`'s five call sites was read from the bytes, because the decomp
  drops the arguments. It differs from the note D5-PARTMORPH left: **`FUN_00446520` sets all four cars to 0
  on entry** (`0x00446541 push 0`) and only then runs the 1/2 distance loop. So it is not only car 0 that
  can be 0 there. When `DAT_007f0fd0 == 5 && FUN_00405890()` the loop is skipped, and all four stay 0.
- `FUN_00448220`'s tail (`0x004486d7`, `push ebx` with `ebx = 0` from `0x00448279`) sets all four to 0 on
  sub-state 7 / -1, or on sub-state 6 with no counted car (slot present, record +0x004 == 1, +0x9f0 == 0).
- Tick order: `FUN_0040fc00` runs `FUN_0040da50` (`0x0040fc75`, per car `FUN_00420870` reads the state) and
  then `FUN_0040d470(0)` (`0x0040fc7c`, the director writes it). A car is drawn with the previous tick's
  decision.
- In the standalone the director always reaches `FUN_00448220`. The gates that could divert it
  (`DAT_00639d78`, `DAT_007f10a1` / `DAT_007f1561` / `DAT_0063ba88`, `DAT_007f1a50`) are all 0 there.

## 2. Port

`TrackRenderer::CarState0Latch` runs at every exit of `UpdateCar` (a scope guard, because the countdown
frames return early). That is after the late `UpdateRace`, which raises the round-end flag the way
`FUN_00411170` does before `FUN_0040fc00`. It evaluates PREREG section 1.3 from the port's live state:
`aib_game_sub_mode()`, `countdown_`, the slot table, `VehiclePhysics_RecordI32(v, 0x004 / 0x9f0)`, live
`DAT_007f0fd0` / `DAT_0067e9fc`, and `CollectAllDone(rulep_)`. `CarState0Apply` (in the carvis callback)
applies the previous tick's decision over `CarLodState`. `MASHED_DBG_CAR_STATE0=<csv>` logs both sides.

## 3. Gates (runner `run_arms_s0.py`, checker `s0_check.py`, outputs `GATES_S0.txt`)

| Gate | Arm(s) | Result |
|---|---|---|
| P1 default unchanged | H_DEF, B_DEF_K0 / K1 / K26 | G-CTRL 5943c5f1 / 364350ae / 52052890, B 3a03670c / 9fcdbd3a / 53778664 = `merge_train.json` refs. PASS |
| P2 knobs do not move the sim | H_S0, B_S0_K0, B_M2S0_K0 | H_S0 = H_DEF on all three files. B_S0_K0 3a03670c = B_DEF_K0. B_M2S0_K0 5868f742 = B_M2_K0. PASS |
| P3 decision replay | H_S0, B_S0_K0, B_M2S0_K0 | 0 L-model mismatches (14398 + 1198 + 1198 rows), 0 D-state mismatches (57596 + 4796 + 4796 rows), 0 decision-id mismatches. PASS |
| P4 reachable in default play | H_S0, B_S0_K0 | Round-1 end at tick 399 (sub-state 7 to the end of the run, no input to leave it). All four slots draw state 0 on those ticks (13998 rows each on H_S0). `law_check.py` G4 PASS: 55992 state-0 rows on H_S0, 3192 on B_S0_K0, vertex lerps exact (41995 / 2395 lines, mismatch 0), DFF pair 3.31e-10. PASS |
| P5 car-0 branch | B_M2S0_K0 (`MASHED_GAME_MODE=2`) | car0 on every L row without all0 (0 misses). Slot 0 draws state 0 on all 483 car0 decisions, slots 1..3 never do. G4 PASS on 1018 rows (slot 0), lerps exact. PASS |

Deviations from the PREREG, stated plainly:

- `s0_check.py`'s decision-id rule was amended after the first H_S0 check. The first four D rows are drawn
  before any L row, carry id -1 (correct by construction), and the rule `last L id - 1` gave -2. With the
  rule set to -1 before the second L row there are 0 mismatches. No model or state rule changed.
- In these recipes sub-state 7 lasts from the round end to the end of the run. The port leaves 7 only on
  a button press (`verify/r7joypad_20261025/RESULT_D3-CF-R7JOYPAD.md`), and the unattended recipes press
  none. So the state-0 window is long here. It follows the port's existing sub-state host and was not
  re-derived.
- B_M2S0_K0: from tick 483 the count `n` drops from 1 to 0 (only slot 0 is in the slot table in this
  route, `MATCH-SEED ... slot=1,0,0,0`), so the decision turns all0 by the count rule. [UNCERTAIN] which
  of record +0x004 / +0x9f0 changed at tick 483. The log keeps `n` only, not the two fields.
- The mode-2 `pm.csv` has rows for slot 0 only, although slots 1..3 draw (D rows). The FUN_0041f770 step is
  skipped for those copies on this route. That is outside this step's code and was not investigated.

## 4. Named gaps (PREREG section 5, unchanged)

- Car load sets state 0 (`0x00420482`). Draws before the first director call keep `CarLodState` here.
- The result camera's own motion (`FUN_00448220` tail) is not ported. Only the state it writes is.
- Match end keeps sub-state 6 in the port and uses the count test. [UNCERTAIN] the original's sub-state
  there was not decoded.
- What the decode adds over the row's framing: state 0 is not only a result-camera effect. In the original
  it also covers every rule-5 tick after all collectibles are done, and car 0 on every tick in game mode 2
  or rule 5 / 9 / 10. The 8 W1 librwrest captures (mode 10 / rule 0, racing ticks) hit none of these, which
  is consistent with their 0 state-0 rows.

## 5. Decisions for the lead

- Default flip of `MASHED_CAR_STATE0` (and with it `MASHED_PARTMORPH`, which stays dormant without it) is a
  decision row. Both moved no sim hash on any arm here. The visible change is parts 0x12 / 0x14 and LOD 0
  on all cars after a round end, and on car 0 in mode 2 / rule 5 / 9 / 10.

## 6. Raw data (not committed), `D:/MashedWT/D5-PARTMORPH-PRODUCER_data/arms`

| File | SHA-256 |
|---|---|
| H_DEF.step.csv / H_S0.step.csv | 5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415 |
| H_DEF.gates.csv / H_S0.gates.csv | 364350aea89110f291569f42fd7ae0de19704a6b3c95e6f29d7e26b4af9ce7e4 |
| H_DEF.ptrace.log / H_S0.ptrace.log | 520528908f8dc61f538902ad09a8ccb5ca46fe23d112859a436da6ace7eab757 |
| B_DEF_K0.step.csv / B_S0_K0.step.csv | 3a03670c3991b702af9fd44b1d49dbdb4f91d6be00876723cb1ba7891895b83f |
| B_DEF_K1.step.csv | 9fcdbd3a6901025544e377132c575910aa5cd001faba802c1e7e8a9ac0d2886f |
| B_DEF_K26.step.csv | 5377866478b33a9827ae5c343b8861189f8f0a838785bf2b83da5f6118931e2d |
| B_M2_K0.step.csv / B_M2S0_K0.step.csv | 5868f7423503dfac9b020149e99c0187045adf8ed5a715fd01fcc5ce955ef25b |
| H_S0.s0.csv | 6db2fa07e32696cbf7dab3816af1dd38c113ee9e0e4e136ead10e19c093c81b7 |
| B_S0_K0.s0.csv | 78748966546ba8d97bac80a26bc94d6794458757d306967dff6d18a02e0e70ae |
| B_M2S0_K0.s0.csv | 86031da361652adeb24ba2f8e5e2d1e32cd70ed8cb0f21125951f09fd2ae4457 |
| H_S0.pm.csv / .vtx | 83cf13927960213fd48a3e60e39e5c0271817c929cff8a2a9458511e53533bfd / dcde7482019dbad9cc5e674a4fc30ab02bfdb1c11f4f8f9de7108175d54f9185 |
| B_S0_K0.pm.csv / .vtx | 3b752d8b436a36435ac48e197f783005cade927efc74b276f5ca00ce549d1909 / d35cb4fc85fe65da206fa0afb52216ec12b8bd84d6040d0689c377190fcb370d |
| B_M2S0_K0.pm.csv / .vtx | a3f57f423dffb16ba073745469193a18870cca0026cb7e9cda871b5ed6fa7f65 / 4a42b84a34772a80a461615b889f53dafc67827ccc6e5efa50903ba94e7f6871 |
| RUNLOG.txt | 1d5d8f56ca3082d06ae60a8c546c8db83c79f14be4d2709b1ea840faba219945 |

## 7. Trackers

None touched. `0x0041ef60`, `0x00448220`, `0x00446520`, `0x0040d470` keep their rows. This is a standalone
decision replay against a static decode, not a Frida diff, so it supports no C-level promotion. The memory
note "Record state 0 = result cameras only" is superseded by section 1 (entry write `0x00446541`).
