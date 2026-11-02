# RESULT D4-DC-77b0-ST3-PRETICK (run 30, 2026-11-01)

**Verdict: GREEN.** With `MASHED_ST3_PRETICK=1` the exe runs FUN_00418860 (`AiBridgeSnapshot` + `Ai_Standalone_Tick`) once
per case-3 tick: 762 entries reading 0x0063ba8c = 3 and 1 reading 4 for a 763-frame phase 3. That is the original's shape
(o_r0_t0: 677 + 1 for a 678-tick phase 3). Every entry reads the original's cells (1, 2, 2, 2) with 0x00897ffc = 0. Unset,
the knob leaves G-CTRL and B on the `merge_train.json` refs. Set, G-CTRL and B **also** stay on the refs: the 763 extra AI
ticks before GO change nothing either recipe hashes. Flip filed as a decision row.

PREREG: `PREREG_D4-DC-77b0-ST3-PRETICK.md` (commit `0889efc83`, alone, before the knob). Knob + runners: `afe21a567`.
Base `orch/integration` `96640c25a`. Exe `mashed_re.exe` sha8 `5cb065de` (this worktree, `MASHED_NO_DEPLOY=1`).

## Method

- Static (PREREG section 1): FUN_004111c0 case 3 is `call 0x004102f0` (0x00411288) then `call 0x0040fc00` (0x0041128d).
  FUN_0040fc00 stores 0x007f100c (0x0040fc57) and 0x007f1008 (0x0040fc63), then calls FUN_00425a40 (0x0040fc6c) while
  DAT_005f29c0 != 0. FUN_00425a40 calls FUN_00418860 at 0x00425a73.
- Knob: at the end of the exe's case-3 arm (`TrackRenderer::UpdateCar`, after the release test), `Ai_AdvanceClock(units)`,
  `AiBridgeSnapshot()`, `Ai_Standalone_Tick()`. Only the `ba8c == 3` arm is edited.
- **Overlap check** (asked by the step): `Ai_CountdownAlivePoke` has no caller since `20efede42` (D5-DEAD-COUNTDOWN-STANDIN),
  and `MASHED_COUNTDOWN_SPAWN` has no reader left in `TrackRenderer.cpp`. No overlap. The knob reaches the same alive-poke
  through `Ai_Standalone_Tick`'s own FUN_00418860 block. It does not seed the spawn pose.
- Arms: `run_all.sh` under `mashed_lock.py run --label D4-DC-77b0-ST3-PRETICK` and `test-slot`, private root
  `D:\MashedWT\D4-DC-77b0-ST3-PRETICK_root` (`MASHED.exe.unpatched` = anchor `BDCAE093...`). Exe probe `exe_vsprobe.py`
  (copy of MATCHEND-SUBSTATE's): x_r0_t0 recipe (Training, rule 0, game mode 10, deterministic, 14400 frames). Gates
  `run_arms.py` (stack `ST3`). No original-side boot (the original's numbers are RUNTIME-PARKED's).

## Results

Exe probe, B (`AiPreTickRubberBand`) entries per tuple (`SUMMARY_x_off.csv`, `SUMMARY_x_st3.csv`; T rows have the same
counts, its first entry reads the cells before its own alive-poke):

| ba8c | re7 | cells v0..v3 | ffc | x_off | x_st3 | original o_r0_t0 |
|---|---|---|---|---|---|---|
| 3 | 0 | 1,2,2,2 | 0 | 0 | **762** | 677 |
| 4 | 0 | 1,2,2,2 | 0 | 0 | **1** | 1 |
| 5 | 0 | 1,2,2,2 | 0 | 112 | 112 | 224 (STATE5-LEG2 class) |
| 6 | 0 | 1,2,2,2 | 0 | 220 | 220 | 196 |
| 6 | 1 | 1,2,2,2 | 0 | 13218 | 13124 | (state 7: 13398) |

Phase 3 lasted N = 763 frames in both probes (`[leg2] phase 3 released after 763 frames`). x_st3's log:
`[st3pretick] ticks state3=762 state4=1`. First T entry in x_st3: cells (1, 0, 0, 0), call 2, ba8c = 3. Both runs: one
`R6 ROUND END winner=car3 scores=[4,5,7,8]`, exit by the frame count, no crash.

Gates (`arms/RUNLOG.txt`):

| arm | step | gates | ptrace |
|---|---|---|---|
| H_DEF | 8284be46 | ba407a16 | 31fe043e |
| H_ST3 | 8284be46 | ba407a16 | 31fe043e |
| B_DEF_K0 / K1 / K26 | 1d5bf4f9 x3 | | |
| B_ST3_K0 / K1 / K26 | 1d5bf4f9 x3 | | |

The knob fired in every ST3 arm: H_ST3 and B_ST3_* logs each carry `[st3pretick] ticks state3=762 state4=1`.

## Predictions

- **P0 holds.** Knob unset: G-CTRL `8284be46 / ba407a16 / 31fe043e`, B `1d5bf4f9` x3 = `merge_train.json`.
- **P1 holds.** x_off: first T entry is call 2 with ba8c = 5. No entry reads 3 or 4.
- **P2 holds.** x_st3: B(3) = 762 = N - 1, B(4) = 1, T the same, `[st3pretick] state3=762 state4=1`.
- **P3 holds.** Every x_st3 entry reading 3 or 4 has (1, 2, 2, 2) and ffc = 0. The first T reads (1, 0, 0, 0) and its poke
  writes the 2s before the first B (call 3).
- **P4 holds for states 5 and 6 (flag down): 112 and 220 in both.** The last row (6 with `s_roundEnd7` up, which runs until
  the frame-count quit) is 94 shorter in x_st3. `MASHED_DET_FRAMES` counts rendered frames from boot (`exe_main.cpp:452`),
  so that row's length is whatever frame budget remains after the pre-race frames. Two facts settle it as a run-length difference, not a knob effect: (1) the rows that end on a game event (5: 112, 6 flag down: 220, the round end) are equal, and (2) without Frida, H_DEF and H_ST3 write a byte-identical player trace `31fe043e` that runs f = 1..13521 up to the frame-count quit, so with the knob set the exe reaches the quit on the same race frame. The 94 come from the probe runs' pre-race frame counts (boot, intro movies, loading: the two probe logs already differ in intro movie frames, 100 vs 102). A repeat probe pair was queued and cancelled unrun, because the machine lock stayed with the merge train for over 30 minutes. The PREREG wording ("B counts per 0x0063ba8c value equal") did not exclude this row. That is a PREREG gap, recorded here.
- **P5 holds.** No crash or hang in x_st3, H_ST3, B_ST3_*.
- Reported: H_ST3 / B_ST3_* hash equal to the default. The extra AI ticks before GO leave nothing that the step dump, the
  gate dump or the player trace read from GO on.

## Not ported (named, unchanged from the PREREG)

The rest of FUN_00425a40 on case-3 ticks (FUN_00423b00, FUN_00424eb0, FUN_00470c70, FUN_00422ba0, FUN_004252c0). The case-3
`[0x007f0fc8] = 0` store (0x0041127e). The case-4 tick's call (reads 5), left to the STATE5-LEG2 / ST5-HOLD steps.
`Ai_Standalone_Tick` has no form of FUN_00418860's `FUN_00418560(0, type)` arm (round type 3/4/5 with DAT_007f0fd0 == 7),
on race frames as on countdown frames. Not reached on the warp-launched match (0x0067e9fc = 10).

## Merge note

ST5-HOLD (`orch/D4-DC-77b0-ST5-HOLD`) edits the `ba8c == 5` arm and the FUN_0040fc00 partials below it. This branch edits
only the `ba8c == 3` arm, so the two do not share a line. Once both are in, the case-3 ticks could run ST5-HOLD's full
`St5Leg2` leg instead of FUN_00418860 alone (that would cover the "not ported" FUN_00425a40 tail above).

## Files

Committed: `PREREG_D4-DC-77b0-ST3-PRETICK.md`, this RESULT, `run_all.sh`, `run_arms.py`, `exe_vsprobe.py`, `summarize.py`,
`SUMMARY_` / `WATCH_` / `SEQ_` for each probe.

Raw (not committed), `D:/MashedWT/D4-DC-77b0-ST3-PRETICK_data/`:

| file | SHA-256 |
|---|---|
| x_off.log | 5fb5c1bd229d9496ec1bdf41e2be7509defb9343ce28cfb7d0ef8d0d1cdf0d57 |
| x_off.mashed_re.log | a16d074f73fdb5007076ecab054ab8089dd73a94868e00561bd32758fd52b00b |
| x_off.vs.csv | 94ecb274a32c8b5ce7c25e0cb232e35b01a67e8624791001cebe724761d60d5f |
| x_off.vsw.csv | 9eac0167d06b228ba50fe664803a114a37f2987dcc8c4be40d2ce5276a7a57fe |
| x_st3.log | f096c1c07451309dadceba38dc0c597c228e78d003c1671ddd14903a305d7897 |
| x_st3.mashed_re.log | 6a10c93571d1cec031dc810095947dcd6a6d76c8fc57ca0c3b7a4c84fb3ecef6 |
| x_st3.vs.csv | e2231a7ab955d25b19a5231d6acd19f4465dba3758ae34cd85de36ec397c92fa |
| x_st3.vsw.csv | af2c71201c2801144fbcf8a159cc78732b732a1f10c0585a4bea3b8d695a50ec |
| arms/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| arms/H_DEF.gates.csv | ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb |
| arms/H_DEF.ptrace.log | 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 |
| arms/H_ST3.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| arms/H_ST3.gates.csv | ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb |
| arms/H_ST3.ptrace.log | 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 |
| arms/B_DEF_K0.step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 |
| arms/B_DEF_K1.step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 |
| arms/B_DEF_K26.step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 |
| arms/B_ST3_K0.step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 |
| arms/B_ST3_K1.step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 |
| arms/B_ST3_K26.step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 |
| arms/RUNLOG.txt | 1ad03787fec4759fbfb59e062027649459d9e930879f30e35555e1019e09b053 |
| arms.log | 277b4472954e25664188c4932bb1635fb6ba6e213320387804334d754e25284a |
