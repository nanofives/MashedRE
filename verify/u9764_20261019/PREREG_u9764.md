# PREREG u9764: car 1's wheel 1 force x `+0x2d8` at call 3 (2026-10-19)

Branch `work/u9764` from `race/first-frame-parity` `67cdd752f` (default G-CTRL `3b3690f6 / 9a657924 / a386f43b`, B dumps
`6efb49ed / 3e0724ac / 395fb8b7`). Model claude-opus-5-5, account2. Exe `7cb526eb` (`MASHED_NO_DEPLOY=1`), private root
`D:\MashedWT\u9756_root`. Written before any knob arm.

## 0. Measured before this PREREG

- Write watch `o_w2d8` (car 1, `+0x2d8 / +0x2e0`): at call 3 A5 projects `x -58.776126861572266`, A6a's addend brings it to
  `-28.201494216918945`, so the original's addend x is `30.5746327`.
- Split log (`B_XW1_K1`, wheel 1, step dump `066864b0` = u9762's `XW3A5`): the port's A5 output `-58.7761269` equals the
  original's; its addend x is `30.5746307` (post `-28.2014961`); z matches. Calls 1-2 equal on both.
- A6a block #4 (`0x00468127..0x00468337`) against `Vehicle/Integrate2.cpp` (~line 470): every product and sum matches the
  original's order except `lc0`. The original keeps `s2 = inv*ld8` on the stack (`0x0046814b`) and sums
  `(+0x9d8*s1 + +0x9d4*s0) + +0x9dc*s2` (`0x0046819f..0x004681c3`); the port computes `(+0x9dc*inv)*ld8 + +0x9d4*s0 + +0x9d8*s1`.

## 1. Hypothesis, knob, arms, gates

H1: with `lc0` in the original's order, the call-3 addend and `+0x2d8` equal the original's. Knob `MASHED_A6A_LC0=1` (default
OFF). Arms: `B_DEF_K0/K1/K26`, `H_DEF` (G-NOOP); `B_XW1LC_K1` (split + record); `B_LC_K0/K1/K26`; then D2 / 13 tracks /
G-CTRL only for a default proposal.

| gate | rule |
|---|---|
| G-NOOP | knob off: B dumps and `H_DEF` as above |
| G-ADD | `XW1LC` wheel 1 addend at call 3 = 30.5746327, `+0x2d8` = o_wobj2 |
| G-POS | `recpos.py`: report the first call where position / velocity differ (now 7 / 5) |
| G-B | (b) on `LC`: no car that passes on the default fails |
| G-D2 / G-13 | for a default proposal: D2 3/3 on the d2rebase gate, 13 tracks 0 RED, G-CTRL reported |
