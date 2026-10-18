# RESULT c3k44: regime-B car 3 (U-9753) (2026-10-17)

Branch `work/c3k44` from `race/first-frame-parity` `438566c88`. Model claude-opus-5-5, account2. PREREG `PREREG_c3k44.md`
(`782107f4a`). Exe `b_262d7ac2` (the base, no code change). Reference: `verify/d3_regimes_20261013/o_reg_r13.msd.aistep.csv`.

## 0. Verdict

- **U-9753 was a probe artifact.** From window call 44 the original's probe records no lookahead target for car 3, because
  `FUN_00414a70` returns 2 there and `FUN_00416250` takes its immediate return (`0x004163f6..0x00416405`). The replay compared
  the port's target against an empty field. Resolved.
- **Regime-B car 3's real divergence is the missing `FUN_00414a70` brake chain (U-9684).** On o_reg_r13 car 3 brakes in mode 3
  on calls 39..41 (steer 178 / 227 / 255, accel 0, brake 255) and holds full brake on 44..57 (return 2, speed 2710 -> 1734).
  The default never calls `FUN_00414a70`, so car 3 keeps accelerating (2830 -> 3049) and fails `c1#` 52 [17, 46].
- **The ported chain does not fix it on today's default**, and no default change is proposed:
  - `MASHED_PATHPROG` + `MASHED_TGT3` (TGT): the car ahead of car 3 is car 2 until call 52 (car 2 runs 0.34 ahead in z under
    these knobs), so return 2 starts at 53 and mode 3 never fires. (b) A 1/3, B 0/3, C 0/3.
  - d3bB2's `S` stack: car 3 brakes one call late (mode 3 on 40..41, return 2 from 45), position within 0.003 up to call 43. At
    call 39 car 2 is 0.0007 nodes ahead of car 3 on `0x008a96e8`, so the car-ahead choice is knife-edge exactly where the
    original fires. (b) 0/3 in every regime.

## 1. Evidence

| item | measured |
|---|---|
| original car 3 (`o_reg_r13`), `ret14a70` / mode / c4 / c5 | 1 / 3 on 39..41; 2 / 0 / 0 / 255 on 44..57; look empty on 44..57 |
| default (`B_DEF_K1`) | `ret14a70` not recorded (not called); c4 255 throughout; speed 2830 -> 3049 on 44..57 |
| TGT (`B_TGT_K1`, `B_TGTT_K1.tgt3.csv`) | return 0 to call 52, 2 from 53; car 2's progress just above car 3's until 52 |
| S (`B_ST_K1`, `B_SR3_K1.rec.msd`) | return 1 on 40..41, 2 from 45; record position keeps moving (z -3.354, -3.478, ...), the step dump's own_x/own_z repeat the call-43 value on return-2 calls (stale `FUN_00416250` locals) |
| S without the contact event (`B_SmE_K1`) | car 3 identical to S |
| G-NOOP | `B_DEF_K0/K1/K26` = the current default's dumps (`f070c4c9 / d78355d9 / 5fe532c5`) |

D3 (b) (`arms/SCORE_B.txt` and the runs above): DEF A 2/3 B 2/3 C 0/3; TGT A 1/3 B 0/3 C 0/3 (TGTRG identical); S 0/3 x3.

## 2. Tracker transaction (re-classify, no C-level change; `apply_trackers.py`)

U-9753 RESOLVED (probe artifact); U-9684 note (regime-B car 3's brake chain, TGT / S results); one CHANGELOG line.

## 3. Follow-ups

- The chain needs car 2 right first (the car-ahead choice for car 3 depends on car 2's progress to 1e-3): U-9747 (AI progress at
  GO under tick-first) and the contact-event freezes (U-9745 / U-9185 / U-9186) are what blocked the full `S` stack in d3bB2.
- The step dump should re-read own position on return-2 calls (measurement only).
