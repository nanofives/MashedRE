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

## 2. Amendment 1 (2026-10-19, before any MASHED_ROTATE_LUT arm)

Measured after the partial RESULT (`8eb8c0fe0`), which this amendment corrects:

- The `0x00468320` probe perturbed the run. `Interceptor.attach` mid-function swaps `[esp]`, a live A6a local (memory
  frida-interceptor-is-entry-only). o_aq1's car 1 tilts at call 1 (`+0x92c` `2.66e-08`) where o_wobj2 and the watch-only
  o_w2d8 hold 0. Its `la8` result is an artifact and the "axis y" reading in RESULT section 0.2-0.3 / U-9764 is withdrawn.
- New probe: entry hook on the LUT length `0x004c3ac0` filtered on return address `0x00468343` and argument `rec + 0x268 +
  0x70` (wheel 1's force). Its run o_aq4 matches o_wobj2 like the watch-only control (only the history block
  `+0x1d4..+0x1e8 / +0x298..+0x2ac` shifts). Against the port's split log (`B_XW1_K1`, step `066864b0`) every block-#4 term
  matches through call 2. At call 3 the first differing input is `le0`: original `0.0337665901` (`3d0a4ed6`), port
  `0.0337664969`. `ld8`, `le4`, `ldc`, the axis, and the record's `+0x9bc..+0x9c4`, `+0x9e4`, `+0x9e8`, `+0x9b0..` are equal.
- `le0` is the spin branch: `RwMatrixRotate(m, +0x9bc, 270, 0)` (`0x0046805a` -> `0x004c4d20`), the vectors transform of the
  wheel offset (`0x00468072`), then `dst0 * f - +0x9b0` with `f = (+0x9e8 * 0.019904457) * max(+0x9e4 * 0.008, 8) * 360`
  (`0x00468077..0x004680ce`, constants `0x005cea18 / 0x005cea14 / 0x005cc9f4 / 0x005ccac4`, equal to the port's). The port's
  `dst0 * f` is larger by a factor `1.000026`, and the LUT length `+0x9e8` (`5.62020905e-05`) over the exact `|av|`
  (`5.62006207e-05`) is `1.0000262`.
- `0x004c4d20` normalizes the axis with `call 0x004c3b90` (`0x004c4d5d`), the LUT inverse sqrt. The port's
  `Math/RwMatrixRotate.cpp` calls `FastInvSqrt`, which in the standalone has no RW LUT and returns `1/sqrt(x)`
  (`Math/RwSqrt.cpp`, the CPU fallback).

H2: with `RwMatrixRotate` normalizing through `LutInvSqrt` (`Math/RwLutTables.h`, the original's table), car 1's call-3 `le0`,
the addend and `+0x2d8` equal the original's. Knob `MASHED_ROTATE_LUT=1` (default OFF). It reaches every `RwMatrixRotate`
caller (A5 Phase 0, A6a spin, A6b, camera, renderer, aero), all of which call `0x004c3b90` in the original.

Arms: `B_DEF_K0/K1/K26` (G-NOOP, knob off); `B_XW1RL_K1` (split + record); `B_RL_K0/K1/K26`; then D2 / 13 tracks / G-CTRL
only for a default proposal.

| gate | rule |
|---|---|
| G-NOOP | knob off: B dumps `6efb49ed / 3e0724ac / 395fb8b7` |
| G-LE0 | `XW1RL` call 3 `le0` = `0.0337665901`, addend x = `30.5746326`, `+0x2d8` = `-28.2014942` (o_aq4) |
| G-POS | `recpos.py` against o_wobj2: report the first call where position / velocity differ (now 7 / 5) |
| G-B | (b) on `RL`: no car that passes on the default fails |
| G-D2 / G-13 | for a default proposal: D2 3/3 on the d2rebase gate, 13 tracks 0 RED, G-CTRL reported |
