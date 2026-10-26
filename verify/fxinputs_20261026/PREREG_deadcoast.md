# PREREG D4-FX-INPUTS attempt 2: `MASHED_DEAD_COAST` (killed cars keep their physics) (2026-10-26)

Committed alone, before any arm of this knob runs.

## 0. The writer (measured before this PREREG, original side)

`re/frida/scenario_launch.py --track 0 --mode 10 --cars 4 --car 1 --rule 0 --hold 300 --statediff-car 2
--watch-write 4,9b0` on the original (raw: `D:/MashedWT/D4-FX-INPUTS_data/ww3_c2.*`, scored by
`ww_kill_writers.py` and `msd_dead_decay.py` in this folder). One kill of car 2 was recorded:

- the `rec+0x4` write: pc `0x0046c5e4` (`FUN_0046c5c0`), ret `0x00422fe0` (`FUN_00422fd0`), ret `0x0041102a`
  (`FUN_00410d10`), ret `0x00411608`.
- after it, every `+0x9b0` write comes from the ordinary physics chain: pc `0x0046e2c2` / `0x0046e340` (ret
  `0x00470948` / `0x00470928`, the substep `FUN_004709a0` called from `0x0047107b`.. inside `FUN_00470c70`), pc
  `0x0046868a` / `0x004688cc` / `0x0046894c` (ret `0x00470948` / `0x0047093b`). 19,389 writes while `rec+0x4 == 0`,
  3,852 of them leave `+0x9b0` at 0.0 (pc `0x0046894c` / `0x0046e2c2` / `0x0046e340`).
- per frame (`.msd`): `rec+0x4` 1 -> 0 and `rec+0x10` 0 -> 1 on frame 1726, |v| 297.67 at the kill, then 283.99,
  278.46, ... 24.98, 19.87, and exactly 0.0 from frame 1753 (27 frames), 0.0 on 3,852 of the 3,879 dead frames,
  0 frames frozen at a non-zero velocity, 4 frames passing the FX speed gate (|v|^2 >= 62500).

So there is no dedicated zeroing writer. The original keeps stepping the killed car:

- `FUN_00422fd0` calls `FUN_0046c5c0(v)` (`rec+0x4 = 0`, `rec+0xb10 = DAT_007f1030`) and `FUN_0046c790(v, 1)`
  (`rec+0x10 = 1`, `(&DAT_008815b0)[v*0x341] = param_2`).
- `FUN_00470c70` A4 loop (decomp, pool slot 10): the car qualifies when `rec+0x14 == 0 && (rec+0x4 != 0 ||
  rec+0x10 == 1)` (0x00470ffe / 0x0047100a / 0x00471012). The input block is `&DAT_007f1038 + slot*0x13` dwords,
  replaced by `&DAT_007f19b8` when `rec+0x9f0 != 0` or `rec+0x10 != 0` (`piVar14[-0x278]`, piVar14 = rec+0x9f0) or
  `FUN_0040e350()` is not 6 / 0xb / 0xa. The substep `FUN_004709a0` makes the same substitution
  (`re/analysis/vehicle_dynamics/004709a0.md`) and has the same qualifying guard.
- `&DAT_007f19b8` read 0x4c zero bytes on the original (`o_r7_1`, VehiclePhysicsRun.cpp MASHED_R7_PHYS note).

The port instead skips a killed AI car in the opponent loop (`TrackRenderer.cpp`, `if (round_mode_ &&
!race_[ci + 1].alive) { ... continue; }`) and in the A4 pre-phase, so its record is never written again and
`+0x9b0..+0x9b8` stays at the last racing value (98.6% / 98.7% of slot 1 / 2 frames, attempt 1).

## 1. The knob

`MASHED_DEAD_COAST=1`, default OFF. With it unset the exe is byte-for-byte the previous behaviour.

1. Every port kill site (the two `race_[victim].alive = false` elimination sites and `KillCar`) also writes the
   victim's physics record: `+0x4 = 0` (`FUN_0046c5c0`, 0x0046c5c0) and `+0x10 = 1` (`FUN_0046c790(v, 1)`).
   `+0xb10 = DAT_007f1030` is NOT ported: the port has no reader of `+0xb10` (grep, Vehicle/ and TrackRenderer.cpp).
2. `VehiclePhysics_StepCar`: when `rec+0x10 != 0`, the car's input is the 0x4c-byte block at `0x007f19b8`, read in
   place as MASHED_R7_PHYS already does (`FUN_00470c70` / `FUN_004709a0` substitution).
3. The opponent loop no longer skips a dead AI car: it steps it through `StepCar` like a live one (same io build).
   For a dead car it skips the scaffold-only branches that have no original counterpart on a dead car: the
   power-up spin-out branch and the 2.5 s stuck relocation stand-in (a stopped car's gate never changes).
4. `StartRound` restores `+0x4 = 1` / `+0x10 = 0` on the cars this knob marked.
   [UNCERTAIN] the original writer that clears `+0x10` and sets `+0x4` back to 1 on a new round is not measured. The
   G-CTRL recipe plays one round on the port (`R6 round 1 OVER t=6.7s`), so no scored arm reaches it.

Not covered: the player (slot 0) already steps every frame. With the knob its input becomes the zero block after a
kill, as on the original. The `MASHED_A4_ORDER` pre-phase (default OFF) still skips dead cars.

## 2. Arms

Exe: this branch's build, `MASHED_NO_DEPLOY=1`, private root `D:\MashedWT\D4-FX-INPUTS_root`, every run inside
`mashed_lock.py run --label D4-FX-INPUTS`.

- **G (gates, knob OFF):** `verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`.
- **GON (gates, knob ON, report only):** the same four arms with the `DC` stack (`MASHED_DEAD_COAST`), copy of the
  runner in this folder.
- **X0 / X1 / X2 (FX + record, G-CTRL recipe):** `fx_arms.py` F1 env (`MASHED_FX_ORIG_GATES`, `MASHED_FX_DEBUG`)
  plus `MASHED_RECDUMP` for one slot. X0 = knob OFF, slot 1. X1 = knob ON, slot 1. X2 = knob ON, slot 2.

## 3. Measures and pass rules

M1, killed-car speed (X0 / X1 / X2 `.msd`, `msd_dead_decay.py`), per killed slot 1 and 2:
- PASS when, with the knob ON, |v| reaches exactly 0.0 within 120 frames of the kill AND frozen-at-non-zero frames are
  under 5% of dead frames AND frames passing the FX speed gate are under 5% of dead frames.
  Original reference: 27 frames, 0%, 4 / 3,879 (0.1%). X0 (OFF) must reproduce the frozen state (the control: frozen
  non-zero over 90% of dead frames). If X0 does not, the measure is void.

M2, FX rates (fx_debug, `verify/fxgates_20261026/fx_score.py` / `fx_inputs_score.py`), emits per 1000 active frames
on all frames, slots 1 / 2 / 3, against the original's 166.1 / 215.3 / 191.0:
- prediction: slot 1 and slot 2 active frames fall from ~14,200 to under 1,000, and the all-frames rate moves to
  the alive-frames rate of attempt 1 (157.4 / 59.1 / 215.5). Slot 2 stays outside 2x (the separate brake-byte lead,
  attempt 1 section 3), so M2 is reported per slot, not as one verdict.
- PASS per slot when the rate is within 2x of the original's.

M3, gates: G must equal `re/orchestrator/merge_train.json` `gctrl.ref` / `b.ref` (5943c5f1 / b493262b / 09551e6a,
3a03670c / e85ea6fb / 53778664 at the time of writing). If not, the knob-OFF path is broken: RED, fix before
anything else. GON hashes are reported. If they differ from the refs, the flip is a DECISION row with the GON
hashes as the expected new refs. No default is flipped here.

Verdict: GREEN when M1 passes on both slots, M3 G passes, and M2 slot 1 passes. M2 slot 2 failing alone does not
block GREEN (pre-registered above as expected). Anything else RED, with the measured numbers.
