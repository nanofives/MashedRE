# PREREG u9762: car 1's `+0x460` at call 2 (2026-10-19)

Branch `work/u9762` from `race/first-frame-parity` `57afde846` (default G-CTRL `726b8fab / b048d6f0 / ecc20282`, B dumps
`85561724 / 76ef3eab / a000895f`). Model claude-opus-5-5, account2. Exe `6ca0f4e6` (`MASHED_NO_DEPLOY=1`), private root
`D:\MashedWT\u9756_root`. Written before the gate arms below (the diagnostic arms in section 0 ran first, as in u9756/u9760).

## 0. Measured before this PREREG

- Write watch `o_w460` (car 1, `+0x460 / +0x464`): `+0x460` is wheel 3's force x (`0x214 + 3*0xc4`), written by A5's
  all-grounded block (`pc 0x0046e91a`) and A6a's addend (`pc 0x0046832a`), as wheel 0's in u9760.
- Diagnostic `MASHED_FORCE_SPLIT_WHEEL` (new, default 0; `B_XW3_K1` step dump `7f7fd0f8` = u9761's `XA6F32FS`): on the u9761
  pair stack, wheel 3 at call 2: port A5 output `-0.248552054`, original `-0.24855206906795502`; A6a's addend equal
  (`0.198584929`). With `MASHED_A5_SUSP_LUT` (u9760, A5's original arithmetic) added (`B_XW3A5_K1`): A5 output
  `-0.248552069` = original, and car 1's record equals o_wobj2 through call 2 on every physics field.
- New `recpos.py` (record position / velocity at full float precision): with the three knobs (`MASHED_A6A_LUT`,
  `MASHED_A6A_F32`, `MASHED_A5_SUSP_LUT`) the position and velocity equal o_wobj2 through call 4; velocity differs from call 5
  (dvz 1.2e-4), position from call 7. First differing field: wheel 1's force x `+0x2d8` at call 3 (-28.2014942 /
  -28.2014961). `posdrift.py` reads the AI's `own_x / own_z` (the previous frame's position) from a 9-digit CSV, so its early
  gaps are the pre-window pose (call 0) and print quantisation.

## 1. Candidate and gates

Candidate: the triple `MASHED_A6A_LUT` + `MASHED_A6A_F32` + `MASHED_A5_SUSP_LUT` (all default OFF). Arms: `B_DEF_K0/K1/K26`,
`H_DEF` (G-NOOP); `B_TRI_K0/K1/K26`; `D2_TRI_1..3` (each alone); 13 tracks with the three knobs; `H_TRI`.

| gate | rule |
|---|---|
| G-NOOP | knobs off: B dumps and `H_DEF` as above |
| G-B | (b) on `TRI`: no car that passes on the default fails |
| G-D2 | slip1500 / slip2000 / driving-median in the d81a8df6 bands, H1 >= 398/400 |
| G-13 | 0 RED, changes cited |
