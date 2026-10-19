# PREREG u9765: car 1's body matrix at call 4 (2026-10-19)

Branch `work/u9765` from `work/u9764` `a9169954e` (default G-CTRL `3b3690f6 / 9a657924 / a386f43b`, B dumps
`6efb49ed / 3e0724ac / 395fb8b7`). Model claude-opus-5-5, account2. Exe `MASHED_NO_DEPLOY=1`, private root
`D:\MashedWT\u9756_root`. Written before any knob arm.

## 0. Measured before this PREREG

- U-9765: on `MASHED_ROTATE_LUT=1` (`B_XW1RL_K1`) car 1's record equals o_wobj2 through call 3 outside read-time fields; at
  call 4 the original's `up.y` (`+0x93c`) is `1.00000024`, the port's exactly 1; `at.x / at.z` (`+0x948 / +0x950`) 1 ulp off.
- Write watch o_w93c (`--watch-write 93c,948`, car 1; matches o_wobj2 like o_w2d8, only the torque-ring history shifts). Two
  writers per substep:
  - `0x0046ee90..0x0046ef5b` (in `FUN_0046e9e0`, from `0x00470948`): `dst row = (omega x src row) + src row`, no normalize.
  - `0x004c4680` (`thunk 0x004c4670`, called at `0x0046ef5e` with `(ebx, ebx)`): the re-orthonormalize; its copy-out stores
    `+0x93c` at `0x004c49fd` and `+0x948` at `0x004c4a0e`.
  `up.y` grows on every write (`1`, `1.00000012` after the step, `1.00000024` after the ortho, `1.00000048`, ...), so the
  original's ortho does not return unit rows.
- `0x004c4680` (disassembly `0x004c4680..0x004c4a41`): each row scaled by `0x004c3b90` (LUT inverse sqrt) of
  `(y*y + x*x) + z*z`; a row whose inverse is `<= [0x005d757c] = 0.0` picks the pair directly (`0x004c47c9 / 0x004c47e0 /
  0x004c47f3`); else `|a.u|`, `|a.r|`, `|u.r|` pick `(ebx, esi, edi)` (`0x004c4898..0x004c48ef`); `edi = ebx x esi`, normalized
  through `0x004c3b90`; `esi = edi x ebx`, normalized; ebx is not renormalized; translation copied; flags
  `(flags & 0xfffdffff) | 3`.
- The port's live path is `VehiclePhysicsRun.cpp:1799` `BodyOrient_IntegrateStep` -> `MatrixOrthoNormalize`
  (`Vehicle/BodyOrientationIntegrate.cpp:120`): `1/std::sqrt` with a `1e-20` guard, and a different order on the tie
  `|a.u| == |a.r| < |u.r|`. `Track/WarzonePontoon.cpp:98` already has a transcription of `0x004c4680` with `LutInvSqrt`.

## 1. Hypothesis, knob, arms, gates

H1: with the body-matrix ortho in `0x004c4680`'s form (LUT inverse sqrt, its selection order, its 0.0 checks), car 1's
record equals o_wobj2 at call 4. Knob `MASHED_ORTHO_LUT=1` (default OFF). Tested on top of `MASHED_ROTATE_LUT=1` (`XW1RLO`) and
alone (`B_OL_K*`, `XW1OL`).

Arms: `B_DEF_K0/K1/K26` (G-NOOP); `B_XW1RLO_K1`, `B_XW1OL_K1` (split + record); `B_OL_K0/K1/K26`, `B_RLO_K0/K1/K26`; then D2 /
13 tracks / G-CTRL only for a default proposal.

| gate | rule |
|---|---|
| G-NOOP | knob off: B dumps `6efb49ed / 3e0724ac / 395fb8b7` |
| G-REC | `XW1RLO`: the first call where car 1's record differs from o_wobj2 outside read-time fields (`+0x9d4 / +0x9dc`, the torque-ring history `+0x1ac..+0x1e8 / +0x270..+0x2ac`, pad dwords), now 4 |
| G-POS | `recpos.py` against o_wobj2: first call where position / velocity differ (now 7 / 5) |
| G-B | (b) on `OL` and `RLO`: no car that passes on the default fails |
| G-D2 / G-13 | for a default proposal: D2 3/3 on the d2rebase gate, 13 tracks 0 RED, G-CTRL reported |
