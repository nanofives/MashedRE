# RESULT: D3-SUBSTATE-PREGO-ALIVE (L-53 (a))

Date 2026-10-29. Branch `orch/D3-SUBSTATE-PREGO-ALIVE` from `orch/integration` 34235b24c. PREREG
`PREREG_D3-SUBSTATE-PREGO-ALIVE.md` committed alone at 14872d1a0, before any arm. Standalone only, no MASHED.exe
launched, no Ghidra MCP, no .ps1 edited, no tracker touched, no default flipped.

## 0. Verdict first

> **DECISION. D2 does not return to the band with MASHED_SUBSTATE on. The alive change works as designed (the gate
> now reads 1 for the player) and moves nothing: D2_SUB is 0.2016 / 0.2591 OUT / 1791.85 OUT on 3 of 3, byte-identical
> to the control build without the change and to the FLIP's own arms.** The FLIP's static cause (section 5 of its
> RESULT) is refuted. The measured difference from the opt-out is the order of the player's spawn-settle and the
> launch release on the GO frame (section 4). Default gates on the branch exe equal the refs.

| gate (post exe 677bfe79, default, no knob) | result | ref |
|---|---|---|
| G-CTRL `H_DEF` | 5943c5f1 / 364350ae / 52052890 | same |
| B `B_DEF_K0/K1/K26` | 3a03670c / 9fcdbd3a / 53778664 | same |
| (b) vsum | 3/3 on all three arms | 3/3 |
| (e) ai_speed_env --check K0 | 3/3 PASS, launch 1425.7 / 2052.5 / 2055.0 | same |
| D2 `D2_DEF_1..3` | 0.1942 / 0.2492 / 1896.74, all in, 1635 / 1635 / 1603 lines | same |
| 13 tracks | 39 of 39 rows identical, INSTRUMENT GATES: PASS, all 13 exit 0 on attempt 1 (tracks_score.txt sha256 a3c14c41..., the reference file's own sha) | verify/hbfullcheck_20261028/tracks_score.txt |

## 1. What ran

| exe | content | sha8 |
|---|---|---|
| pre (control) | 34235b24c + FLIP amendment 1 (knob-gated) + `SUB` stack (09e2c504b) | d814b26d |
| post (candidate) | pre + the alive change (ac4efceff) | 677bfe79 |

Runner `verify/savelink_20261023/run_arms.py`, new stack `SUB` = `MASHED_SUBSTATE=1`, `MASHED_P3_RESET_PAIR=1` (the
FLIP's seeded knob set), `A4_D2_WALL` ported from the FLIP (default 90, unchanged). Private root
`D:/MashedWT/D3-SUBSTATE-PREGO-ALIVE_root`. Every run inside `test-slot` and
`mashed_lock.py run --label D3-SUBSTATE-PREGO-ALIVE` (D2 and the 13 tracks `--solo`).

The change (exe arm only, reachable only with `MASHED_SUBSTATE`): the three `FUN_0046c7b0` sites call
`::VehicleSlotGetter(v)` (record `+0x4`) instead of `aib_alive(v)`:

| site | file |
|---|---|
| `0x00410330` (FUN_004102f0 phase 3, before `0x0041033e` FUN_0046baa0) | D3d9Render/TrackRenderer.cpp |
| `0x0041042c` (FUN_004103a0 loop 1, before `0x0041043a` / `0x00410441`) | Race/SubStateMachine.cpp `CarAlive` |
| `0x0041048d` (FUN_004103a0 loop 2, before `0x0041049b`) | Race/SubStateMachine.cpp `CarAlive` |

`SubStateHost::car_alive` and its `aib_alive` binding are removed (no other reader). Plus a one-shot
`[substate] state5 alive=` log line (exe only, SUBSTATE only).

## 2. Predictions

| id | result | verdict |
|---|---|---|
| P1 | `[substate] state5 alive=1,0,0,0` on D2_SUB_1..3 (post). The second half (a `launchrev release car=0` line) was mis-specified: that line is written only by the stand-in's `LaunchRev_Release` (Vehicle/LaunchRevCharge.cpp:281-291), and the sub-state path calls `LaunchRevRelease46d780` directly at `0x0041049b`. It cannot appear under SUBSTATE whether or not the release runs | gate PASS, release witness invalid |
| P2 | D2_SUB_1..3 (post): 0.2016 in / 0.2591 OUT / 1791.85 OUT on 3 of 3 | **FAIL** |
| P3 | default gates = refs (table above) | PASS |
| P4 | D2_SUB_1 (pre): 0.2016 / 0.2591 OUT / 1791.85 OUT. motion_diag sha 10b584f0 = the FLIP's `D2_DEF_4` byte for byte | PASS |

Report-only:
- D2_SUB (post) motion_diag sha8 23f1dbf4 / 56dcfd8d / 56dcfd8d = the FLIP's D2_DEF_3 / D2_DEF_1 / D2_DEF_2. Post
  D2_SUB_2 is a byte prefix of pre D2_SUB_1 (`cmp`: EOF on the shorter file at line 920). The change moved no byte of D2.
- Line count 917 / 920 / 920, under `d2_min_lines` 1600. The FLIP already showed 102 s arms give the same ~920, so no
  102 s arm was re-run: the line count is a property of the sub-state race, not of the wall clock.
- With SUB, `B_SUB_K0/K1/K26` 1d5bf4f9 x3 and `H_SUB` 8284be46 / c33e1c11 / 31fe043e: identical to the FLIP's expected
  flip refs. The change is a no-op on every SUB arm measured.

## 3. Why the alive change is a no-op on D2

Post's log has `alive=1` for car 0 on the first state-5 call, so the hold, the charge and the release at `0x0041043a` /
`0x00410441` / `0x0041049b` now run for the player. The pre control's motion_diag already starts with `bf8=2` on its
first row (the release state), as does post's, and neither logs the stand-in's release. That is consistent with the
release also running in the pre build. **[UNCERTAIN]** whether `aib_alive(0)` read 0 on the pre build's state-5 frames:
the pre build has no alive log. Evidence missing: a pre-build run with the log. It does not change the verdict, because
with the gate measured at 1 the outcome is byte-identical.

## 4. What does differ from the opt-out (measured, not yet tested as a fix)

First rows of motion_diag (reseed / sp / bf4 / bf8 fields only):

| row | opt-out (FLIP `D2_NOSUB_1`) | SUB (post `D2_SUB_1`; pre identical) |
|---|---|---|
| 1 | `settle slot=0 order=1 settleOnly=1` | `settle slot=0 order=1 settleOnly=0` |
| 2 | reseed=1 sp=0.00 bf4=3000 bf8=0 | reseed=1 sp=0.00 bf4=2900 bf8=2 |
| 3 | `launchrev release car=0 bf4 3000 -> 3000 bf8 0 -> 2` | reseed=0 sp=0.00 bf4=2700 bf8=2 |
| 4 | reseed=0 sp=0.00 bf4=2800 bf8=2 | reseed=0 sp=0.64 bf4=2500 bf8=2 |
| 5 | reseed=0 sp=0.64 bf4=2600 bf8=2 | reseed=0 sp=0.75 bf4=2300 bf8=2 |
| 6 | reseed=0 sp=0.75 bf4=2400 bf8=2 | reseed=0 sp=0.87 bf4=2100 bf8=2 |

The opt-out's settle step runs with the car unreleased (bf8 0) and the release follows it. Under SUB the settle step
runs already released (bf8 2) and decrements the charge there (2900). From then on the speeds match row for row
(0.00 / 0.64 / 0.75) while SUB's charge sits 100 higher (2700 / 2500 / 2300 against 2800 / 2600 / 2400).

Under the stand-in, the GO frame runs `Vehicle::VehiclePhysics_SpawnSettle(0, ...)` BEFORE `LaunchRev_Release()`
(TrackRenderer.cpp, the `countdown_ <= 0.f` block, d2engage fix B part 4, `MASHED_POS_ORDER`). Its comment records
what happens otherwise: "a post-release settle opened A6a's front-wheel gate under +0xbf8 == 2 and spent one 200-unit
decrement of the 3000 charge" (PREREG_d2engage.md 0.1-0.3). Under SUBSTATE the release comes from state 5 at
`0x0041049b`, and the settle then runs inside the first StepCar after it (`settleOnly=0`, bf8 already 2 on that row).
That is the order d2engage fixed for the stand-in, reintroduced by the sub-state path. The measured charge offset is
100, not the 200 the d2engage note names, so the mechanism is not the same in every detail. **[UNCERTAIN]** that the
order is the whole of D2's shift: not tested. Evidence missing: a SUB arm with the settle placed before the state-5
release.

## 5. Options (DECISION)

- **(a) recommended**: a step D3-SUBSTATE-SETTLE-ORDER. Under MASHED_SUBSTATE, run the player's
  `VehiclePhysics_SpawnSettle` before the state-5 release loop (`0x00410470`, before `0x0041049b`), the place
  d2engage put it for the stand-in. PREREG: D2_SUB x3 back in band, default gates unchanged. Keep this branch's alive
  change (faithful, measured no-op, removes a named deviation) and amendment 1. Then re-run the flip.
- (b) merge this branch as is (alive change + amendment 1 behind MASHED_SUBSTATE, default unchanged) and park the
  flip (D-11073 stays open).
- (c) drop this branch: the alive change has no measured effect.

## 6. Branch content (no tracker touched)

- `mashedmod/src/mashed_re/Race/SubStateMachine.cpp` / `.h`: the alive change, the state-5 alive log line.
- `mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp`: the phase-3 alive site, FLIP amendment 1 (SUBSTATE only).
- `verify/savelink_20261023/run_arms.py`: `SUB` stack, `A4_D2_WALL` (default 90 = unchanged).
- `verify/d3subprego_20261029/`: PREREG, this RESULT, SCORE_D2.txt, vsum_b.txt, e_check.txt, tracks_score.txt,
  RUNLOG copies.
- Owed via re-classify if the flip lands: the D-11073 rewrite (BLOCKER 3 named deviation retired by this change).

## 7. Raw (D:/MashedWT/D3-SUBSTATE-PREGO-ALIVE_data, sha256)

| file | sha256 |
|---|---|
| `mashed_re_pre.exe` | d814b26d342333ab096dc2417e2d4f4cbcc7fd06d5cedf14c307547075990ba1 |
| `mashed_re_post.exe` | 677bfe7919b70b2b1550e8e22dc46ae66274efde9abf1d0425edd03b36c2aaf4 |
| `pre/D2_SUB_1.motion_diag.log` | 10b584f01812926d7bfe36566bd300194905b165dc538d1b658ea83429fcd111 |
| `pre/D2_SUB_1.log` | 9ac3a4cccd73859fbb22b6d7d813ac6e548c57808ecfa14a66d701f799a0d20d |
| `post/D2_SUB_1.motion_diag.log` | 23f1dbf48891cece7c90c164f4ff25f5f67a07afbc3544562b7fde321cc12b5c |
| `post/D2_SUB_2.motion_diag.log` (= `_3`) | 56dcfd8db7334bac263b33b02a16b7f5c7c53320f64582815d4828fd7264a45c |
| `post/D2_SUB_1.log` | 88efd4ec68522557dc9c83ab8275396fb0193dca1b7594ecfa1bdb50b8a03007 |
| `post/D2_DEF_1.motion_diag.log` (= `_2`) | e55c5708939a56d43affb0a93298524ffc45ba69324491be8fcd2b62fca0b34b |
| `post/D2_DEF_3.motion_diag.log` | 397d107b639f5a9231a22d048fabc10eb89939c5b1acebb18549dd6b4cc7dfdd |
| `post/H_DEF.step.csv` | 5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415 |
| `post/B_DEF_K0.step.csv` | 3a03670c3991b702af9fd44b1d49dbdb4f91d6be00876723cb1ba7891895b83f |
| `post/B_SUB_K0.step.csv` | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 |
| `post/H_SUB.step.csv` | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| `post/H_SUB.gates.csv` | c33e1c11be346285d3788d3338e21fa7ca071b33f3687ea70ab27ef25bbf7fb4 |
| `post/H_SUB.ptrace.log` | 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 |
| `tracks/` | 13-track out dir (111 MB, not committed). Scored copy committed: tracks_score.txt, sha256 a3c14c418b1cb6ad8e3e97306bca2ebfbf733f98446b51fec1fb81231b936ae0 |

## 8. Process

Every mashed_re PID was spawned by this step's runner and exited on its own, except the D2 arms, which the runner ends
at their wall time by killing its own PID (the D2 recipe). Nothing else was killed. No .asi deploy. `original\`
untouched. The lock was held by D5-PREGO-CUBE-FLIP at times, and this step queued behind it.
