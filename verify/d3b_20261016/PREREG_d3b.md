# PREREG d3b: the AI's RW-LUT arithmetic (FUN_00443dc0 lookahead, FUN_00415e20 steer error) and D3 (b)

Branch `work/d3b`, base `race/first-frame-parity` `2cc15b2cf`. Model claude-opus-5-5 (account3). Written after the
re-score below and BEFORE any run of a new knob.

## 0. Re-score of the default (done, no knob)

Base exe `68e037d7` (built from `2cc15b2cf`, `MASHED_NO_DEPLOY=1`), private root, `run_arms.py`.
- G-CTRL `H_DEF` = `d3b90ee0 / 80f01f35 / 56d62ae7` (the default trio).
- (b) K0 / K1 / K26 step dumps `17d07c90` / `062a3c98` / `b8124a63`, byte-identical to d2close's.
- (b) per regime, every item (`percar.py`, `SCORE_DEF_PERCAR.txt`): **A 1/3, B 2/3, C 0/3.** A: car 1 and car 3 fail
  `abs_steer~` 0 (original car 1 = 7, car 3 = 23). B: car 2 fails `c1#` 60 / `steer#` 93 (bounds 17..46 / 29..84).
  C: car 1 fails 6 items, car 2 4, car 3 5. The extra phases K2 (A), K4 / K27 (B), K54 / K57 (C) give the same verdicts.

## 1. Observation that motivates the arms (from the committed dumps, no new run)

Regime A, car 1, window steps 18..39 (`B_DEF_K0` vs `verify/d3_regimes_20261013/o_reg_r1`): speed (`rec_9e4`) and
position agree within 0.2 and 0.005, but the look point does not. The original's `look_z` jumps between -7.46 and
-10.12 from one step to the next and its steer follows those jumps (steer 0 exactly on the far picks). The port's
`look_z` advances smoothly by about 0.1 per step and its steer decays to 0 from step 28 on. Over the 220-call window
the port's car 1 / car 3 hold steer 0 on 131 / 138 calls, the original's on 102 / 77.

The look point is the max of 16 alignment scores in `FUN_00443dc0` phase 6 (`decomp_00443dc0.txt`):
`m = dot(norm(C(p_i) - C(p_{i-1})), norm(C(p_{i-1}) - own))`, `m > 1.0 -> 1.0` (`_DAT_005cc320`), strict `<`, first
index wins. The original normalizes both vectors with `FUN_004c3c60` (RW fast-sqrt LUT, `decomp_math.txt`), the port
(`Ai/AiStandalone.cpp` `Normalize2`) with `std::sqrt`. Phase 8's two lengths are `FUN_004c3bf0` (LUT) in the
original and `std::sqrt` in the port. `FUN_00415e20` (the steer error) normalizes both its vectors with
`FUN_004c39b0` (LUT, 3-D) in the original and `std::sqrt` in the port (`SteerAngleErrorFwd`).

## 2. Hypothesis and knob

H-LUT: the port's smooth look point and its long steer-0 runs come from computing those three sites exactly.
Knob `MASHED_AI_LUT=<mask>` (exe only, default unset = current code): bit 0x1 = phase 6 normalize via
`Math::LutNormalize2D` (`FUN_004c3c60`) and phase 8 lengths via `Math::LutSqrt(dz*dz + dx*dx)` (`FUN_004c3bf0`);
bit 0x2 = `SteerAngleErrorFwd`'s two normalizations via `Math::LutNormalize` (`FUN_004c39b0`).
Replay (measurement only): `MASHED_AI_LOOK_REPLAY=<original aistep csv>` + `MASHED_AI_LOOK_REPLAY_OUT=<csv>` feeds
each recorded row's `spline`, `v`, `own_x`, `own_z` through the port's `SplineLookahead` twice (exact, LUT), each mode
with its own index-continuity state starting at 1000, and writes both look points beside the original's.

## 3. Gates (decision criteria fixed here)

| gate | arm(s) | pass |
|---|---|---|
| G-NOOP | knob build, no knob: `H_NOOP`, `B_NOOP_K{0,1,26}` | trio `d3b90ee0 / 80f01f35 / 56d62ae7`, K dumps `17d07c90 / 062a3c98 / b8124a63` |
| G-REPLAY-LOOK | `R_LOOK` on `o_reg_r1` (A), `o_reg_r13` (B), `o_reg_r19` (C), rows with `ai_mode` 0 | LUT mode: look point bit-equal to the original's on >= 99 % of rows on each file. Prediction: exact mode is bit-equal on fewer than 50 % of the window rows |
| G-TOOK | `H_L1`, `H_L3` vs `H_NOOP` | step hash differs (the knob changes the run) |
| determinism | `H_L3` = `H_L3-2` | identical trio |
| (b) | `B_L{1,2,3}_K{0,1,26}` (+ K2/K4/K27/K54/K57 report) | per regime n/3 and per-car items vs section 0 |
| collateral | `coll.py` on `H_L3` (floor `H_L3-2`) vs `H_NOOP` (floor `H_NOOP-2`) | at most +2 fields over the base's count |
| D2 | `D2_L3_{1,2,3}` | three metrics inside the `d81a8df6` bounds (`score_d2.py`), H1 PASS. The D2 recipe is solo, so the prediction is byte-identical motion_diag to the default |

Predictions for (b), written now: with bit 0x1, regime A car 1 and car 3 `abs_steer~` rise above 0 (the look
jitter returns). No prediction is registered for B and C.

## 4. Default decision (fixed here)

An arm becomes the default (opt-out `MASHED_NO_AI_LUT=1`) only if all hold: G-NOOP, G-REPLAY-LOOK (for bit 0x1;
bit 0x2 has no replay and needs (b) only), G-TOOK, determinism, D2 in bounds, collateral within +2, and (b) loses no
car that passes on the default in any regime (per-regime n/3 >= section 0 and no passing car turns to FAIL).
Otherwise the knob stays OFF and the result is reported with a recommendation. A faithful site that fails (b) stays
OFF and is reported as a user decision.

## 5. Report-only arms (no default change from these)

The candidates on file, re-scored on this base and stacked with the winning LUT mask if one exists:
`MASHED_TGT3=1 MASHED_PATHPROG=1` (U-9684 brake), the player first-step settle (U-9684 player-at-rest row; the
pathprog_a section 6 "decision" is UNCONFIRMED, so measured as a report arm only, never default ON).

## Amendment 1 (2026-10-16, after LR_LOOK, H/B arms of section 3; before any G arm)

Results so far, scored as registered:
- G-NOOP **PASS** (B side): `B_NOOP_K0/K1/K26` = `17d07c90 / 062a3c98 / b8124a63` on the knob exe `1abe36ef`.
- G-REPLAY-LOOK **FAIL as registered** (`LR_LOOK.look.csv`): LUT mode bit-equal on 232 / 3037 (A), 330 / 3184 (B),
  245 / 524 (C) window rows, exact mode 189 / 285 / 202. Under 1e-5 the LUT mode agrees on 98 % of the 220-call window
  rows of cars 1-2, and on the rows where the two modes pick different points (> 1e-4 apart) the LUT matches the
  original on 225 of 237, the exact mode on 0. The last-ulp residual and car 3's 44 / 53 / 15 rows off in both modes are
  not explained (open). So bit 0x1 is the original's look-point arithmetic, and the registered bit gate fails on a
  residual outside it.
- (b) prediction of section 3 **missed**: `B_L1/L2/L3`: A **0/3** on every mask (cars 1 and 3 still `abs_steer~` 0,
  car 2 now fails `c1#` 74-77 / `steer#` 97-102), B 2/3, C 0/3.

Diagnosis that motivates the new arms (no new run): `hist_dc` is the steer error itself (`0x008032dc + v*0x14`,
`AiStandalone.cpp` 0x004165f7). Under L1 the look point jumps like the original's but car 1's err decays to 0.25-0.9
from step 25 while the original's stays 0.85-1.7, so steer falls under the deadband. Same look point, smaller error =
the port's car-1 heading is turned further toward the target. The default spawns AI cars at yaw -1.5760 rad, the
original's grid law (`FUN_00408b00`, gridyaw) gives -1.5708 (verify/gridyaw_20261016 P_DEF / P_G carpos), and car 1 at
x -1.0933 vs -1.1012.

New arms: `G` = `MASHED_GRID_DIR` (gridyaw, all slots), `L1G`, `L3G`; `B_*_K{0,1,26}`, `H_*` pairs, D2 x3.
Predictions: with `L1G`, car 1's err at window steps 18..26 lies within 0.2 of the original's on at least 7 of 9 steps,
and A car 1 `abs_steer~` > 0. `MASHED_GRID_DIR` moves the PLAYER's spawn too, and gridyaw measured it out of the D2
bounds on the W default (`WG` 0.2007 / 0.2634 / 1758.25), so any arm containing it is report-only for the default
unless an AI-slot-only form is registered in a later amendment. Section 4's rule is unchanged.

## Amendment 2 (2026-10-16, written after LR_M1 / LR_M5 ran, before any live arm with bit 0x4)

New knob bit 0x4: `FUN_00443300` (Catmull-Rom) in the listing's operand order (`disasm.py 0x00443300`: t3 = t2*t,
A = ((2P0 - 5P1) + 4P2) - P3, B = ((3P1 - P0) - 3P2) + P3, out = (((A t2 + B t3) + (P2 - P0) t) + 2P1) * 0.5). The port's
`CatmullRom` sums the same terms in another order. Added after the amendment-1 residual analysis, so LR_M5 is a re-run
of the section 3 gate on a further fidelity fix, not a blind test. Disclosed as such.

G-REPLAY-LOOK on mask 5 (`LR_M5.look.csv`, `score_replay.py`):
- as registered literally (every ai_mode-0 row of each file): A 5225 / 5269 = 99.2 %, B 5098 / 5434 = 93.8 %,
  C 2597 / 2618 = 99.2 %. B fails the literal 99 %.
- every miss but one per file is a row where the original's look is (0, 0) (A 43, B 335, C 20): the original left
  `FUN_00416250` before the probe at 0x004165a5 wrote a look point, so there is nothing to compare. The other miss is
  car 1's first captured row (frame 2), before which the original's index word was already set.
- excluding the probe-zero rows: A 5225 / 5226, B 5098 / 5099, C 2597 / 2598 (>= 99.98 %). In the nine (b) windows
  (220 calls, cars 1-3, three regimes): **1384 / 1384 bit-equal** (mask 1: 685; exact: 559).
Verdict recorded: mask 5 reproduces the original's lookahead bit for bit on every comparable (b)-window row. The
literal gate is reported as failed on B because of the probe-zero rows.

New arms: `L5` (`MASHED_AI_LUT=5`), `L7` (=7), `L5G`, `L7G` (+ `MASHED_GRID_DIR`), `B_*_K{0,1,26}`; `H_L7`, `H_L7-2`.
No new (b) prediction: the amendment-1 runs show the A failure is not decided by the look point alone (section 1 of
amendment 1). Section 4's default rule is unchanged and applies to L5 / L7 (bit 0x4 is a no-op when unset).

## Amendment 3 (2026-10-16, before the original-side capture `o_lb1`)

Finding from committed captures (no new run, `heading.py`, `o_rd3.msd` car 1, regime A): on car 1's steer-0 window
steps (27, 28, 29, 34, 38) the original's body heading (atan2 of +0x9d4/+0x9dc) moves -0.161, -0.170, -0.179,
-0.188, -0.198 deg per frame; with steer -8 / -10 it moves +0.080 / +0.100. On those frames +0x10 = 0 (driving arm),
+0x9e0 = 4.0, the wheel steer +0x1a8 = 0, and +0x148 decays by exactly damp^2 (0.014352 x 0.934444 = 0.013411, read
0.013412), so `FUN_0046e9e0` saw no steer and added +k2*accum (predicted +0.077 deg per frame). The port shows
+0.0785 at its first steer-0 frame (its dump is one physics step behind the original's .msd row). So the original
turns the body by about -0.24 deg per frame on steer-0 frames through something other than `FUN_0046e9e0`'s
steer/accumulator terms, and the port has no such term.

Capture (one original run, shared install per the 2026-10-12 re-run rule, own PID): the regime-A reference argv +
`--statediff-car 1 --lat-bracket --lat-bracket-solver` (new opt-in site 3 = `0x0046f6c0` entry, ESI-filtered; the at-rows of both body
matrices, +0x9a8/+0x9ac, +0x144..+0x14c and +0x1a8 appended to every sample). Gates:
- G-REPRO: the run's car-1 window (aistep) equals `o_rd3`'s c0/c1/c4/c5 sequence over the 220 calls (else re-run;
  a piz-collision AV = re-run).
- G-SPLIT (report): on the steer-0 frames, the heading change of the live matrix between consecutive samples,
  per interval: [A6a entry -> A6b entry] (A6a), [A6b -> substep entry], [substep entry -> solver entry] (FUN_0046e9e0
  of substep 1), [solver -> next substep / next A6a]. Prediction: the -0.24 deg term sits outside
  [substep entry -> solver entry], i.e. FUN_0046e9e0's interval carries only the +k2*accum turn.
