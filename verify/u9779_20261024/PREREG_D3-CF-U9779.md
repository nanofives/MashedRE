# PREREG D3-CF-U9779: sub-state-7 physics (U-9781), knob `MASHED_R7_PHYS` (2026-10-24)

Branch `orch/D3-CF-U9779` from `orch/integration` `30913130b`. Committed alone, before every knob arm. Evidence
`D:\MashedWT\D3-CF-U9779_data`. Private root `D:\MashedWT\D3-CF-U9779_root` (`MASHED.exe.unpatched` SHA-256 = anchor).

## 1. Static reading (MASHED.exe.unpatched, capstone)

FUN_0040e350 returns `DAT_0063ba8c`. Three sites in the physics frame read it and treat anything but 6 / 0xb / 0xa alike:

| site | what it does when FUN_0040e350() is not 6, 0xb, 0xa |
|---|---|
| FUN_0046e9e0 `0x0046ea1e..0x0046ea4c` | the position increments `[esp+0xc]` / `[esp+0x10]` / `[esp+0x14]` (dt * `_DAT_005cc948` * `_DAT_005cea80` * `+0x9b0..+0x9b8`, `0x0046e9e3..0x0046ea1a`) are set to 0, so the stores `0x0046ea5f` / `0x0046ea69` / `0x0046ea73` write dst = src |
| FUN_004709a0 `0x00470a31..0x00470a4f` | the ctrl-block pointer `[esp+0x10]` handed to FUN_0046e9e0 (`push ecx` `0x00470ac9`) becomes `0x007f19b8` instead of `0x007f1038 + [0x007f1a14 + i*0x10] * 0x4c` |
| FUN_00470c70 `0x00470fdb..0x00470ff9` | the ctrl block `edi` handed to A4 FUN_00470670 (`push edi` `0x00471066`, call `0x00471076`) becomes `0x007f19b8` |

The orientation step of FUN_0046e9e0 (`0x0046ea76..0x0046eaa6`) has no such gate. FUN_00470c70's only caller is `0x00425a8d`
(FUN_00425a40), FUN_004709a0's only caller is `0x00471137`, FUN_0046e9e0's only caller is `0x00470ad3`.

The port: the exe's `Fi_GameMode()` (`Vehicle/ForceIntegratorStubs.cpp`) returns the constant 6, so `MASHED_ROUND_END7`
(u9779) never reaches the physics. `VehiclePhysicsRun.cpp`'s position step states the 0x0046ea1e gate as a deviation and does
not apply it, and the AI io takes the slot's own ctrl block (`TrackRenderer.cpp`, the opponent loop and the A4 pre-phase).

## 2. Witness capture (original, before this PREREG)

`o_r7_1`: the u9779 B recipe (`--track 0 --mode 10 --cars 4 --car 0 --poke-ctrl-slots --statediff-car 3 --statediff-aistep
--hold 25`) plus the new `--r7phys-probe` (entry hooks on FUN_00470c70 and FUN_004709a0 only, car 3's record and ctrl block, a hex
dump of `0x007f19b8` whenever its bytes change). 4627 rows, `D:\MashedWT\D3-CF-U9779_data\o_r7_1.msd.r7phys.csv`.

- The first FUN_00470c70 call with `DAT_0063ba8c` = 7 is call 1041. The clock `0x007f0ff4` stays 52000 from there.
- FUN_00470c70 gets 50 ms on every racing frame (220 / 220) and on every sub-state-7 frame (502 / 503, one with 0 substeps), and
  FUN_004709a0 runs twice per frame at 25.0 in both. The o_ww_2 reading "one substep's writers per frame" in U-9781 is a
  watch artefact (equal-value writes), not a substep count.
- Car 3's position x is 6.0739159 at the entry of call 1041 and at every later row: substep 1 of call 1041 writes the other ring
  half equal to it (`p1x` 6.0274033 -> 6.0739159), as the zeroed increment predicts.
- `0x007f19b8`: 0x4c zero bytes on the first row and never changes (one dump in 4627 rows). Car 3's own block (index 3) is
  `00 00 00 00 00 ff 00 00` on the same frames.
- Velocity changes only between FUN_00470c70's entry and its first FUN_004709a0 (the A4 loop), and by x0.85 between frames
  (FUN_00424eb0's FUN_0046c570): vx 2009.3203 (end of call 1040) -> 1707.9223 (entry 1041) -> 1676.8298 (after A4) ->
  1425.3054 (entry 1042).

## 3. Knob `MASHED_R7_PHYS` (default OFF)

Acts only while u9779's round-end flag `s_roundEnd7` is up (`MASHED_ROUND_END7`, a rule-engine round end, until StartRound):

- (a) the exe's `Fi_GameMode()` returns 7 (= FUN_0040e350 on those frames), for every exe physics reader of it;
- (b) the position increment of the A9 step is 0 (`0x0046ea3c..0x0046ea4c`);
- (c) the car's ctrl input is the 8 bytes at `0x007f19b8` in place of its slot block (`0x00470a1b..0x00470a4f`,
  `0x00470fd6..0x00470ff9`), in `StepBegin` (so the per-car step, the outer step and the A4 pre-phase all read it).

Not covered (stated): FUN_0040e350 = 5 on the d2engage hold tick stays outside (b) / (c), as today.

## 4. Arms (B recipe, current harness `verify/savelink_20261023/run_arms.py` copied to `verify/u9779_20261024/run_arms.py`)

Stacks: `U7` = `MASHED_ELIM_RESOLVE`, `MASHED_ROUND_END7`, `MASHED_RACE_TAIL_LATE` (u9779 R7). `U7P` = `U7` + `MASHED_R7_PHYS`.
`*R3` adds `MASHED_RECDUMP` slot 3. Arms: `H_DEF`, `B_DEF_K0/K1/K26` (knob unset), `B_U7R3_K0`, `B_U7PR3_K0`, `B_U7P_K0/K1/K26`,
`H_U7P`. Records compared with u9779's `reck.py` against `o_r7_1` (car 3).

## 5. Predictions

- P0 (gates): with the knob unset G-CTRL is `bacfc747 / 8dd1c004 / 6f9932b6` and B `f9deb9b9 / d3601fcc / 9da9a26a`.
- P1: `B_U7R3_K0` reproduces U-9781: car 3's first record difference against `o_r7_1` is at k = 220, and the port's +0x958 keeps
  changing for k > 220.
- P2: `B_U7PR3_K0`: car 3's +0x958 / +0x960 are constant for every k > 220 up to the dump's end (the freeze).
- P3: `B_U7PR3_K0`: car 3's first record difference against `o_r7_1` on (+0x958, +0x960, +0x9b0, +0x9b8) is past k = 220.
  If it is at 220, the residue is reported by field and not explained here.
- P4: `U7P` changes the B hashes against `U7` (the knob acts after a round end). `H_U7P` is reported only.
- Out of scope, owed before any flip: D2 x3 and 13 tracks with the knob on, (b) (inert: its window ends at 220).

No default changes. A flip is a decision row.

## 6. Amendment 1 (2026-10-24, before its arms): the flip gates (decision L-4 = (1), Mariano)

Exe `563c27c2` (HEAD `45dccb4f5`, `MASHED_NO_DEPLOY=1`). Stack `U7P` = `MASHED_ELIM_RESOLVE` + `MASHED_ROUND_END7` +
`MASHED_RACE_TAIL_LATE` + `MASHED_R7_PHYS`.

| gate | arm | pass rule |
|---|---|---|
| G-D2 | `D2_U7P_1/2/3`, alone (`mashed_lock.py run --solo`), `verify/d2rebase_20261019/score_d2_v2.py port` | PASS 3/3 on the 8-run band |
| G-13 | `verify/rsky_20261016/run13.py run` with the four knobs, root `D:\MashedWT\D3-CF-U9779_root13`, scored with `verify/rres_20261016/cites_rres.json` | 0 RED, INSTRUMENT GATES PASS; row changes against rpct's `tracks_flip_score.txt` (the current default's table) cited |

If both pass: the four knobs flip default ON as one set (opt-outs `MASHED_NO_ELIM_RESOLVE`, `MASHED_NO_ROUND_END7`,
`MASHED_NO_RACE_TAIL_LATE`, `MASHED_NO_R7_PHYS`), then re-gate on the flipped exe: G-CTRL x2 and B K0 / K1 / K26 (new default
hashes recorded), the all-opt-out arm = the pre-flip default (`bacfc747 / 8dd1c004 / 6f9932b6`, `f9deb9b9 / d3601fcc / 9da9a26a`),
and with the flipped stack B = `B_U7P` (`c37d4c0c / 0a8f9806 / 7013671d`), G-CTRL = `H_U7P` (`ef5f2e10 / 57f7ff83 / b4f4b55a`).
If either fails: no flip, RED with every failing row cited. The k = 357 round-restart residue (D3-CF-K357) is noted, not fixed.
