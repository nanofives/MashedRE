# RESULT: D3-SUBSTATE-FLIP (L-45 / D-11073, flip MASHED_SUBSTATE + MASHED_P3_RESET_PAIR default ON)

Date 2026-10-28, run 20. Branch `orch/D3-SUBSTATE-FLIP` from `orch/integration` 327fd83c6. PREREG amendment 1
`PREREG_D3-SUBSTATE-FLIP.md` committed alone at 1156b6179, before any arm of its change. Standalone only, no MASHED.exe
launched, no Ghidra MCP, no .ps1 edited.

## 0. Verdict first

> **DECISION. Not flipped. D2 leaves the band under the flip: 0.2016 / 0.2591 OUT / 1791.85 OUT on 6 of 6 arms (90 s and
> 102 s), against 0.1942 / 0.2492 / 1896.74 for the opt-out control on the same exe and machine.** The cause is a pre-GO
> gap in the sub-state port for a solo race (section 5), not the flip mechanism.
>
> - G-CTRL and B are explained and deterministic once amendment 1 is in. With the flip, G-CTRL would be
>   `8284be46 / c33e1c11 / 31fe043e` (5 of 5 runs, including a first launch on a fresh root). B would be `1d5bf4f9` on K0, K1
>   and K26 (2 of 2). Both are the opt-out's race shifted by the 696-frame longer pre-race: the dumps are identical except
>   the absolute `DAT_007f0ff4` value and the frame-0 record fields (section 4).
> - Amendment 1 (found here): under MASHED_SUBSTATE the AI clock `DAT_007f0ff4` reached GO at 0, so FUN_00416250 took the
>   decay arm on the first racing calls, and (e) launch moved 0.5 away from the original. With the fix, B's launch rows equal
>   the original o_reg_r1 exactly. The fix is only reachable with MASHED_SUBSTATE set, so the default is unchanged.
> - The branch carries amendment 1 (knob-gated), a default-OFF diagnostic, runner stacks and this evidence. The default seed
>   is NOT in it: `Core/FaithDefaults.cpp` is unchanged.

## 1. What ran

| exe | content | sha8 |
|---|---|---|
| flip 1 | SUBSTATE + P3_RESET_PAIR seeded in `Core/FaithDefaults.cpp` (opt-outs `MASHED_NO_SUBSTATE`, `MASHED_NO_P3_RESET_PAIR`) | b55f9419 |
| flip 2 | flip 1 + amendment 1 + `MASHED_SUBFLIP_DIAG` | c893a292 |
| branch | amendment 1 + diag, no seed (what is committed) | d11b2297 |

Runner `verify/savelink_20261023/run_arms.py` (the merge-train runner) with new stacks `NOSUB` (both opt-outs), `NOP3R`,
`NCC` (`MASHED_NO_AI_COUNTDOWN_CLOCK=1`), `L697` (`MASHED_DET_FRAMES=1897`), `SFD` (diag), and `A4_D2_WALL` (D2 wall
seconds, default 90). Private roots `D:/MashedWT/D3-SUBSTATE-FLIP_root` and `_root2`. Every run under
`mashed_lock.py run --label D3-SUBSTATE-FLIP` (D2 under `--solo`) inside `test-slot`.

## 2. Wall-clock / determinism check (asked: run each gate twice)

- G-CTRL, B and the 13-track recipe run under `MASHED_DETERMINISTIC=1` with a frame budget (`MASHED_DET_FRAMES`). The
  port's phase-3 camera ticks are fed from `in.dt` (`cam_ticks_ += dt * 3e6`, TrackRenderer.cpp), not from QPC, so phase 3
  lasts a fixed 763 frames here (log `[leg2] phase 3 released after 763 frames`, every DET run). The QPC dependence in
  memory phase3-length-is-the-qpc-clock is the ORIGINAL's.
- D2 is not deterministic (90 s wall). There phase 3 took 776 frames (D2_DEF_4.log). The D2 result is still identical
  over 6 runs, see section 5.
- flip 2: G-CTRL 5 of 5 identical (H_SFD x2 with the diag, H_DEF x2, H_SFD first launch on fresh `_root2`). B 2 of 2.
- flip 1: B 2 of 2 identical, G-CTRL 1 of 2. Run 1 (first launch on a freshly copied root) dumped 12078 race frames,
  run 2 13521, out of the same 14400. Every row of run 1's three files is identical to run 2's prefix, so the content is
  deterministic. Only the number of race frames inside the budget differs. The diag on flip 2 shows no stretch without an
  AI tick after GO and no sub-state stall (H_SFD.log: ba8c 0 -> 3 at call 2, 4 at 765, 5 at 766, 6 at 878, then an AI tick on
  every call through 14000). **[UNCERTAIN]** where run 1's 1443 frames went: not in a racing frame on flip 2, not reproduced
  in 5 later runs including a fresh-root first launch. Evidence missing: a run that reproduces it with the diag on.

## 3. Flip 1 (b55f9419): what moved and why

| gate | flip 1 | ref |
|---|---|---|
| G-CTRL | 72433c0f / 1e5a1c92 / d1cb07bd (run 2: 97eeec7a / b4589112 / 9f18aea6) | 5943c5f1 / 364350ae / 52052890 |
| B K0 / K1 / K26 | 68d6a0b8 / 68d6a0b8 / 68d6a0b8 | 3a03670c / 9fcdbd3a / 53778664 |
| (b) | 2/3 per arm (car 1 A FAIL: c0_distinct 10, steer_distinct 32, abs_steer_median 0) | 3/3 |
| (e) launch | 1425.7 / 2052.0 / 2054.5 | 1425.7 / 2052.5 / 2055.0 (= o_reg_r1, r13, r19) |
| NOSUB opt-out | G-CTRL and B = the refs exactly | |

Launch rows, car 2, first 5 racing ticks (c0): original o_reg_r1 0 / 11 / 23 / 35 / 46, NOSUB the same, flip 1
0 / 0 / 0 / 0 / 48. `DAT_007f0ff4` at the first racing row: original 42800 (r1) / 43100 (r13), NOSUB 9050, flip 1 0.
Cause: PREREG section 1 (the sub-state block returns before the stand-in's countdown add, and the late add runs on race
frames only). This is the U-9744 symptom MASHED_AI_COUNTDOWN_CLOCK fixed for the stand-in.

## 4. Amendment 1 (c893a292): predictions and the shifted gates

| id | result | verdict |
|---|---|---|
| P1 | `B_DEF_K0` cars 1..3, first 5 racing ticks: c0, rec_b0c, own_x, own_z, rec_9e4 equal o_reg_r1 (car 2: 0/11/23/35/46, 861.1806 at tick 3). (e) 1425.7 / 2052.5 / 2055.0, ft_median_m0 2551.6 / 2052.5 / 2278.1, 3/3 PASS | PASS |
| P2 | clk_0ff4 at the first racing row 43800 | PASS |
| P3 | `H_NOSUB` 5943c5f1 / 364350ae / 52052890, `B_NOSUB` 3a03670c / 9fcdbd3a / 53778664 | PASS |
| P4 | `B_NCC_K0` 68d6a0b8 (= flip 1) | PASS |
| P5 | G-CTRL 5/5, B 2/2 identical | PASS |

Reported:
- (b) with the flip: 3/3 on `B_DEF_K0` (1200 frames) and on `B_L697_K0 / K1` (1897 frames).
- Window shift, measured: the flip's pre-race is 877 frames (763 + 1 + 112 + 1) against the stand-in's 181. So the
  14400-frame G-CTRL budget holds 13521 race frames instead of 14218, and B's 1200 holds 321 dump frames instead of 1018.
  `H_DEF` (flip 2) vs `H_NOSUB`: ptrace 27042 of 27042 lines identical, step.csv all rows identical except clk_0ff4
  (offset) and frames 0 (3 rows), gates.csv all rows identical except frame 0 (4 rows: rec_x/z_rec, ptr_x/z, racepct_ec,
  prog_96e8 read 0). `B_L697_K0` (flip, +697 frames) vs `B_NOSUB_K0`: 1458 = 1458 rows, the same two exceptions.
  **[UNCERTAIN]** whether the original has those record fields set on its first racing row (o_reg captures lack the columns).
- B loses its ring phases under the flip: K0, K1 and K26 give one step file at 1200 and at 1897 frames, although
  MASHED_RING_SKIP still changes the countdown roll (`[substate] FUN_00414180 ... cd=3,1,4,2` on K0/K26, `cd=1,4,2,1` on K1).
  All three equal the NOSUB K0 trajectory. **[UNCERTAIN]** why the K1 / K26 trajectories collapse: not traced. If the flip
  lands, B stops sampling three phases. That is a gate-power change L0 should know about.

Expected new refs if the flip lands with amendment 1 and nothing else: G-CTRL `8284be46 / c33e1c11 / 31fe043e`, B
`1d5bf4f9 / 1d5bf4f9 / 1d5bf4f9` (1200-frame recipe unchanged).

## 5. D2 (score_d2_v2.py, band verify/d2rebase_20261019/band.json) — leaves the band

`SCORE_D2.txt`:

| arms | slip1500 | slip2000 | drvmed | n | motion_diag lines |
|---|---|---|---|---|---|
| D2_DEF_1..3 (flip 2, 90 s) | 0.2016 in | 0.2591 OUT | 1791.85 OUT | 125 / 175 / 448 | 920 / 919 / 917 |
| D2_DEF_4..6 (flip 2, 102 s) | 0.2016 in | 0.2591 OUT | 1791.85 OUT | 125 / 175 / 448 | 921 / 917 / 919 |
| D2_NOSUB_1 (opt-out, 90 s) | 0.1942 in | 0.2492 in | 1896.74 in | 334 / 529 / 1208 | 1633 |

- Every arm is under the 1600-line rule. The 102 s arms give the same ~920 lines (two identical sha8 to 90 s arms), so
  the shortfall is not the wall clock. The drive itself is shorter: the flip's log ends with the car at sp=11.91, steer
  +1.000, ftot constant.
- The launch differs from the first line. The opt-out logs `settle slot=0 order=1 settleOnly=1` and
  `launchrev release car=0 bf4 3000 -> 3000 bf8 0 -> 2`. The flip logs `settleOnly=0` and no release line.
- Static cause (not yet measured): the sub-state machine's `CarAlive(v)` (`FUN_0046c7b0` in SubStateMachine.cpp) is the host's
  `car_alive` = `aib_alive(v)` (TrackRenderer.cpp, `h.car_alive`). That reads `g_aib.alive`, which `AiBridgeSnapshot` fills
  only inside an AI tick. A MEASURE_SOLO race has no AI car, so `h.ai_tick` returns early and `subClock` is false: no AI tick
  runs before GO. So state 5 skips the player's hold (`0x0041043a`), its rev charge (`0x00410441`) and its release
  (`0x0041049b`), which the stand-in ran (LaunchRev_PreRaceTick / LaunchRev_Release). Same trap as memory
  ai-host-car-alive-is-racing-frames-only. **[UNCERTAIN]** until a gate log shows alive = 0 on the state-5 frames of a D2 arm.

## 6. Not run

13-track table: not run. The flip is not committed, and the run holds the solo window for about an hour. It should run
with the flip's next attempt. Expected to move, because the recipe frames are `max shot + 350` (run13.py) and the
pre-race grows by 696 frames.

## 7. Options (DECISION)

- **(a) recommended**: a step D3-SUBSTATE-PREGO-ALIVE. In SubStateMachine.cpp, make the pre-GO alive tests read the record's
  alive word (`FUN_0046c7b0` / `VehicleSlotGetter`) instead of `aib_alive`, PREREG with D2 x3 back in band and B / G-CTRL
  unchanged from section 4. Then re-run this flip (seed + amendment 1) with the 13-track table and the new refs.
- (b) flip now with amendment 1 and accept D2 out of band. Not recommended: D2 moves away from its band on 6 of 6 arms
  with a known port gap.
- (c) keep MASHED_SUBSTATE default OFF (D-11073 stays open).

Gate-recipe notes for whichever flip lands: B's three ring phases collapse to one trajectory (section 4). D2's line
count under the flip depends on the fix in (a), so re-check `d2_min_lines` then.

## 8. Branch content (no tracker touched)

- `mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp`: amendment 1 (one line + comment, only with MASHED_SUBSTATE),
  `MASHED_SUBFLIP_DIAG` (default OFF, log only).
- `verify/savelink_20261023/run_arms.py`: stacks NOSUB / NOP3R / NCC / L697 / SFD, `A4_D2_WALL` (default 90 = unchanged).
- `verify/d3subflip_20261028/`: PREREG, this RESULT, SCORE_D2.txt, LAUNCH_ROWS.txt, RUNLOG copies.
- Owed when a flip lands, via re-classify: the D-11073 rewrite (PH3's section 5 item plus amendment 1 and the alive gap).

## 9. Raw (D:/MashedWT/D3-SUBSTATE-FLIP_data, sha256)

| file | sha256 |
|---|---|
| `mashed_re_flip1_b55f9419.exe` | b55f94190861c3b004c2e8bd8eae5e504d0763c7c1a10c6ae9be0c2920dbf445 |
| `arms/H_DEF.step.csv` (flip 1 run 1) | 72433c0fb6455dc958ae0a6e837669e0920a5f865b31238816a8a8b4a3c686d0 |
| `arms/H_DEF-2.step.csv` (flip 1 run 2) | 97eeec7a336ad496bd2149a5a9c59c272aad64d2cdad694eb69270b0ce735891 |
| `arms/B_DEF_K0.step.csv` (flip 1) | 68d6a0b8fd542090cf4ed1a2f07ee4104b1fc4dd4b204e37a3e58d5f29e4f996 |
| `arms/B_NOSUB_K0.step.csv` | 3a03670c3991b702af9fd44b1d49dbdb4f91d6be00876723cb1ba7891895b83f |
| `arms2/B_DEF_K0.step.csv` (flip 2) | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 |
| `arms2/B_L697_K0.step.csv` | 5f133b3799d6ad587a2fb2044165e83f335e14a5b3f21ae45cdf3390f0c955c6 |
| `arms2/B_NCC_K0.step.csv` | 68d6a0b8fd542090cf4ed1a2f07ee4104b1fc4dd4b204e37a3e58d5f29e4f996 |
| `arms2/H_DEF.step.csv` (flip 2) | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| `arms2/H_DEF.gates.csv` | c33e1c11be346285d3788d3338e21fa7ca071b33f3687ea70ab27ef25bbf7fb4 |
| `arms2/H_DEF.ptrace.log` | 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 |
| `arms2/H_SFD.log` (diag) | 077058af64950f0c5f3332eec7f536fc7bdbf81125ac33505ef1e05c3c7bb800 |
| `d2/D2_DEF_1.motion_diag.log` | 56dcfd8db7334bac263b33b02a16b7f5c7c53320f64582815d4828fd7264a45c |
| `d2/D2_DEF_4.motion_diag.log` (102 s) | 10b584f01812926d7bfe36566bd300194905b165dc538d1b658ea83429fcd111 |
| `d2/D2_NOSUB_1.motion_diag.log` | 411c026d596825808f0f68399b280d5b97a5a2bb9969662b91def4eb8e44c36f |
| `d2/D2_DEF_1.log` / `d2/D2_NOSUB_1.log` | 7a0aadc5... / 228b2bf9... (full in LAUNCH_ROWS.txt footer) |
| flip 2 exe `mashed_re.exe` (c893a292, not kept) | c893a292f15526797d887da781787f617f44bb0f7b3efba62f4e58a4f1b7127d |

## 10. Process

Every mashed_re PID was spawned by this step's runner and exited on its own, except the D2 arms. The runner ends those at
their wall time by killing its own PID, which is the D2 recipe. A tool time limit stopped one waiting wrapper pipeline (the
fresh-root H_SFD) while it was still queued. Its lock holder and game PID 16020 kept running and exited on their own
(RUNLOG line present). Nothing else was killed. No .asi deploy. `original\` untouched. The merge train and D5-ICONCUBE-GLOW
held the lock at times, and this step queued behind them.

Default gates on the committed branch exe d11b2297 (arms4, after the queue cleared): G-CTRL 5943c5f1 / 364350ae / 52052890, B 3a03670c / 9fcdbd3a / 53778664 = merge_train.json refs.
