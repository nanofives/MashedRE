# RESULT D3-CF-K357: the k = 357 jump is two port-side timers, not a round restart. With `MASHED_R7_EXIT`, car 3 matches `o_r7_1` on every k (2026-10-25)

Branch `orch/D3-CF-K357` from `orch/integration` `b5c84688e`, `orch/D3-CF-U9779` @ `45dccb4f5` merged (`181af712f`, the four
u9779 knobs, default OFF). PREREG `814626902` (committed alone), amendment 1 `b1c68f867` (committed alone, before the amended
arms). Knob `7f48cc4aa` + `a9fb085f6`. Final exe `3182ad61` (`MASHED_NO_DEPLOY=1`, `original\mashed_re_dev.asi` not
redeployed). Private root `D:\MashedWT\D3-CF-K357_root`. Evidence `D:\MashedWT\D3-CF-K357_data`, cited files copied to
`verify/k357_20261025/data/` (`arms1/` batch 1 + the yaww diagnostic, `arms2/` the amended batch; `*.ptrace.log`,
`*.motion_diag.log` and the H step / gates CSVs left on D:, their line counts and hashes are in each `RUNLOG.txt`). No default changed.

## 1. Cause

Two port-side timers both fire on k = 357 in u9779's `B_U7PR3_K0`. Neither is a port of an original function:

1. **The round hold** (`exe_main.cpp`, R6 match flow): `t - s_round_end_t > 3.0f` -> `NextRoundOrEnd()` -> `StartRound()`.
   The original has no such timer.
2. **The stall stand-in** (`TrackRenderer.cpp`, opponent loop, "Standalone stand-in for the original's respawn"): an AI car
   whose gate has not changed for 2.5 s is placed two gates ahead. A car frozen in sub-state 7 never changes gate. With (1)
   removed, the stand-in alone still moves car 3 at k = 357, then at 509, 661, 813 and 965 (every 152 frames), to (12.0, -18.0),
   (12.973, -10.0293), (12.0, -1.9988), (16.0, 6.0), (16.0, 13.9999) (`B_U7PXR3_K0` of batch 1, `B_U7PXY_K0` yaw jumps at yaww
   frames 358 / 510 / 662 / 814). u9779 read the two as one StartRound event.

**The original** (MASHED.exe.unpatched, capstone + headless decomp): the only writers of `DAT_0063ba8c` inside the sub-state-7
handler FUN_00410860 (FUN_004111c0 case 7) are 9 (`0x00410a5e`, match over) and 8 (`0x00410a6e`, next round), behind one gate:

| step | site |
|---|---|
| alive count n: active slots (`*(PTR_PTR_005f2770 + 0x34 + 4v) != 0`) with FUN_0046c7b0(v) == 1 | `0x004108c3..0x004108f2` |
| `DAT_005f29b8 = (n != 0 ? DAT_005f29b8 : 0x190) + 2` | `0x004108f4..0x00410910` |
| gate: for any v in 0..3, `[0x007f1a14 + v*0x10] != -1` and byte `0x007f1042 + idx*0x4c` != 0 (byte 0xa of ctrl block `0x007f1038 + idx*0x4c`), or `[0x007f0fd0] != 1` and n == 0 | `0x00410903..0x004109b4` |
| exit only when `DAT_005f29b8 >= 0xc8` and the gate | `0x004109b9` / `0x004109c5` |

`DAT_005f29b8` is zeroed at `0x004104c0` (state-5 tail, sets 6) and has no writer in state 6, so it needs 100 sub-state-7 ticks.
With no input the gate stays false: `o_r7_1` stays in 7 for 502 frames to the end of its capture. The original never relocates
car 3 there.

## 2. Knob `MASHED_R7_EXIT` (default OFF)

Acts only while u9779's `MASHED_ROUND_END7` flag is up (`TrackRenderer::RoundEnd7ExitActive()`).
- (a) `TrackRenderer::RoundEnd7ExitTick()`: the FUN_00410860 counter and gate above, read from the mapped `0x007f1a14` /
  `0x007f1042` / `0x007f0fd0`. It replaces the 3.0 s hold. It ticks once per frame after the round-end frame. The counter is
  zeroed where the flag rises (`0x004104c0`). On exit the port keeps its own `NextRoundOrEnd()`.
- (b) The stall stand-in block is skipped for every AI slot (amendment 1).

Knob OFF: both port paths unchanged.

## 3. Results (exe `3182ad61`, `verify/k357_20261025/run_arms.py`, records against `o_r7_1` with u9779's `reck.py`)

| prediction | arm | outcome |
|---|---|---|
| P0 | `H_DEF` | `bacfc747 / 8dd1c004 / 6f9932b6` = default. **holds** |
| P0 | `B_DEF_K0 / K1 / K26` | `f9deb9b9 / d3601fcc / 9da9a26a` = default. **holds** |
| P0b | `B_X_K0` (knob alone) | `f9deb9b9`. **holds** (inert without `ROUND_END7`) |
| P0c | `B_U7P_K0` | `c37d4c0c` = u9779's `B_U7P_K0`. **holds** |
| P1 | `B_U7PXR3_K0` log | one `R6 ROUND END`, one `RULE-EVAL` (round 1), no `round2_go` in 1200 frames. **holds** |
| P2 | `B_U7PXR3_K0` vs `o_r7_1` | `reck.py`: first difference **None** on (+0x958, +0x960, +0x9b0, +0x9b8) over all 719 shared k (0..718 = the original's last record). +0x958 / +0x960 = 6.073916 / -22.0904903 from k = 220 to 718. **holds** (was k = 357) |
| report | `B_U7PX_K0 / K1 / K26` | `3a03670c / 2516d903 / 53778664` (U7P: `c37d4c0c / 0a8f9806 / 7013671d`) |
| report | `H_U7PX` | `5943c5f1 / b493262b / 09551e6a` (U7P: `ef5f2e10 / 57f7ff83 / b4f4b55a`). Evidence replaced by the run-4 re-run under a clean hold (section 6, `data/arms4/`): the same three hashes and line counts. Log: 1 `R6 ROUND END`, no `MATCH OVER` (`H_DEF`: 4 and 1). `B_U7PX_K1` / `K26`: 1 round end each. With the knob the port never leaves round 1 when no input arrives, as the original does |

Batch 1 (exe `9085dbdd`, knob part (a) only): P0 / P0b / P0c / P1 held, P2 failed at k = 357 (the stand-in, section 1).
Amendment 1 added (b).

Whole-record context, not a prediction: the full 0xd04 record differs from `o_r7_1` on every k (0..718) in both u9779's
`B_U7PR3_K0` and this `B_U7PXR3_K0`. For k >= 357 the number of differing dword offsets drops from 274 to 31 with the knob.
The remaining ones (0x44, 0x48, 0x13c, 0x140, 0x944..0x9a4, 0xaf8, 0xb30, ...) were not examined here.

Original `o_k357_poke` (the `o_r7_1` recipe, `--hold 30`, `--poke-u32 0x007f1040=0x00010000 --poke-delay 20`): **the poke was not
applied**. `scenario_launch.py` runs `--poke-u32` only in the `--no-warp` hold loop, and the log has no `[poke-u32]` line. The
run is a second no-input witness: `DAT_0063ba8c` 6 -> 7 at r7phys frame 1108 (o_r7_1: 1042), then 7 on every later row (783
rows) to the end. The launcher sends no pulses under `--statediff-out` (`scenario_launch.py`, "press pulses are wall-clock-timed
nondeterministic input"), so `o_r7_1` is no-input as well. The existing path to an original exit-branch witness is
`--continue-rounds --continue-rounds-states 7` (pulses control 4 in sub-state 7), not run here.

## 4. Scope and owed

- [UNCERTAIN] the exit branch (a button in a sub-state-7 round) is not measured on the port. The frame alignment of the
  original's 8 -> 2 -> 3/4 states (case 8 and case 2 return without FUN_0040fc00) against the port's single StartRound is not
  ported. The port also never writes byte 0xa of a ctrl block (the keyboard writer `WriteMashedInputGlobalsFromKeyboard` writes
  `0x007f1044 + col`). So with the knob ON, a round-ending elimination with a survivor waits forever in the standalone. For
  interactive play that blocks the match flow until an input path writes byte 0xa.
- The stall stand-in is port-side in every sub-state. Only its sub-state-7 firing is removed here.
- Not run: D2 x3 and the 13-track sweep with `U7PX`, both owed before any flip.
- Trackers (through re-classify, not done here): U-9781's k = 357 residue row resolves to the two port-side timers. The
  FUN_00410860 gate (`0x004108c3..0x004109c5`) is ported default OFF behind `MASHED_R7_EXIT`.

## 5. Decision for Mariano

Flip `MASHED_R7_EXIT` default ON? Knob-off gates are unchanged. With it on (plus the four u9779 knobs, default ON on the
U9779 flip branch), car 3 on B K0 matches `o_r7_1` on the four fields for every k the original has (was k = 357). The cost: in
the standalone a round that ends with a survivor never restarts, because no input writes the exit byte. Options: (1) keep it
default OFF as a measurement knob until an input path writes byte 0xa (`0x007f1042 + idx*0x4c`), then flip with D2 + 13
tracks; (2) flip now for the B / G-CTRL reference only, accepting that interactive matches stall after round 1; (3) leave it
OFF. Recommendation: (1).

## 6. Run 4: merged onto orch/integration, re-gated, `H_U7PX` re-run (2026-10-25)

Branch `orch/D3-CF-K357-M` from `orch/integration` `2e80adace` (the D3-CF-U9779 flip: the four u9779 knobs default ON).
`git merge-base --all` gave two bases (`b5c84688e`, `45dccb4f5`), neither branch an ancestor of the other, so a real merge:
`git -c merge.autoStash=false merge --no-ff orch/D3-CF-K357` @ `a30d080b5` -> merge commit `5943a23f2`. No conflicts
(`TrackRenderer.cpp`, `TrackRenderer.h`, `exe_main.cpp` auto-merged). `MASHED_R7_EXIT` stays default OFF (L-10 = (1)).
Exe `e9532e6e` (`MASHED_NO_DEPLOY=1`, `original\mashed_re_dev.asi` not redeployed). Root `D:\MashedWT\D3-CF-K357_root`,
data `D:\MashedWT\D3-CF-K357_data\arms4`. One `mashed_lock.py run --label D3-CF-K357` hold for all 8 arms, taken at once
(no other holder, no wait line), about 09:22:01 (first arm end 09:22:51 - 50 s) to 09:44:30 local, released by the runner (`data/run_arms4.txt`).

| arm | hashes | reference | verdict |
|---|---|---|---|
| `H_DEF` (G-CTRL) | `ef5f2e10 / 57f7ff83 / b4f4b55a` (15941 / 53273 / 26636 lines) | integration post-flip `ef5f2e10 / 57f7ff83 / b4f4b55a` | **equal** |
| `B_DEF_K0 / K1 / K26` | `c37d4c0c / 0a8f9806 / 7013671d` | integration post-flip `c37d4c0c / 0a8f9806 / 7013671d` | **equal** |
| `H_U7PX` (re-run, report) | `5943c5f1 / b493262b / 09551e6a` (14659 / 56873 / 28436 lines) | batch 3 (exe `3182ad61`) `5943c5f1 / b493262b / 09551e6a` | **bit-identical** |
| `B_U7PX_K0 / K1 / K26` (report) | `3a03670c / 2516d903 / 53778664` | batch 3 `3a03670c / 2516d903 / 53778664` | **bit-identical** |

K357 does not move either default gate on the merged tree. On the merged tree the U7P knobs set by the `U7PX` stack are
the defaults, so `U7PX` = the default + `MASHED_R7_EXIT`.

**`H_U7PX` re-run (HB-LOCKBREAK `verify/lockbreak_20261025/RESULT_HB-LOCKBREAK.md` section 4).** The batch-3 run's last ~30 s
(02:54:58 to 02:55:28) overlapped the D4 gate job's `H_DEF` after the lock break. The run-4 arm ran under a clean hold
(exit 0, self-exited, 519 s) and reproduced all three batch-3 hashes and line counts. Its log has the same single
`R6 ROUND END winner=car3 scores=[4,5,7,8]` and no `MATCH OVER` (`data/arms4/H_U7PX.log`), against 5 round ends and
`MATCH OVER t=55.0s winner=car3` in `H_DEF` (`data/arms4/H_DEF.log`). The section 4 `[UNCERTAIN]` in the HB-LOCKBREAK RESULT
(bit-identity of `H_U7PX` under the overlap) resolves: the overlap did not change it. The batch-3 row's evidence is
replaced by `data/arms4/RUNLOG.txt` line 5. No prediction arm was re-run (none ran inside the window).

Cited files: `data/arms4/RUNLOG.txt` (per-arm env + hashes), `data/run_arms4.txt` (runner stdout, Python SyntaxWarning lines
dropped), the six B step CSVs, `H_DEF.log`, `H_U7PX.log`. The H step / gates / ptrace files stay on D:, their hashes and line
counts are in `RUNLOG.txt`.
