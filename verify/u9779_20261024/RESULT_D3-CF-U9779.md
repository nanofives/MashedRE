# RESULT D3-CF-U9779: U-9781 is FUN_0046e9e0's sub-state gate. With `MASHED_R7_PHYS`, car 3 is bit-exact through k=356 (was 220) (2026-10-24)

Branch `orch/D3-CF-U9779` from `orch/integration` `30913130b`. Probe tool `b0b6731bb`, PREREG `1c5ec4fda` (committed alone, before
every knob arm). Exe `563c27c2` (`MASHED_NO_DEPLOY=1`). Private root `D:\MashedWT\D3-CF-U9779_root`. Evidence
`D:\MashedWT\D3-CF-U9779_data` (original `o_r7_1.*`, arms `arms\`, batch log `run_arms1.txt`). No default changed.
`original\mashed_re_dev.asi` was not redeployed.

## 1. Cause (static, MASHED.exe.unpatched)

Three physics-frame sites read FUN_0040e350 (`DAT_0063ba8c`) and treat anything but 6 / 0xb / 0xa alike:

- FUN_0046e9e0 `0x0046ea1e..0x0046ea4c`: the three position increments (dt * `_DAT_005cc948` * `_DAT_005cea80` * `+0x9b0..+0x9b8`)
  are set to 0, so `0x0046ea5f` / `0x0046ea69` / `0x0046ea73` store dst = src. **That is the freeze.** U-9781's writer `0x0046ea62` is
  the instruction after the x store `0x0046ea5f`.
- FUN_004709a0 `0x00470a31..0x00470a4f`: the ctrl block FUN_0046e9e0 gets (`0x00470ac9`) becomes `0x007f19b8`.
- FUN_00470c70 `0x00470fdb..0x00470ff9`: the ctrl block A4 FUN_00470670 gets (`0x00471066` / `0x00471076`) becomes `0x007f19b8`.

The port: the exe's `Fi_GameMode()` returned the constant 6, so u9779's `MASHED_ROUND_END7` never reached the physics. The position
step listed the gate as a deviation and did not apply it. The AI io took the slot's own ctrl block.

## 2. Original witness `o_r7_1` (new `--r7phys-probe`)

Recipe as `o_cc3_1` (`--track 0 --mode 10 --cars 4 --car 0 --poke-ctrl-slots --statediff-car 3 --statediff-aistep --hold 25`) plus
`--r7phys-probe`: entry hooks on FUN_00470c70 and FUN_004709a0 only, 4627 rows. Car 3's records match U-9781 (frozen at 6.0739 /
-22.0905 from k=220, vx 1676.83, 1396.51, ...).

- The first FUN_00470c70 call with `DAT_0063ba8c` = 7 is call 1041 (= record k 220). The clock `0x007f0ff4` stays 52000 from there.
- FUN_00470c70 gets ms = 50 on 220 / 220 racing frames and on 502 / 503 sub-state-7 frames (one with no substep). FUN_004709a0 runs
  twice per frame with dt 25.0 in both states. **The U-9781 reading of "one substep per frame" does not hold.** The o_ww_2 watch saw
  equal-value writes, not one substep.
- Substep 1 of call 1041 writes the other ring half equal to the entry position (`+0x998` 6.0274033 -> 6.0739159), the zeroed
  increment.
- `0x007f19b8`: 0x4c zero bytes on the first row and unchanged across all 4627 rows. Car 3's own block (index 3) reads
  `00 00 00 00 00 ff 00 00` on the same frames.
- Velocity moves only between FUN_00470c70's entry and its first FUN_004709a0 (the A4 loop), then x0.85 into the next frame
  (FUN_00424eb0's FUN_0046c570): 2009.3203 -> 1707.9223 -> 1676.8298 -> 1425.3054.

## 3. Knob `MASHED_R7_PHYS` (default OFF)

It acts only while u9779's round-end flag `s_roundEnd7` is up (`MASHED_ROUND_END7`, a rule-engine round end, until StartRound).
`D3d9Render::RoundEnd7PhysActive()` is in `TrackRenderer.cpp`.
- (a) `Fi_GameMode()` (`Vehicle/ForceIntegratorStubs.cpp`) returns 7, so every exe physics reader of FUN_0040e350 sees it.
- (b) `VehiclePhysicsRun.cpp` position step: the increment factor is 0 when the mode is not 6 / 0xb / 0xa (`0x0046ea3c..0x0046ea4c`).
- (c) `StepBegin` (`VehiclePhysicsRun.cpp`): the car's 8 ctrl bytes are read from `0x007f19b8` in place. This one site covers the
  per-car step, the substep-outer step and the A4 pre-phase.

Stated scope: the d2engage hold tick (mode 5) stays outside (b) / (c), as before.

## 4. Results (B recipe, `verify/u9779_20261024/run_arms.py`; records against `o_r7_1` with u9779's `reck.py`)

| prediction | arm | outcome |
|---|---|---|
| P0 knob unset | `H_DEF` | `bacfc747 / 8dd1c004 / 6f9932b6` = default. **holds** |
| P0 knob unset | `B_DEF_K0 / K1 / K26` | `f9deb9b9 / d3601fcc / 9da9a26a` = default. **holds** |
| (control) knob alone | `B_P7_K0` | `f9deb9b9` = default (inert without `ROUND_END7`) |
| P1 | `B_U7R3_K0` | first difference k = 220 (dx +0.0756, dvx -44.31). +0x958 keeps integrating (6.1495, 6.2106, ...). **holds** (U-9781 reproduced on the current default) |
| P2 | `B_U7PR3_K0` | +0x958 / +0x960 = 6.0739159 / -22.0904903 on every k in 220..356 (frozen). **holds** through 356 |
| P3 | `B_U7PR3_K0` | first difference on (+0x958, +0x960, +0x9b0, +0x9b8) moves from 220 to **k = 357**. Velocity is bit-equal throughout (1676.8298, 1396.5137, ... 0.0). **holds** |
| P4 | `B_U7P_K0 / K1 / K26` | `c37d4c0c / 0a8f9806 / 7013671d`, against U7 K0 `69795f64` (step file). `B_U7PR3_K0` = `B_U7P_K0` (`c37d4c0c`): the record dump does not perturb. **holds** |
| report | `H_U7P` | `ef5f2e10 / 57f7ff83 / b4f4b55a` |

Residue at k = 357 (new, outside U-9781): the port starts round 2 there. Its log shows `R6 round 1 OVER t=6.7s`, then StartRound
moves car 3 to the grid (12.0, -18.0, velocity 0). The original's car 3 stays in sub-state 7 on the same frames and to the end of the
25 s capture (502 sub-state-7 frames, clock 52000, position frozen). The port's 137-frame round-result hold is a rule-engine /
frontend timer, not physics. The original's sub-state-7 exit (writer of `DAT_0063ba8c` 7 -> next, and what it waits on) is not
measured [UNCERTAIN: no original capture leaves sub-state 7 within --hold 25].

Not run, owed before any flip: D2 x3 and the 13-track sweep with `U7P`. (b) is inert by construction: its window ends at k = 220.

## 5. Trackers (owed, through re-classify, not done here)

- U-9781: resolved. Cause FUN_0046e9e0 `0x0046ea1e..0x0046ea4c` + the `0x007f19b8` ctrl block (`0x00470a31..0x00470a4f`,
  `0x00470fdb..0x00470ff9`). Knob `MASHED_R7_PHYS` default OFF, car 3 bit-exact k 220..356 on B K0. The "one substep" statement in
  its row is refuted by `o_r7_1`.
- New row: the sub-state-7 exit / round-restart timing. The port starts round 2 at k = 357 (137 frames after the round end). The
  original is still in sub-state 7 at k = 721 (end of `o_r7_1`). Path: a longer original hold (`--hold 60`) with a `--watch-write` on
  `0x0063ba8c` for 7 -> next.

## 6. Decision for Mariano (filed as a ledger decision row, no flip)

Flip `MASHED_ELIM_RESOLVE` + `MASHED_ROUND_END7` + `MASHED_RACE_TAIL_LATE` + `MASHED_R7_PHYS` default ON together? Knob-off gates are
unchanged. With the four on, B K0 is bit-exact for car 3 through k=356. Owed first: D2 x3 and 13 tracks with U7P, plus the new
round-restart row. Recommendation: run those two gates, then flip all four as one set. A partial set makes no faithful sense
(R7_PHYS needs ROUND_END7's flag).

Scripts: `run_arms.py` (copy of `verify/savelink_20261023/run_arms.py`, adds `U7`, `U7P`, `P7`, `*R3`). `reck.py` is u9779's
(`verify/u9779_20261021/reck.py`).
