# PREREG u9766: car 1's torque `+0x9c0` at call 5 (2026-10-19)

Branch `work/u9766` from `work/u9765` `27ee18b78`. Model claude-opus-5-5, account2. Exe `MASHED_NO_DEPLOY=1`, private root
`D:\MashedWT\u9756_root`. Written before any new knob arm.

## 0. Measured before this PREREG

- Write watch o_w9c0 (`--watch-write 9c0`, car 1; matches o_wobj2 outside the torque-ring history): per frame A6a's tail
  writes `+0x9c0` at `0x004685a2`, then A6b at `0x0046892b` (later `0x0046887c`) writes half of it (call 5: `-0.00179728237`
  then `-0.000898641185`). A watch on `9c0,9e8` (first try) froze car 1 (664 frames, 4 distinct payloads), moved to
  `D:/MashedWT/u9766_data/bad`.
- On `RLO` (`MASHED_ROTATE_LUT` + `MASHED_ORTHO_LUT`) `recfirst.py` shows wheel 0's force z `+0x21c` differing first, at call 5.
- A6a probe on wheel 0 (o_aq0, matches o_wobj2) against the port's wheel-0 split (`B_XW0RLO_K1`): every block-#4 term equal
  through call 4; at call 5 `lc8` (`0.00821873918` / `0.00821873825`) and `lc0'` (`0.999960005` / `0.999959946`) differ while
  `s0`, `s1` and the axis are equal, so the shared factor `lc0` differs.
- The port's default `lc0` is `(+0x9dc * inv) * ld8 + +0x9d4 * s0 + +0x9d8 * s1`; the original's
  (`0x0046814b`, `0x0046819f..0x004681c3`) is `(+0x9d8 * s1 + +0x9d4 * s0) + +0x9dc * (inv * ld8)`, which `MASHED_A6A_LC0=1`
  (u9764, default OFF) already implements.

## 1. Hypothesis, arms, gates

H1: with `MASHED_A6A_LC0=1` on top of `RLO`, wheel 0's call-5 `lc0` terms, `+0x21c` and `+0x9c0` equal the original's. No new
knob. Arms: `B_DEF_K0/K1/K26` (G-NOOP); `B_XW0RLOLC_K1` (wheel-0 split + record); `B_RLOLC_K0/K1/K26`; D2 / 13 tracks / G-CTRL
only for a default proposal.

| gate | rule |
|---|---|
| G-NOOP | `B_DEF_K0/K1/K26` `6efb49ed / 3e0724ac / 395fb8b7` |
| G-LC0 | `XW0RLOLC` call 5: `lc8` / `lc0'` / `Fz` equal o_aq0 |
| G-REC | `firstdiff.py` / `recfirst.py` against o_wobj2: first differing call per field, now `+0x21c` / `+0x9c0` at 5 |
| G-POS | `recpos.py`: now velocity from call 21, position from 34 |
| G-B | (b) on `RLOLC`: report against the default (A 2/3, B 3/3, C 0/3) and `RLO` (A 2/3, B 2/3, C 0/3) |
