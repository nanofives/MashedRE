# RESULT D4-DC-77b0-MATCHEND-SUBSTATE (run 30, 2026-11-01)

**Verdict: GREEN.** The knob `MASHED_MATCHEND_SUBSTATE` (default OFF) ports FUN_00410510's match-end stores, FUN_004111c0
case 0xb and a live FUN_00443080. With the knob set, the exe goes 6 -> 0xb at the rule engine's match end, and 0x00897ffc
goes 0 -> 1 in `VehicleSeedWritePair`. Every later `AiPreTickRubberBand` entry reads 0xb with ffc = 1: 11296 calls, and 0 in
sub-mode 6 after the match end, where the knob-off run has 11275. With an action-4 press, 0xb exits to 9 after b91c passes 2.0.
Default gates unchanged. The default flip is filed as a decision row.

Base `orch/integration` `6d9d01a7f`. PREREG `66927411c` (committed alone), knob `58ade91d3`. Exe sha256 `7b389f80...8e0e`
(`MASHED_NO_DEPLOY=1`, the shared `.asi` not touched). Private root `D:\MashedWT\D4-DC-77b0-MATCHEND-SUBSTATE_root`
(`MASHED.exe.unpatched` = anchor `BDCAE093...`). Every run under `test-slot` + `mashed_lock.py run`. No original-side boot:
the original reference is RUNTIME-PARKED's measurement, the static reading is PREREG section 1.

## Code (`58ade91d3`)

- `TrackRenderer.cpp`: `MatchEndSubstateEnabled()`. `aib_ai_target_enable()` returns `[0x00897ffc]` under the knob
  (0x00443080). `MatchEndConclude(esi)` = LAB_0041062a's stores (0x00410645, 0x0041064f, 0x00410655, 0x0041065f, 0x00410669,
  FUN_00448700 at 0x0041067e / 0x00410811, 0x0041068d / 0x00410693, 0x00410819 / 0x0041081f), called in the rule engine block
  on `r != 0`. `MatchEndCaseB()` = 0x00410ab0..0x00410b25, called once per `UpdateCar` after the AI tick and physics, before
  the race tail. It skips the store frame. The fadd sum is compared unrounded (a double), as fcomp compares the x87 ST.
- `exe_main.cpp`: arm-test `MASHED_MATCHEND_PRESS=<n>` (one press on the n-th cook frame in 0xb).
- Named gaps, as in PREREG section 3: the powerup teardown (FUN_0045bed0 / FUN_0045bf30 / FUN_00458f80) and FUN_0044df80,
  case 9's FUN_0040ddb0 and 0xa (the exe keeps running its race frame in 9, with phase 5 off), the ctrl shadow copy
  (0x007f1501 / 0x007f1502 stay 0, so a held byte counts as a press), and `Fi_GameMode()` = 6 in 0xb / 9.

## Arms and predictions

| arm | result | prediction |
|---|---|---|
| H_DEF | `8284be46 / ba407a16 / 31fe043e` | P0 holds |
| B_DEF_K0 / K1 / K26 | `1d5bf4f9` x3 | P0 holds |
| B_ME_K0 / K1 / K26 | `1d5bf4f9` x3 | P1 holds |
| H_ME try 1 | `937d89d8 / 277b95f7 / 1925768a`, **void**: see below | not scored |
| H_ME try 2 | `8284be46 / ba407a16 / 31fe043e`, no `R7IN` line | P1 holds (= P0) |
| x_off | B: 5:560, 6 flag up:600, 6 flag down:12444. Round 5 ends with `RULE-EVAL r=4` (call 4663), then 6 for the rest. ffc = 0 throughout | P2 holds (RUNTIME-PARKED x_r0_t0c's shape) |
| x_me | rows up to call 4663 identical to x_off (29 run-length rows equal, the 30th truncated at 4663). Watch: `UpdateRace+0xd25` 6 -> 0xb and `VehicleSeedWritePair+0x25` ffc 0 -> 1, both at call 4663. Then T / B 11296 calls each in (0xb, ffc 1). 0 B calls in 6 after 4663, no 9 | P3 holds |
| x_mep | as x_me to 4663, then 150 B calls in 0xb, the press on cook frame 150, `UpdateCar+0x3816` 0xb -> 9 at call 4963 with b91c = 2.4833312. Then 10959 B calls in (9, ffc 1) | P4 holds |
| x_me, x_mep | each ran its 14400 frames and exited on its own. No crash, no hang. The player is stepped through `Ai_Standalone_Tick` after the match end (ffc = 1) | P5 holds |

H_ME try 1 is void. Its step dump equals H_DEF's row for row through line 12211 (frame 11769, deep in round 1's state-7
hold), with `tgt_7ffc` = 0 on every row. Then `R7IN byte0xa slot=0 val=0xff test=0 det_frame=12648 type=2` shows a real
keyboard Enter: action 4's key, slot 0, not an arm-test press. It ended the hold (`R7X EXIT det_frame=12648 round 1 -> 2`), so
the run played a second round, which H_DEF does not. H_DEF has no `R7IN` line. The press is external input reaching the
exe's keyboard read during the run, not the knob: the knob changes nothing before a match end, and none happened in try 1.
H_ME was re-run (try 2).

Original comparison (RUNTIME-PARKED leg 2 case A and leg 3): at a match end the original runs FUN_004177b0 in 0xb with
0x00897ffc = 1 (128..149 calls, exit timed by the harness's control-4 pulses), then 9, where FUN_004177b0 is not called.
x_me / x_mep reproduce the 0xb reads (value and ffc) and the 9 store. The exe's AI tick still runs in 9 (named gap), with
phase 5 off as in 0xb. Note: x_mep logs `MATCHEND 9` twice in the 9-store call, because two entries of the slot table
0x007f1a14 hold idx 0 and the loop does not break after a store (0x00410b1a), as in the original.

## Decision row

Flip `MASHED_MATCHEND_SUBSTATE` to default ON (opt-out `MASHED_NO_MATCHEND_SUBSTATE`)? Default gates do not reach a match
end, so G-CTRL and B are expected to hold. Recommendation: flip.

## Files

Committed: `PREREG_D4-DC-77b0-MATCHEND-SUBSTATE.md`, this file, `run_arms.py`, `run_me.sh`, `exe_vsprobe.py`,
`summarize.py`, `SUMMARY_` / `WATCH_` / `SEQ_` for x_off, x_me, x_mep.

Raw (not committed), `D:/MashedWT/D4-DC-77b0-MATCHEND-SUBSTATE_data/`:

| file | SHA-256 |
|---|---|
| mashed_re.exe | 7b389f80408971a8d784ca77b43887fd01f89cd3ead3ba5026148d2df0568e0e |
| x_off.vs.csv | eff87873c0a92942b53d928c86825d560bbdb1e7b385ce0b3513eb4dda1d4147 |
| x_off.vsw.csv | a9d1854c65a3e2b6e06306ce1e364af3dd2f1432f764184c687d94220b8632b5 |
| x_off.mashed_re.log | df3cae2a8bb663d8a8641af04f27e2cd154545d84a06ab55573c3ee14e9fc1cf |
| x_me.vs.csv | 2fb6feac2e3920b01f52378435c2bd6571313ae3d0c818fa6ebe3b4d232d3e2f |
| x_me.vsw.csv | 485dd5d4ff263ecb67a8c76ddd2b9e5fc29073b85d22b268fc13867fc33bb875 |
| x_me.mashed_re.log | 694d983c0cf8f067fa6699e0f8149dc7ee94463b29c4e7270455c4e4a7d59bba |
| x_mep.vs.csv | ef689108ebaa31d904b485292d480c09dac634faaba54bb5261773d9f1b804af |
| x_mep.vsw.csv | 7bde7036c20617917445ef5f3549167cbb8b16dd81e0d2702d61bd17f2afe516 |
| x_mep.mashed_re.log | 6f4813b300513775aba94ae61aca1552e1f6fcabfd946005b0d9c8c3a75a5d98 |
| arms/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| arms/H_DEF.gates.csv | ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb |
| arms/H_DEF.ptrace.log | 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 |
| arms/B_{DEF,ME}_K{0,1,26}.step.csv (all six) | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 |
| arms/H_ME_try1.step.csv | 937d89d84f696261cff23da8b69d0365f9a14638f469bb2a1e9bff8983f0f28b |
| arms/H_ME_try1.gates.csv | 277b95f7bfc57df3ac55ca04914f7990c05324dc46ecb10f809737d90456e629 |
| arms/H_ME_try1.ptrace.log | 1925768a9203c0a5195b25c82d97a7359001cf0d92c9ec02c820c3faa61ef2ae |
| arms/H_ME_try1.log | 5519da70220a4553b61118462d6025c7723fa2a529506fe29eee5a77065fde3c |
| arms/H_ME.step.csv (try 2) | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| arms/H_ME.gates.csv (try 2) | ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb |
| arms/H_ME.ptrace.log (try 2) | 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 |

## Gates

Knob unset, this exe: G-CTRL `8284be46 / ba407a16 / 31fe043e`, B `1d5bf4f9` x3 = the `merge_train.json` refs. Knob set: the same (H_ME try 2, B_ME_*).
