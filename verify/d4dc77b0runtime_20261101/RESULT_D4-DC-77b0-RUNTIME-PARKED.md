# RESULT D4-DC-77b0-RUNTIME-PARKED (run 28, 2026-11-01)

**Verdict: GREEN.** All three runtime legs were measured on the original (10 boots, LIVING-PC, VB-CABLE endpoint
checked before each boot). The two VALSRC rows were also measured on the exe (2 runs, Frida on `mashed_re.exe`, no build
change). Row 20 closes: v0 is 1 and v1..v3 are 2 at every FUN_004177b0 entry of a warp-launched match, on both sides.
Row 23 closes with two exe gaps confirmed by measurement, so two new default-OFF-knob ledger steps were filed
(**D4-DC-77b0-MATCHEND-SUBSTATE**, **D4-DC-77b0-ST3-PRETICK**). The 0x00897ffc watch closes VALSRC Part 1 row 1's
per-mode residual for the modes and states reached: during racing the cell is 0, and it becomes 1 only at a match end.
This step ports nothing. There is no `mashedmod/` change, no tracker change and no flip.

Base: `orch/integration` @ `c82901f6a`. Inputs: `verify/d4dc77b0writers_20261030/RESULT_D4-DC-77b0-WRITERS.md`
(rows 20, 23), `verify/d4dc77b0decode_20261030/RESULT_D4-DC-77b0-DECODE.md` (b), sibling
`verify/d4dc77b0st5orig_20261101` (method).

## Method

- **Original probe**: `re/frida/scenario_launch.py --valsrc-probe` (new on this branch, read-only).
  (1) Entry hook on FUN_004177b0. Each call reads the return RVA, 0x0063ba8c, the four cells
  `*(*(0x005f2770) + 0x34 + 4v)` (0x005f275c..0x005f2768), 0x00897ffc, 0x0067e9fc (FUN_0042f6a0's word) and
  0x007f0fd0. Rows are run-length per identical tuple, with first / last tick 0x007f0ff4.
  (2) Hardware WRITE watchpoints (DR0..DR3) on 0x0063ba8c, 0x00897ffc, 0x005f275c (v0) and 0x0067e9fc on every thread
  at arm time. Each hit logs the pc after the store, the four dwords, the ACCURATE backtrace and the first four `.text`
  dwords of a stack scan (the caller of FUN_00448700 / FUN_0040e480 comes from the scan; the ACCURATE walk stops at the
  writer's own frame).
  FUN_004177b0 runs 1 to 2 times per frame, so it is not a hot path. Nothing is written to the game.
- **Original recipe**: `--mode 10 --cars 4 --rule <r> --track 0 --poke-ctrl-slots --statediff-out <msd> --valsrc-probe`.
  Arms `o_r0_t0` (hold 240) and `o_r<r>_t0c` (hold 300 for r = 0, 150 otherwise, plus
  `--continue-rounds --continue-rounds-states 7,8,9,10,11`) use the harness's existing option. It pulses control 4 only
  while 0x0063ba8c is in 7..0xb, so a capture continues past round and match ends. Without it, `o_r0_t0` sits in
  state 7 for the rest of the hold with the tick frozen at 49300.
- **Exe probe**: `verify/d4dc77b0runtime_20261101/exe_vsprobe.py`. It spawns `mashed_re.exe` (this worktree's build, an
  unchanged `orch/integration` tree, `MASHED_NO_DEPLOY=1`) with run_arms.py's H_DEF `base_env` (Training, rule 0, game
  mode 10, `MASHED_DETERMINISTIC=1`, 14400 frames). It hooks `Ai_Standalone_Tick` (T), `AiPreTickRubberBand` (B) and
  `TrackRenderer::StartRound` (S) at their `.map` addresses, and reads the same cells plus `s_roundEnd7`. It uses the same
  four watchpoints, with each pc resolved to a `.map` symbol. Arm `x_r0_t0` is the H_DEF recipe.
  `x_r0_t0c` adds the exe's own arm-test knob `MASHED_R7_INPUT_PRESS=150` (one key press on the 150th frame of each
  round-end stretch, `exe_main.cpp:2822`), the exe analogue of the original's control-4 pulse.
- Every boot ran under `mashed_lock.py run --label D4-DC-77b0-RUNTIME-PARKED`, with private root
  `D:\MashedWT\D4-DC-77b0-RUNTIME-PARKED_root` (robocopy of `original\`; patched MASHED.exe sha256 `b9977dab...fe87`,
  `.unpatched` = the anchor `BDCAE093...`) and `MASHED_ROOT` set to it. Runner: `run_all.py`.
  `Get-PnpDevice -Class AudioEndpoint -PresentOnly` listed the three VB-Audio endpoints before every original boot that
  ran. One boot, `o_r9_t0c` attempt 1, stalled at phase 0 ("TIMEOUT waiting for menu"). Its probe never armed, it was not a piz
  collision, and it was re-run. No crash, no piz-collision signature in any log.

## Runs (FUN_004177b0 calls per 0x0063ba8c value at entry)

| arm | rule (0x007f0fd0) | calls | per sub-state | calls with 0x00897ffc = 1 | round / match ends |
|---|---|---|---|---|---|
| o_r0_t0 | 0 | 14496 | 3:677 4:1 5:224 6:196 7:13398 | 0 | 1 round end, then state 7 to the end (no input) |
| o_r0_t0c | 0 | 19680 | 3:1515 4:6 5:5152 6:10322 7:1779 8:17 0xa:213 0xb:675 | 0xb:675, 0xa:213 | 17 non-match round ends, 5 match ends, 5 frontend re-launches |
| o_r2_t0c | 2 | 9876 | 3:636 4:1 5:1792 6:6675 7:764 8:7 | 0 | 7 round ends, no match end |
| o_r4_t0c | 4 | 9130 | 3:639 4:1 5:224 6:8266 | 0 | none in 150 s |
| o_r5_t0c | 5 | 9092 | 3:746 4:1 5:224 6:8121 | 0 | none in 150 s |
| o_r6_t0c | 6 | 9717 | 5:1568 6:7497 7:645 8:6 | 0 | 6 round ends, no match end |
| o_r7_t0c | 7 | 4059 | 3:777 4:1 5:224 6:2929 0xb:128 | 0xb:128 | match end on the first round end |
| o_r8_t0c | 8 | 9016 | 3:681 4:1 5:224 6:8110 | 0 | none in 150 s |
| o_r9_t0c | 9 | 1131 | 3:756 4:1 5:224 6:1 0xb:149 | 0xb:149 | match end on the first state-6 call |
| o_r10_t0c | 10 | 1119 | 3:760 4:1 5:224 6:1 0xb:133 | 0xb:133 | match end on the first state-6 call |

Every FUN_004177b0 call in every arm returns to 0x00418905, inside FUN_00418860. Per-tuple tables: `SUMMARY_<arm>.csv`.
(Totals are from the agent counter; the run-length rows can be 1 lower, because one call can land between the row drain
and the counter read.)

## Leg 1, VALSRC row 20: the 0x005f2770 slot cells at FUN_004177b0 entry

- **Original, every arm, every match started by the warp: (v0..v3) = (1, 2, 2, 2) on all calls.** No race-path store to
  v0 fired: the watch on 0x005f275c has no hit inside a race. The case-1 block at 0x0040fec3 (stores (0,2) under
  `FUN_0042f6a0() == 0xb`) did not run, because 0x0067e9fc was 0xa (10) or 3, never 0xb.
- **Frontend re-launch (o_r0_t0c only)**: after a match end the control-4 pulses walk the menus in sub-state 9 (tick
  frozen). The watch shows FUN_00432800's loop (returns 0x004328fc) storing v0 = 0, then FUN_0042b960 (0x0042b9b5) and
  FUN_0043dfd0 (0x0043e8b1) storing v0 = 1. 0x0042f6d6 (called from 0x0043f495 in FUN_0043dfd0) writes 0x0067e9fc = 3.
  FUN_0040d270 (store 0x0040d3e7, called from FUN_0040d440 at 0x0040d452) then sets 0x0063ba8c = 1. The re-launched
  matches run on (1, 2, 0, 0) with 0x0067e9fc = 3. **No FUN_004177b0 entry saw v0 = 0**: the 0 lasts only inside
  sub-state 9, where FUN_004177b0 is not called.
- **Exe, both arms: every `AiPreTickRubberBand` entry sees (1, 2, 2, 2).** The first `Ai_Standalone_Tick` entry (call 2)
  sees (1, 0, 0, 0), and its alive-poke (`AiStandalone.cpp:2121-2126`) writes the 2s before the first B call (call 3).
  The v0 = 1 comes from `CarSlotStateSet` at load (watch hit 1, `_CarSlotStateSet+0x16`).
- **Row 20 verdict: closed for the warp-launched match. No value difference at FUN_004177b0 / AiPreTickRubberBand entry.**
  The writers RESULT's "v0: yes" (`FUN_0042f6a0() == 0xb` case 1, frontend values 0 / 2) did not materialise in any arm.
  The re-launched matches' (1, 2, 0, 0) table is the frontend's own configuration after a harness-warped first match.
  The exe has no second match (`NextRoundOrEnd` returns on `match_winner_`), so it has no counterpart to compare.
  [UNCERTAIN] `verify/d3_gatefire_20261008/RESULT_E470.md:16` measured v0 = 0 on 91/672 samples. That is not reproduced
  here (0 of 87,316 entries). Missing: that run's recipe (round type, launch route), to re-run it under this probe.

## Leg 2, VALSRC row 23: 0x0063ba8c at FUN_004177b0 entry, original vs exe

Original write sequence (watch, `SEQ_<arm>.csv`; the pc is the instruction after the store):

| pc after store | store (WRITERS #) | value | seen in |
|---|---|---|---|
| 0x0040d3f1 | 0x0040d3e7 (#1) | 1 | every load and re-launch (from FUN_0040d440, ret 0x0040d452) |
| 0x004100e7 | 0x004100dd (#10) | 2 | case 1, every match start |
| 0x00410283 | 0x00410279 (#11) | 3 | case 2, match start |
| 0x004102b6 | 0x004102ac (#13) | 4 | rule 6 only, same case-2 call as #11, so state 3 never runs a dispatcher call |
| 0x00410291 | 0x00410287 (#12) | 4 | case 2 after a round end (0x005f29b8 != 100000) |
| 0x00410391 | 0x00410387 (#14) | 4 | case 3 exit |
| 0x0040dc4a | FUN_0040dbd0 (#2..#5) | 5 | case 4 |
| 0x004104ed | 0x004104e3 (#15) | 6 | case 5 exit |
| 0x004111b0 | 0x004111a6 (#20) | 7 | round end, not a match end |
| 0x00410a78 | 0x00410a6e (#18) | 8 | case 7 exit (all 30 case-7 exits; 0x00410a5e = 9 never fired) |
| 0x0040ddad | 0x0040dda3 (#6) | 2 | case 8 |
| 0x0041064f | 0x00410645 (#16) | 0xb | match end (rules 0, 7, 9, 10) |
| 0x00410b0c | 0x00410b02 (#19) | 9 | case 0xb exit |
| 0x0040ddff | 0x0040ddf5 (#7) | 0xa | case 9 (re-launched matches of o_r0_t0c) |

Never fired: #8 / #9c-e (frontend 0x0040de1a, 0x0043dd66 / 0x0043dd93 / 0x0043f982), #9a (0x0040e57c), #9b (0x00429537),
#17 (0x00410a5e).

Exe write sequence (`WATCH_x_r0_t0c.csv`): `StartRound+0xc61` stores 0 (`TrackRenderer.cpp:7733`).
`UpdateCar+0x50d` stores 3 and `UpdateCar+0x63c` stores 4 on the first round. `UpdateCar+0x52d` stores 4 directly from 0
on later rounds. `SubStateEnter4+0xb1` stores 5 and `SubStateHeldCountdown5_C+0x229` stores 6.

Exe B entries (`SUMMARY_x_r0_t0*.csv`). x_r0_t0: 5:112, 6:220, then 13062 with `s_roundEnd7` = 1 (sub-mode 7, no
input). x_r0_t0c: 5:560, 6 with the flag down:12327, 6 with the flag up:600 (4 round ends x 150), S x 5. Its `mashed_re.log` shows
rounds 1-4 ending with `match_winner=-1` and round 5 ending in `R6 MATCH OVER t=51.7s winner=car3 (score>11 rule,
0x00410510)`.

Per WRITERS frame class (phase 5's gate is `== 6`, 0x00417aec):

| class | original | exe | differ? |
|---|---|---|---|
| countdown 3 / 4 | FUN_004177b0 runs in state 3: 636..777 calls per match start (one per tick), 1 call in state 4. Rule 6 skips state 3 | **no B call** before state 5 (first T at call 2 with 0x0063ba8c already 5) | **yes, in call presence**. Both sides are non-6, so phase 5 is off on both, but the exe never runs phases 1-4 there. New step D4-DC-77b0-ST3-PRETICK |
| state 5 | 224 per match start (2 per tick) | 112 (1 per step) | known, D4-DC-77b0-STATE5-LEG2 (L-76, closed evidence-only) |
| round end, not a match end | state 7 (1779 calls over 17 round ends in o_r0_t0c, input timing dependent), then one call in state 8 | flag up: B reads 7 (150 calls per round end, input timing dependent) | no, in the gate: both are non-6 |
| **case A, match end** | 6 -> 0xb (0x00410645), FUN_004177b0 runs 128..149 calls in 0xb with 0x00897ffc = 1, then 9, and 0xa (213 calls) on a re-launch | `NextRoundOrEnd` sets `match_winner_` and returns. 0x0063ba8c stays 6 and the flag is down: **11387 B calls with sub-mode 6 after `R6 MATCH OVER`** | **yes, confirmed.** The exe runs phase 5 for the rest of the run, where the original runs FUN_004177b0 in 0xb / 0xa with phase 5 off. New step D4-DC-77b0-MATCHEND-SUBSTATE |
| **case B, round start** | 8 -> 2 (0x0040dda3) -> 4 (0x00410287). One call in 8, none in 2 | `StartRound` stores 0, then `UpdateCar+0x52d` stores 4 before the next B call: **0 B calls with 0x0063ba8c = 0** (4 round starts) | **no**. The writers RESULT's [UNCERTAIN] closes: no exe pre-tick reads the 0 |
| time trial #9b / FUN_0040d270 #1 | #9b never fired (no mode-2 game mode was run). #1 fires once per load / re-launch before any FUN_004177b0 call | not applicable | no, in the canonical scenario |

## Leg 3, 0x00897ffc per mode (DECODE (b) residuals)

Callers from the stack scan (return address after each FUN_00448700 call):

| return | site (DECODE #) | value | sub-state at the write | arms |
|---|---|---|---|---|
| 0x00410305 | 0x00410300, FUN_004102f0 (#3) | 0 | 3 | every match start except rule 6 |
| 0x00410a7d | 0x00410a78, FUN_00410860 (#7) | 0 | 7 -> 8 | every non-match round end (rules 0, 2, 6) |
| 0x00410683 | 0x0041067e, FUN_00410510 (#4) | 1 | 6 -> 0xb | match ends of rules 0, 7 and 9 |
| 0x00410816 | 0x00410811, FUN_00410510 (#5) | 1 | 6 -> 0xb | match end of rule 10 |
| 0x00492bef | 0x00492bea, FUN_004929d0 (#8) | 1 | 9 | after every match end |

Never fired: #1 / #2 (FUN_00403250, needs `FUN_0042f6a0() == 0xb`) and #6 (0x00410a4d, case-7 winner exit).

- **Racing value: 0 in every mode run (0, 2, 4, 5, 6, 7, 8, 9, 10).** Every FUN_004177b0 call in sub-states 3..8 read
  0x00897ffc = 0. That includes rule 6, where site 3 does not run and the cell keeps its BSS 0 from load. The cell reads 1
  only in 0xb, 9 and 0xa, after a match end (sites 4, 5, 8). Site 3 resets it to 0 on the next match start.
- Residual (1) of DECODE (b), "mode 6 skips site 3 and keeps a previous 1": not reached. Rule 6 had 6 round ends and no match
  end in 150 s, so no 1 existed to carry. [UNCERTAIN] Missing: a rule-6 match that follows a match end (a frontend
  re-launch into mode 6). Static reading: site 8 (1) and then FUN_0040d270 / case 1 run on the re-launch path, and no
  site writes 0 before racing on the mode-6 path. That run would settle it.
- Residual (2), "`[0x005f29b8] == 100000` at 0x004102f2": measured. Site 3 fired on every match start that passed state 3
  (rules 0, 2, 4, 5, 7, 8, 9, 10 and each o_r0_t0c re-launch). After a round end, case 2 takes the 0x00410287 arm
  (4, not 3), so site 3 runs once per match, not once per round.
- Residual (3), "which FUN_0042f6a0() value per mode": 0x0067e9fc was 10 (the harness's warp write) in every warp-launched
  match and 3 after a frontend re-launch. It was never 0xb, so sites 1 / 2 and the case-1 v0 store stay unreached.
  [UNCERTAIN] for 0xb. Missing: a launch route that sets round type 0xb.
- Exe: `aib_ai_target_enable()` returns 0 (`TrackRenderer.cpp:251` stand-in). That matches the original on every racing
  frame measured here. It differs only in 0xb / 9 / 0xa, which the exe does not have (folded into
  D4-DC-77b0-MATCHEND-SUBSTATE).

## New ledger steps (behaviour gaps, not ported here)

- **D4-DC-77b0-MATCHEND-SUBSTATE** (default-OFF knob, PREREG first). At a match end the original stores
  0x0063ba8c = 0xb (0x00410645) and 0x00897ffc = 1 (FUN_00448700 sites 4 / 5). It runs FUN_004177b0 in 0xb (128..149 calls), then stores 9
  (0x00410b02) and 0xa (0x0040ddf5). The exe keeps 6 and runs `AiPreTickRubberBand` with phase 5 on (11387 calls in
  x_r0_t0c).
- **D4-DC-77b0-ST3-PRETICK** (default-OFF knob, PREREG first). The original calls FUN_00418860 -> FUN_004177b0 once per
  tick through sub-state 3 (636..777 calls per match start) and once in state 4. The exe makes no AI pre-tick call before
  state 5.

## Files

Committed (this folder): `RESULT_D4-DC-77b0-RUNTIME-PARKED.md`, `run_all.py`, `exe_vsprobe.py`, `summarize.py`, and per arm
`SUMMARY_<arm>.csv` / `WATCH_<arm>.csv` / `SEQ_<arm>.csv`. `re/frida/scenario_launch.py` adds the `--valsrc-probe` option.

Raw (not committed), `D:/MashedWT/D4-DC-77b0-RUNTIME-PARKED_data/`:

| file | SHA-256 |
|---|---|
| o_r0_t0.log | 0ac0e96f70cbb4522b8c24f5c8018d991c276e5db112c5ec6853ed1e20e00375 |
| o_r0_t0.msd | d1a95878a129c4892a4ac2be38eb02d4b6bbd7f3bb191e4105430a4980808f95 |
| o_r0_t0.msd.vs.csv | f737abbf74a2f2bf6dd874f9c4c16e98570407e66e581becec03c4eb075e9cea |
| o_r0_t0.msd.vsw.csv | 57a2cb718d559c5c43c6a4289b93085729cf8eb0b78fee9dc0fccecd9a1d0bb4 |
| o_r0_t0c.log | 943b2db4da6a1148a9211402c3fd59e823cea4f9a40af3f37fab8a7bb943da08 |
| o_r0_t0c.msd | 3201f7ecad371461b7ff560143dfd17a86fdc7aff54c143e7c2da7542e4579ef |
| o_r0_t0c.msd.vs.csv | ba824b41c7469a47790b172df43cb0729d18eae6cc0a26ecb918cc21dae30d70 |
| o_r0_t0c.msd.vsw.csv | 3e61caf4bd426fd30b2795f2aa82117e69512ce87e498d536e4c40608c7775ba |
| o_r10_t0c.log | ec8f9cf8e8130d03357f102bb4c75aad8246dfc9134866440555cdbe38971684 |
| o_r10_t0c.msd | f14197304049d12312f76e88e7ded476a7103fb889accf9fea754ff28f76e243 |
| o_r10_t0c.msd.vs.csv | dcd7ad638b694a23a8003820f6aa61f4c5f4d48519e9f3d35450708ef5c9b0d4 |
| o_r10_t0c.msd.vsw.csv | df86be48218a85e6a1bd7a8afa96a0ae54565a68f675c120ad6cc3bbb5dac739 |
| o_r2_t0c.log | 142ac21029ae2ae8943f5fadcdbc4a1dc3f8f640c626b87eccd190c43ecbc848 |
| o_r2_t0c.msd | 89190cdfecdd765b75b16a2851938ee2caf51562b7e2c09541cfc8fa56be47c3 |
| o_r2_t0c.msd.vs.csv | 734d961219bf8feb391ea11bba37e93bc752f83163d0b339d7b58896e8170835 |
| o_r2_t0c.msd.vsw.csv | 5321665ba2d8627a854d94dcc0625ca1681c0f4b93b2838da170a3bfca2ebb18 |
| o_r4_t0c.log | 3ac7f1d90a1efbb5a1c5b332c26661c2e18bfd850956974e91c08ee468c248d2 |
| o_r4_t0c.msd | cd883f2f9f200bcd6066a2fba348413979cea73ea2bd4962d15aaf29f63922c3 |
| o_r4_t0c.msd.vs.csv | 4c369a379d6db183c1f681c8882d075739ab9cf390b812f7da6411999bd46608 |
| o_r4_t0c.msd.vsw.csv | 41389884be3b6de3fc5c5a50c47c65f59eab18e459a8843f96e42947596c830d |
| o_r5_t0c.log | 98b51eed7fad2fe0c882e3e72de3a404507d7a931f2d6a7e3ba9298a0c163c8d |
| o_r5_t0c.msd | dc632f836f850d38e687e518589223c8cac0e5ceee544bfcb1ba2386618a587f |
| o_r5_t0c.msd.vs.csv | 819f3159c3a5b998c1bc61b5146ad329d67b65b58eef04f0eeb244c370768042 |
| o_r5_t0c.msd.vsw.csv | a4b01ef37bac051507d706075185af1025acd76110dd340ef9ce48948cad7b1f |
| o_r6_t0c.log | f563e84bfbbfbe522f92eac2246d3eef68bf064db1cc9e33f5168b7654f79c4a |
| o_r6_t0c.msd | bae3d7c2f98f99340c3af31920de3b2893512837482939065e9f6153edf7e068 |
| o_r6_t0c.msd.vs.csv | c06eb66fc8d3a10a8075383b55b8f2725865548b3776b002d07d574a6ea13cae |
| o_r6_t0c.msd.vsw.csv | 37c48e4cf3feb756f45bcf01cc7bc305a71000cc534a9b0b32e93ac442c4f4ed |
| o_r7_t0c.log | 02ae53bf1f304e42358b8d0fee8b88ebe6bf1f8d2065b12a28c6f4e615895faa |
| o_r7_t0c.msd | a1ffbcca13e9c0ef893f255b27e076ab9cae03d973e3370ab274892b18049d90 |
| o_r7_t0c.msd.vs.csv | add8a8df0a8ead88bbbb8f6b4c976122020b3e39859e15f6da88292da3fbd5fe |
| o_r7_t0c.msd.vsw.csv | 579946f1a1969879543e15edc7b781bce6e3f38860fe5090ab97f0dc3ef8c726 |
| o_r8_t0c.log | f37127da660da05ee16fba7be9e6187da181172fc8019d31639fc08e0ca99e46 |
| o_r8_t0c.msd | f09a193c8b863f092fe5088d732d134cf2ad454606c20f7569a6e07959f2e33f |
| o_r8_t0c.msd.vs.csv | 7dec3414ce9dd86c6b8e828a65ff74ee0d89a1606f0caf51fbca6a22ab6a3776 |
| o_r8_t0c.msd.vsw.csv | c4c334a90e40fc2e3a938a72f1f4e77926580504eb58daeac6c005cfd24c789a |
| o_r9_t0c.log | cb2fef04e72fd496a1dca93c529977fe4c67f2f294cc6e29da84a000ff561545 |
| o_r9_t0c.msd | bba17680dbbb7c1950e71b24acb1e8ca5812c99559dfc01f54e64f235a08edd3 |
| o_r9_t0c.msd.vs.csv | 78bf15c748b121901a53e50461cb4279feb2db91ad069fad6fde91866b15c010 |
| o_r9_t0c.msd.vsw.csv | a3382ea125ae72a5c37e968b194ff524146e0fccd9d5f2883db6244ce025afc5 |
| o_r9_t0c_bootstall.log | 4e0b9ea97842cd78f09d7abeeaaf5bd1b856fdd2a0e0323279b5fe971d20ba96 |
| x_r0_t0.log | d064e07d2e46b8448968fcce9f7b9f610ce99907a1d405d64b41f0f448b6897a |
| x_r0_t0.vs.csv | 235eb9451b68bfaa3bc8479611bddaec77418b3b5223b9dfd4f9a5a47a9637e5 |
| x_r0_t0.vsw.csv | 50a394a9cef07fd4fe83d0cbf66e79add9e1b5502ffdd360cdf606b208a039eb |
| x_r0_t0c.log | fe153dd5429f299b30b8b8b09b53a8ab7b95076205ac4086f2ea1413dd73685b |
| x_r0_t0c.vs.csv | 208e4501a59836d5a2c20ab6f8af022b770c5abd78b26310dde6af59316997d8 |
| x_r0_t0c.vsw.csv | 4351f2e1219eee6653f05c7359dcefac44c0e1d434cd34ec805cdfd08b423c7c |
| x_r0_t0c.mashed_re.log (the exe's own log of that run, copied from the private root) | 5fe3b38779640d8cb0b676bd225fcbcced433119ce7f08aac6b10f5c79512535 |

## Gates

Not run. This step changes no build input. The code changes are `re/frida/scenario_launch.py` (a Python/Frida harness)
and Python scripts under `verify/`. None is compiled into `mashed_re.exe` or `mashed_re_dev.asi`. The exe probed here was
built from the unchanged `orch/integration` tree, so G-CTRL and B stay at the `merge_train.json` refs.

## Re-merge onto orch/integration 6a997bfa3 (2026-11-02, LIVING-PC)

Merge train 20261101_212449 hit a conflict in `re/frida/scenario_launch.py` against D4-DC-77b0-STATE5-ORIGREAD.
Hand merge `a36d8a97b` (`git -c merge.autoStash=false merge orch/integration`). All 5 hunks kept both sides: the
VALSRC probe (`vsProbeArm` / `vsProbeDrain` / `vsWatchDrain`, `--valsrc-probe`, `<out>.vs.csv` / `<out>.vsw.csv`) and
ORIGREAD's sub-state-5 probe (`st5ProbeArm` / `st5ProbeDrain`, `--st5-probe N`, `<out>.st5.csv`). Both attach an entry
hook on FUN_004177b0. Two `Interceptor.attach` on one address are allowed in Frida, and the two probes are separate options.
Checks: `py_compile` OK, the extracted `AGENT` JS passes `node --check`. No other file conflicted.

Build inputs: `git diff --name-only 6a997bfa3 a36d8a97b` lists only `re/frida/scenario_launch.py` outside `verify/`,
so the merged tree compiles exactly what orch/integration 6a997bfa3 compiles. Rebuilt anyway (`MASHED_NO_DEPLOY=1`,
Build OK), `mashed_re.exe` sha256 `a2e1c8aad8eebba55ff99b9fc510417a3a0c6ef5529eed80462fbac7f9f1692a`.

Gates: **not measured.** Two attempts of `run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` (under `mashed_lock.py`,
`MASHED_ROOT` = `D:\MashedWT\D4-DC-77b0-RUNTIME-PARKED_root`) exited with code 3 after 1 to 4 s per arm. `mashed_re.log` shows
`InitD3D9: adapter mode 3840x2160 fmt=22 refresh=60` and every `CreateDevice` combination failing with
`hr=0x8876086A` (D3DERR_NOTAVAILABLE). This is machine-wide on LIVING-PC since about 09:20 local (12:20Z): the same failure is in
the newest `mashed_re.log` of `mtrain_root` (merge train), `MATCH-PORTSTATE-RECORDS_root`, `HB-GATE-FLAKE-GAMEFLOW_root` and
`D4-FLIP-MATCHEND-ST3PRETICK_root`. The last successful `CreateDevice` seen (`MATCH-S09-REMERGE_root`, 08:17 local) reported
`refresh=120`, so the display mode changed between those times. Since the merged tree's build inputs equal 6a997bfa3's, the
refs in `merge_train.json` (G-CTRL `8284be46/ba407a16/31fe043e`, B `1d5bf4f9` x3) hold by construction. The merge train
re-measures them on enqueue. The original-side smoke of the merged harness was not run for the same reason (the d3d9 shim
also needs a device).

Raw (not committed), `D:/MashedWT/D4-DC-77b0-RUNTIME-PARKED_data/`:
- `gates_remerge2.log` sha256 `81e5438296a7e092c90e4f6261c7a8245005e60e9992737cf6284f0f169159c6`
- `gates_remerge2_mashed_re.log` sha256 `780ccd1d97ecb2e8f1e863bbe50717b61882ecb7eb79ca48351e846883ece03a`
- `build_remerge.log`, `gates_remerge.log` (first attempt, same failure)
