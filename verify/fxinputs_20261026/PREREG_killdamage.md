# PREREG D4-FX-INPUTS attempt 3: `MASHED_KILL_DAMAGE` (the kill's 50.0 damage reaches the record) (2026-10-27)

Committed alone, before any arm of this knob runs. Follows attempt 2 (`PREREG_deadcoast.md`, RESULT section 3: port
killed-car decel 10.2 per frame against the original's 5.7, [UNCERTAIN] cause).

## 0. The cause (measured before this PREREG, no new game run)

Existing captures only (`D:/MashedWT/D4-FX-INPUTS_data`, SHA-256 in the RESULT), decomp via `re/tools/decomp_pc.py`
(pool slot 10, read-only).

1. Matched band. `decel_band.py` (this folder): per-frame decel of the killed car for |v| 25..70.
   - original ww3_c2: n=8, median 5.1184 (5.1062..5.1322), wheel+0x54 = 0.125 on all four wheels, rec+0x2c = 50, +0x30 = 50.
   - original o60_c2: n=8, median 5.4170 (5.0804..6.8009), the same wheel and record values.
   - port H_X1: n=4, median 10.2172, wheel+0x54 = 0.25, rec+0x2c = 0, +0x30 = 0. H_X2: median 10.2208, same values.
   The ratio is 2.0 at matched speed, so the difference is not the kill speed.
2. The friction term. A6a `FUN_00467650` (original code, called by the port at 0x00470936) builds the per-wheel
   friction as `piVar12[0x15] * piVar12[0x1b] * _DAT_0088e5f0 * local_e4 * _DAT_005cea0c` (piVar12 = rec+0x1a4 +
   w*0xc4, so piVar12[0x15] = wheel+0x54 = rec+0x1f8 + w*0xc4). A5 `FUN_0046ddb0` (original code, called at 0x00470923)
   writes it: `*pfVar14 = 0.25` (pfVar14 = rec+0x1f4 + w*0xc4), then `if (+0x2c != 0) *pfVar14 = +0x2c * *pfVar14 *
   _DAT_005cc328`, then `pfVar14[1] = _DAT_005cc318 * v` and `if (rec+0x10 == 1) pfVar14[1] = v`. Live cars read 0.15
   on both sides. A dead car reads 0.25 with +0x2c == 0 and 0.125 with +0x2c == 50, which is 0.25 * 50 * 0.01.
3. The writer of rec+0x2c. In the image the only displacement 0x008815cc (rec base 0x008815a0 + 0x2c) is at 0x0046bee0,
   case 4 of the setter `FUN_0046be50(v, case, value, paired)` (value capped at 100; case 4 = +0x2c / +0x44, case 5 =
   +0x30 / +0x48). Its E8 callers: 0x00420e33 (case 5) and 0x00420f40 (case 4), both in `FUN_00420e00`, called from
   `FUN_004215a0` (the 4-entry loop over the table at 0x0063e4b8, stride 0x24). `FUN_00420e00` writes
   `+0x30 = ftol(t[0])`, `+0x2c = ftol(t[1])`. The kill `FUN_00422fd0` calls `FUN_004215c0(v, 50.0f, 0)` and
   `FUN_004215c0(v, 50.0f, 1)` (0x42480000), which go to `FUN_00420de0`: `t[k] = min(t[k] + 50.0, 50.0)`.
   Per frame (.msd): rec+0x2c and +0x30 go 0 -> 50 on the kill frame on both original captures (1726, 1802), and stay 50.
4. The port runs neither `FUN_004215a0` nor `FUN_00420e00` (hooks.csv rows 004215a0 / 00420e00 at C2, no port caller).
   Its rec+0x2c / +0x30 stay 0 for the whole capture.

Also seen, NOT changed here: the original's rec+0x44 / +0x48 read 0xbf800000 from frame 2 on (the paired value of the
same setter calls, alive and dead). The port's read 0. [UNCERTAIN] which reader consumes them. That is a live-frame
difference, so any port of it moves default hashes and needs its own PREREG.

## 1. The knob

`MASHED_KILL_DAMAGE=1`, default OFF, effective only together with `MASHED_DEAD_COAST=1` (it lives in
`VehiclePhysics_MarkKilled`). With it unset, the exe behaves exactly as before.

1. `MarkKilled(v)` also writes rec+0x2c = 50 and rec+0x30 = 50 (int), the values `FUN_00420e00` copies after the kill's
   two `FUN_004215c0(v, 50.0f, k)` calls. The port has no table at 0x0063e4b8 and no `FUN_00420e00`. This writes the
   steady-state result once, at the kill. Not ported: the smoke / particle side of `FUN_00420e00` and the +0x44 / +0x48
   paired words.
2. `ClearKilled(v)` (StartRound) also writes rec+0x2c = 0 and +0x30 = 0. [UNCERTAIN] the original's reset path
   (`FUN_00420d00` clears the table) is not measured on a second round. No scored arm reaches it.

## 2. Arms

Exe: this branch's build, `MASHED_NO_DEPLOY=1`, private root `D:\MashedWT\D4-FX-INPUTS_root`, every run inside
`mashed_lock.py run --label D4-FX-INPUTS`. Runner: this folder's `run_arms.py`.

- **G (gates, both knobs OFF):** `H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`.
- **GON (gates, DEAD_COAST + KILL_DAMAGE ON, report only):** stack `DCKD`, the same four arms.
- **Y1 / Y2 (FX + record, G-CTRL recipe):** attempt 2's X1 / X2 env plus `MASHED_KILL_DAMAGE=1`. Y1 = slot 1, Y2 = slot 2.

## 3. Measures and pass rules

M1, matched-band decel (Y1 / Y2 .msd, `decel_band.py`, band 25..70):
- PASS per slot when the median per-frame decel is within [4.86, 5.69] (the original's two medians, 5.1184 - 5% and
  5.4170 + 5%) AND wheel+0x54 reads 0.125 on all four wheels at the first band frame.
- Prediction: 10.22 / 2 = 5.11.
- Control: X1 / X2 of attempt 2 (DEAD_COAST ON, KILL_DAMAGE absent, exe e7cdf357) gave 10.22. They are not re-run.
  The G arm guards the OFF path.

M1b, reported, not gated: frames to |v| == 0.0, frozen-nonzero and FX speed-gate frames (`msd_dead_decay.py`). The
attempt 2 limit of 120 frames to stop is withdrawn as a gate. It compared a kill at |v| 2015 / 2508 against kills at
175 / 298. M1 replaces it with a matched-speed measure. Prediction: the stop takes longer than attempt 2's 133 / 149
frames, because the decel per frame halves.

M2, FX rates per 1000 active frames, all frames, slots 1 / 2 / 3 (`fx_inputs_score.py`), against 166.1 / 215.3 / 191.0.
Reported per slot, PASS per slot when within 2x. Slot 2 is expected to stay outside 2x (brake-byte lead, open).

M3, gates: G must equal `re/orchestrator/merge_train.json` `gctrl.ref` / `b.ref` (5943c5f1 / b493262b / 09551e6a,
3a03670c / e85ea6fb / 53778664 at the time of writing). If not: RED, fix first. GON hashes are reported. A flip of
either knob is a DECISION row with the GON hashes as the expected new refs. No default is flipped here.

Verdict: GREEN when M1 passes on both slots and M3 G passes. M2 is reported. Anything else RED, with the numbers.
