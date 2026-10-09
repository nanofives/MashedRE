# RESULT: what gates branch 2's first firing (U-9204 step 2)

Date 2026-10-09. Read-only analysis of existing captures plus the B2 arms (`RESULT_B2C.md`). No code
changed.

## 0. Verdict first

> **The gate is the same on both sides, and it is met ~2.5 s after GO in the original but ~10 s
> after in the port, because the two (b) recipes drive the PLAYER car differently.**
>
> - **The gate (`FUN_004148b0`, the only return-1 path, exit 114; `AiLeaderTimer.cpp:204-213`):** AI
>   car v fires branch 2 when ITS reference distance `0x008989b0[v]` (`FUN_00442cc0`) is exactly 0
>   (it is the reference car) AND the slot-state-1 car's distance exceeds `_DAT_005cd0a8` = **6.5**,
>   and `DAT_0089a368 != 2`.
> - **Original, confirmed by two independent captures.** Evaluating that condition on every
>   `FUN_004148b0` call of `verify/d3_leader_20261003/o1.msd.leaderprobe.csv` (entry hook, all
>   inputs) gives the first firing at game frame **149 (car 1), 216 (car 2), 180 (car 3)**. These are
>   exactly the first non-zero `ret148b0` calls of the separate capture
>   `verify/d3_elim_20261003/o_t3.msd.aistep.csv`. The original's player falls behind:
>   reference distance 0 -> 1.70 -> 4.85 -> 8.55 -> 9.52 at frames 0 / 50 / 100 / 150 / 200, while
>   AI cars take distance 0.
> - **Port (B2 arm `B2K00`):** the player takes reference distance 0 by frame ~100 and the AI fall
>   30+ behind. The condition first holds at frame **643**, in line with RESULT_WIRE2's 558-660.
>   The port's player drives at full pace: `L2_CTRL.ptrace.log` speed 1,432 / 1,108 / 906 at frames
>   50 / 100 / 150.
> - **So the timing gap is a SCENARIO difference, not an AI-code difference.** The original's (b)
>   reference captures (`scenario_launch --statediff-out`) suppress all presses, while the port's (b)
>   recipe (`run_h4.ps1` / `run_bscore.ps1`) has its player driven. Which knob drives it is not
>   established here: none of `MASHED_DRIVE_DEMO`, `PLAY_DEMO`, `STEER_HOLD` or `AI_DRIVES_PLAYER` is
>   set ([UNCERTAIN] U-9205).

## 1. Consequence for criterion (b)

The (b) window is the AI's first 220 calls. In the original those calls race a player who falls
behind, and branch 2 shapes their last ~70 calls. In the port they race a player who leads. So **the
regime-A comparison (2/3, car 2 failing) and the regime-C comparison are both made across different
scenarios**. That is a candidate cause of car 2's failure, unmeasured.

The start-of-race reference distances also match across sides only with the car indices reversed:
port v0..v3 = 2.739 / 1.829 / 1.553 / 0, original v0..v3 = 0 / 1.553 / 1.833 / 2.750. That needs
checking before any per-car comparison (option (iv)).

## 2. Next concrete step

Find what drives the port's player in the deterministic recipe, give the port's (b) recipe the
original's player input (none), pre-register the predicted branch-2 first firings (149 / 216 / 180 if
the AI is faithful), and re-score (b) per regime.

## 3. Raw

`GATE_ORIG_O1.txt` (the o1 condition evaluation), `B2K00.gates.csv` (port reference distances),
`../d3_gatefire_20261008/L2_CTRL.ptrace.log`, `../d3_elim_20261003/o_t3.msd.aistep.csv`.
