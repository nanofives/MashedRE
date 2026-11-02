# RESULT MATCH-PORTSTATE-GAMEFLOW (2026-11-02)

Verdict: **GREEN for the state half, and the machine body is not swapped.** With `MASHED_PORTSTATE_GAMEFLOW` (default
OFF, exe only), the exe keeps the game-mode machine's words at the original addresses. That covers the phase word
`0x00771968`, the game mode `0x0067e9fc` (written before the course load), and the course row in `0x0067f17c` /
`0x0067ea94` / `0x0063ba7c` / `0x0063ba78`. The exe arm of 0x0046b540 now runs the original's selector chain into
0x0040ce80.

All PREREG predictions hold. One knob-ON arm in run g1 (H_PSGO) moved and did not reproduce on the same exe (see
P4). No gate hash moves with the knob OFF or ON, so no decided-flip decision is filed.

This step does **not** run 0x004929d0 in place of `GameFlow` / `RaceSession`. The machine is unmatched (S15), 14
of its 30 callees have no exe body, and its case-3 round end would move round-end and score behaviour. The handoff
below lists what that swap needs.

PREREG: `verify/portstategameflow_20261102/PREREG_MATCH-PORTSTATE-GAMEFLOW.md`, committed alone at `c255a4063`
before any arm ran. Basis: L-78 (c) (Mariano 2026-11-01), port-state migration 3/3. Base branch:
`orch/MATCH-PORTSTATE-RECORDS` @ d5a57c978 (2/3), which sits on `orch/MATCH-PORTSTATE-COURSE` @ 622335487 (1/3).
Neither is on `orch/integration` yet.

## Change (`b03eb46ac`)

- `Race/GameModeState.{h,cpp}` (new, exe only).
  - `SetPhase` writes `0x00771968`.
  - `PreLoad` writes `0x0067e9fc`.
  - `OnCourseLoad` publishes the course row k and logs the `[portstate-gameflow]` witness.
  - `HandlingType` is the original's 0x0046b56c..0x0046b585 chain. It returns false when the course table is not
    seeded.
  - The original's writers of each word were read with `decomp_pc.py` on a read-only pool clone (the table in the
    PREREG).
- `Track/CourseTableState.{h,cpp}`: `CourseTableState_IndexOf(course_id)` returns the row whose record dword 0 =
  `course_id`, read from the transcribed tables.
- `Race/GameFlow.cpp`: the phase word follows the original's transitions, which are:
  - launch: 2
  - LoadingRace -> InRace: 3
  - pause: 7, resume: 3
  - results: 5
  - exit: 1
- `exe_main.cpp`:
  - The boot phase is 1, written before the dev-race block.
  - Dev-race route (`MASHED_ROUND`): `PreLoad` and phase 2 before `g_track.Load`, then phase 3 after `StartMatch`.
  - `MenuLaunchRace`: `PreLoad(launchMode)` and phase 2 before the load.
- `D3d9Render/TrackRenderer.cpp`: `GameModeState_OnCourseLoad(course_id_)` is called right after
  `CourseTableState_OnCourseLoad`.
- `Vehicle/VehicleInit.cpp`, exe arm: `A3HandlingType()` calls `GameModeState_HandlingType` first and otherwise
  keeps the constant.
- `exe_sources.rsp`: `Race\GameModeState.cpp` and `Util\PromoLoop_round49.cpp` were added. 0x0040ce80
  `PtrTable5f2770Get` now has one body in both targets.
- `run_arms.py`: new stacks `PSG` (COURSE + RECORDS + GAMEFLOW), `PSGO` (GAMEFLOW alone), and `DEFA` / `PSGA`
  (Arctic).

Build: `MASHED_NO_DEPLOY=1`, `=== Build OK ===` for both targets, `[rva-lint] 3 finding(s) ... allowlisted=3 NEW=0`,
`[asm-lint] OK`.

## Predictions

Gate runner: `verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26 H_PSG B_PSG_K0 B_PSG_K1
B_PSG_K26 H_PSGO B_DEFA_K0 B_PSGA_K0`. Settings:

- `A4_ROOT=D:/MashedWT/MATCH-PORTSTATE-GAMEFLOW_root` (a robocopy of `original/`).
- The exe `fd0211e9` was copied out first.
- The run was wrapped in `test-slot` and `mashed_lock.py run --label MATCH-PORTSTATE-GAMEFLOW`. It queued behind
  the merge train's hold both times.

All 12 arm runs exited 0.

| id | prediction | measured | holds |
|---|---|---|---|
| P1 | knob OFF: 8284be46 / ba407a16 / 31fe043e, B 1d5bf4f9 x3 | H_DEF 8284be46 / ba407a16 / 31fe043e, B_DEF_K0/K1/K26 1d5bf4f9 x3 | yes |
| P2 | H_PSG witness, Training | `course=30 k=0 mode=10 row=0 qbrow=0 sel=0 ld=0 v_rec_id=30 v_key=6`, `a3 mode=10 v=0 type=6`; phases `0 -> 1`, `1 -> 2` (before the course line), `2 -> 3`, no later phase line | yes. The first line reads `0 -> 1`: `Boot/FrameDispatch.cpp`'s write of 1 does not run in the exe |
| P3 | H_PSG step 8284be46, ptrace 31fe043e, B x3 1d5bf4f9, gates.csv 67931834 with the abs columns masked equal | step 8284be46, ptrace 31fe043e, gates.csv 67931834, B_PSG_K0/K1/K26 1d5bf4f9 x3. `gates_mask_g1.txt`: knob-ON abs != rec on 0 of 54,084 rows, masked differing cells 0 | yes |
| P4 | H_PSGO (GAMEFLOW alone): `v_rec_id=-1 v_key=-1`, no `a3` line, P1 hashes | witness as predicted on both runs. Run g1: 3cc38c6b / 75d81021 / 8c8ba1f1, 280 s. Rerun g1b, same exe: **8284be46 / ba407a16 / 31fe043e**, 193 s | yes on g1b. g1 is a non-reproducing outlier, see below |
| P5 | Arctic: `course=0 k=3 ... v_rec_id=0 v_key=18`, `a3 mode=10 v=3 type=18`, B_PSGA_K0 != B_DEFA_K0 | `course=0 k=3 mode=10 row=3 qbrow=3 sel=3 ld=3 v_rec_id=0 v_key=18`, `a3 mode=10 v=3 type=18`. B_DEFA_K0 b030be28 vs B_PSGA_K0 f28b5695 | yes |

**The g1 H_PSGO outlier.**

- The step file equals H_DEF for its first 4,969 lines. At frame 4528 the remaining car 3 is joined by respawned
  cars 1 and 2 (substate 7 -> 6, `ai_mode` 3). H_DEF instead continues with car 3 alone.
- That is a round-level difference. The arm ran 280 s, against 145 s for H_PSG and 193 s for the rerun.
- The rerun on the same exe (`fd0211e9`), the same env and the same root reproduced the refs exactly.
- The knob-OFF and knob-ON witness lines are identical on both runs, and H_PSG performs the same writes with no
  move.
- So the move is not attributed to the knob. No cause was found [UNCERTAIN: which wall-clock or round-level input
  diverged at frame 4528. The evidence is the two step files listed below, and no instrumented repeat was run].
- `merge_train.json` gives G-CTRL `retries: 1` for this class of run.

P5 is the behaviour change the knob makes on any track other than Training. Arctic's row 3 handling key is 18
(record 0x005f2584, dword +4 = 0x12): `kHandlingTable` gives 100 / 42000, against the constant 6's 105 / 40000.
This is not a gate: the gates race Training, where k = 0 and the key is 6. No original-side Arctic arm was run, so
P5 shows only that the exe reads the row the original's selector reads from the same words.

## Re-check of the blocked T-rows (knob ON, PSG)

| row | rva | before | knob ON now | remaining blocker | outcome |
|---|---|---|---|---|---|
| T13 | 0x0040ce80 | the selector input read `0x0067f17c` while the original reads `0x0067ea94` in mode 10; only Training's row was right; no exe caller | **cleared.** The exe arm of 0x0046b540 calls `PtrTable5f2770Get` (0x0040ce80) with the original's index: Training `v=0 type=6`, Arctic `v=3 type=18` | default OFF (a flip). The body is the hand port in `Util/PromoLoop_round49.cpp` (C3); the matched `re/match/s02/0040ce80.c` is not yet the body. It needs COURSE seeding too (`PSGO` keeps the constant) | **exe caller added, knob ON only**; swap-ready behind a flip of COURSE + GAMEFLOW |
| T13 | 0x00414490 | held word `0x0088fc88` reads 0 on all four cars | unchanged. The exe holds the held item as a type code in `PowerupSystem pw_`, while the original holds an entry pointer at +0xa8 (`RESULT_MATCH-PORTSTATE-RECORDS.md`) | powerup state, not part of the game-mode machine | left; needs a powerup port-state row |
| T10 | 0x0040d440 | `0x0063ba7c` not written; calls 0x0040d270 | `0x0063ba7c` = `0x0063ba78` = k | 0x0040d270 (Course::Finish) has no exe body and is unmatched; the exe course load is port code | left |
| T10 | 0x0040b6e0 | gate 0x0042f6a0 read 0 at load | `0x0067e9fc` is 10 from before the load | its counter `0x0063b8ec` has no exe writer; only caller is case 3 of 0x004929d0 | left |
| T10 | 0x0040b430, 0x0040b700, 0x0040e340, 0x0040e450, 0x0040e460, 0x00432080 | round state dead in the exe | phase word live; round state still not migrated | `0x008a94f0[4]`, `0x008a9530..60[4]`, `0x008a94d0` (exe uses `g_participantCount = 4`), `0x0063b908`. The round end is decided by GameFlow / the dev route, not 0x00432080 | left; round-state migration (handoff) |
| T07 | 0x00411750 | arm runs when `0x0042f6a0() == 2`; e9fc was 0 at load | e9fc is the launch mode before the load. The gate scenario is mode 10 on both sides, so neither enters the arm | needs a mode-2 route to exercise | left; input now faithful |
| T04 | 0x00410b30 | `*PTR_PTR_005f2770[DAT_0063ba78]`; `0x0063ba78` had no exe writer (0, right on Training only) | `0x0063ba78` = k on every track | RW device slot +0x20, `DAT_0063ba98` / `0x007f0f30` / `0x007f0f3c` / `0x0063b920`, about 30 render callees (`RESULT_MATCH-TOPDOWN-T04.md`) | left |
| T05 | 0x0041e870 and others | unchanged from COURSE | unchanged | as in `RESULT_MATCH-PORTSTATE-COURSE.md` | left |

## Handoff: 0x004929d0 in place of GameFlow (not done here)

`callee_bodies.py` / `callee_bodies.txt` lists 0x004929d0's 30 depth-1 callees by where a body exists (an
`RH_ScopedInstall` TU, by rsp list):

- **16 have an exe body** (exe+asi): 0x0042c2e0, 0x0042c2d0, 0x0042b940, 0x0042b930, 0x0042b8d0, 0x0042c1d0,
  0x0042c1c0, 0x0042f6a0, 0x0042b910, 0x004331a0, 0x0042f500, 0x0040b6d0, 0x0040b430, 0x00448700, 0x0042b8f0,
  0x0042b8e0.
- **9 are asi-only**: 0x0042c2f0, 0x0040d440, 0x0042c280, 0x0040e350, 0x00432080, 0x0040e450, 0x0040b6e0,
  0x0040b540, 0x0040e460.
- **5 have no body**: 0x004927c0 (500 B; steps `DAT_00771980` against input bytes `DAT_007f1044..` and at 0xc fills
  `DAT_007f0a40` x 0x9c with 2, `decomp1.txt`), 0x0042c220 (the pause arm, `Race/GameFlow.h:36-38`), 0x005c9d00,
  0x0040b700 (matched source `re/match/s02/0040b700.c` exists, no TU), 0x0040d270 (unmatched).

A port of the machine that runs in place of `GameFlow` needs four things:

1. Bodies in the exe for the 14 missing callees. 0x0040d270 is the deep one, since T05 hides 31 callees under it.
2. The round state at the original addresses (`0x008a94d0`, `0x008a94f0`, `0x008a9530..60`, `0x0063b8ec`,
   `0x0063b908`). Without them, case 3's end check (0x00432080) and score updates run on dead data.
3. The exe's per-frame tick (`exe_main` RenderFrame / `GameFlow_Update`) to dispatch on the phase word.
4. A PREREG per behaviour change, because the round end would move from GameFlow / `TrackRenderer` to 0x00432080.

The phase word is already live, so a witness can compare the exe's transitions with the original's. A
`mashed_lock` run of the original with a read of `0x00771968` per frame would do it.

## Scope limits

- The gates were measured on Training. The only non-Training arm is P5 (Arctic, B arm, 1200 frames).
- The menu route's phase writes (7 / 5 / 1) were not exercised by any arm. The gate route never enters `GameFlow`.
- The default build's code layout changed: new calls, one new TU, and `PromoLoop_round49.cpp` added to the exe.
  The train's `-Full` pass covers the layout-sensitive tracks.
- This branch carries COURSE and RECORDS. It touches `Vehicle/` (in `full_set_paths`), `Race/`, `Track/`,
  `D3d9Render/` and `exe_main.cpp`.

## Re-classify

None. 0x0040ce80 gains an exe caller that runs only with the knob ON, and no row changes level.

## Decision

None filed. No gate hash moved with the knob ON (H_PSG, B_PSG x3, and H_PSGO on the rerun). For the lead: flipping
COURSE + GAMEFLOW to default ON would make 0x0040ce80 a sound swap and would give non-Training tracks the
original's handling row (P5). That is a behaviour change off the gate track and would need its own decision row,
with an original-side non-Training witness.

## Raw data (D:/MashedWT/MATCH-PORTSTATE-GAMEFLOW_data, not committed)

```
fd0211e93b286efb3b3a034d2d21eeaf06b69fde3c77d469c9e4421b42b68789  exe_g1/mashed_re.exe (b03eb46ac; exe_g1b is the same file)
4c25c30651f82bb352bc43e700ad1c28b9c37c0e0731c7739e37e6433e0d542f  build1.log
e166fbef636b7ba5e01a8052882655230f17965d6128a706d61cb240d935e8e7  build2.log
7feaa6a257f838e69a0a54140d0d4b4392ac49f64761bf5b9081f81e112b486a  gates_g1.log
84f28f6ffcb4589c588536b58e90811c20997442d2c65c4941b3836a19181a66  gates_g1b.log
8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198  arms_g1/H_DEF.step.csv (= H_PSG.step.csv = arms_g1b/H_PSGO.step.csv)
ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb  arms_g1/H_DEF.gates.csv (= arms_g1b/H_PSGO.gates.csv)
6793183492e0c0822570ad71254f9fdf27568b77c81a25c56347f1616863d58c  arms_g1/H_PSG.gates.csv
31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3  arms_g1/H_DEF.ptrace.log (= H_PSG.ptrace.log = arms_g1b/H_PSGO.ptrace.log)
1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457  arms_g1/B_{DEF,PSG}_K{0,1,26}.step.csv (all six)
3cc38c6b0e0c0df627f336e55bcb45d997fca623c64c97329485b8feaa2a1b69  arms_g1/H_PSGO.step.csv (the outlier)
75d810216f6a7a3285823e1ce2fc1e70c86fb902d9e703c73d8c7f1bea899567  arms_g1/H_PSGO.gates.csv (the outlier)
8c8ba1f164ce13b7a41a72aaee8ce1a31ee559b03bb6f829e1b4b0a36f10a105  arms_g1/H_PSGO.ptrace.log (the outlier)
b030be28efc3188b6b892787bcbfe8e7b3b3810423fd8b8d68ff348b778ab859  arms_g1/B_DEFA_K0.step.csv
f28b5695b02723542cc9bbd89c05efc0f2d79b2a6de4b5677f322b9d6c2afb56  arms_g1/B_PSGA_K0.step.csv
96162cea5153d3b58b02bd8ecf1659d7ed0343474719d86ad1d9be70452b2a08  arms_g1/H_PSGO.log
f8b6d42d836b4373d8f8a7e9fbc1f2b63158c64a36efa09c17476b57c38ca169  arms_g1b/H_PSGO.log
5ce67d03f41352e02a953590fa587cf45b0f33513271a645350b4c677bf78d84  arms_g1/H_PSG.log
d780adae3cd5425d7b9836d20a26425df580a3b959cc98d3d6ccb4e3bebfbbc7  arms_g1/B_PSGA_K0.log
bcbb73a626503611a2176041e89065f1f237157d79f4b4d5fc904314a4b45d7f  arms_g1/B_DEFA_K0.log
00958745c19fc86bbda9360be1a5797c2663c0eacbe3865405d13a117b45f810  decomp1.txt (0x0040ce80, 0x0043dfd0, 0x00431d00/d70, 0x0040e170, 0x0040d250, 0x00492770, 0x00492d10, 0x00493590, 0x004927c0, 0x0043df00, 0x00430790)
d966c398e448560aeda5ecebb875ea5c5ba077c16c959693f9e95016b0490a4a  decomp2.txt (0x0040d040, 0x0040d110, 0x0042a940, 0x0042a980, 0x0040d020, 0x0040d270, 0x00414490, 0x0046b540)
```

Committed extracts: `witness_g1.txt` (every `[portstate-gameflow]` line of H_PSG, both H_PSGO runs and B_PSGA_K0),
`gates_mask_g1.txt`, `callee_bodies.py` / `callee_bodies.txt`, and `raw_sha256.txt`.
