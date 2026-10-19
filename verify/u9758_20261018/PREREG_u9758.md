# PREREG u9758: the world forward +0x9d4 (2026-10-18)

Branch `work/u9758` from `race/first-frame-parity` `bf451f914` (default G-CTRL `b7534619 / fb690fb1 / d3a20f4c`, default B
dumps `c7b40cf2 / 5b9905d8 / f680a529`). Model claude-opus-5-5, account2. Exe `26ee6117` (`MASHED_NO_DEPLOY=1`), private root
`D:\MashedWT\u9756_root` (reused). Written before any knob arm.

## 0. Measured before this PREREG

- New original capture `o_fw1` (`scenario_launch.py --track 0 --mode 10 --cars 4 --car 0 --poke-ctrl-slots --statediff-car 1
  --watch-write 9d4,9dc --hold 10`, 72 frames, 845 hits). Every write of car 1's `+0x9d4` / `+0x9dc` is at `pc 0x004dc481` /
  `0x004dc492` with `ret0 0x004c3e14`, i.e. inside `FUN_004c3df0`. Callers: A5 (`ret1 0x0046ddce`, `ret2 0x00470948`, once per
  frame, 69 + 1 from `0x0040956d`) and `FUN_0046d510` (`ret1 0x0046d53f`, from `0x00414605 / 0x00414aad / 0x00415f26 /
  0x00425374 / 0x0047ed3a`, 70-71 each). First value `+0x9d4` -8.742277657347586e-08, `+0x9dc` -1.
- Disassembly: A5 `0x0046ddb0..0x0046ddc9`: `FUN_004c3df0(edi + 0x9d4, 0x00614708, 1, [esp+8] = arg 2)`; A4 passes arg 2 =
  `rec + [+0x9a8]*0x40 + 0x928` (`0x0047068e..`). `FUN_0046d510` `0x0046d51e..0x0046d53a`: `FUN_004c3df0(rec + 0x9d4,
  0x00614708, 1, rec + 0x928)`. `DAT_00614708` = (0, 0, 1) (`.data`, raw bytes `00000000 00000000 0000803f`). So the
  forward is the matrix's `at` row. Neither caller runs inside `FUN_004709a0`'s substep loop.
- Port: `(cos yaw, 0, sin yaw)` at frame entry (`VehiclePhysicsRun.cpp` StepCar, the re-seed block), after the heading is
  taken from the basis, and after every substep's orientation step. U-9758: at call 0 the original holds -8.74227766e-08,
  the port -4.37113883e-08.

## 1. Hypothesis

H1: with the forward = the basis `at` row, written at frame entry / frame start and not in the substep, car 1's `+0x9d4` at
window call 0 equals o_wobj2 and the call-1 velocity `+0x9b0` / omega `+0x9c0` gap closes; the call-21 gap (1.202e-4) drops.

## 2. Knob and arms

- `MASHED_FWD_AT=1` (default OFF): the three writes above become `0*right + 0*up + 1*at` of the body basis (all three
  components); the substep write is dropped.
Arms: `B_DEF_K0/K1/K26`, `H_DEF` (G-NOOP); `B_SPLSBKNF60FAR1_K1`, `B_SPLSBKNF60UTFAR1_K1` (car-1 record);
`B_FA_K0/K1/K26` (default + knob); `H_FA`.

## 3. Gates

| gate | rule |
|---|---|
| G-NOOP | knob off: B dumps and `H_DEF` as above |
| G-FWD | `SPLSBKNF60FAR1` car 1 `+0x9d4..+0x9dc` at call 0 = o_wobj2, `recdiff1.py` |
| G-DRIFT | car 1 x/z gap at call 21 < 1e-5 (now 1.202e-4); else the next first-differing field |
| G-B | (b) on `FA`: no car that passes on the default turns to FAIL |
| G-D2 / G-13 | for a default proposal: D2 3/3, 13 tracks 0 RED, G-CTRL reported |

## Amendment 1 (2026-10-18, after the B arms, before these)

Measured: G-NOOP PASS (`c7b40cf2 / 5b9905d8 / f680a529`). G-FWD PASS (`+0x9d4..+0x9dc` at call 0 = o_wobj2). G-DRIFT FAIL
(call 21 1.202e-4 unchanged; call-1 `+0x9b0` -0.00108456612 / -0.00108465552 unchanged). G-B PASS (`FA`: A 2/3, B 2/3, C 0/3,
car 3 B c1_distinct 52 -> 51). At calls >= 1 the port's dumped `+0x9d4` equals the original's one call earlier: the original
also writes it after physics, from the frame driver `0x00425a99` `FUN_004252c0` -> `0x0042536f` `FUN_0046d510` (gated at
`0x00425360`), and before physics from the AI callers and A5, all from slot 0, so physics reads the frame-start pose's `at`
row in both. Default candidate `FA`: D2 `D2_FA_1..3`, 13 tracks (`verify/rsky_20261016/run13.py` with `MASHED_FWD_AT=1`,
scored with `verify/rres_20261016/cites_rres.json`), `H_FA`.

## Amendment 2 (2026-10-18, rebased onto `ffb209b32`, before these)

The f60flip merged (`MASHED_FIX_5CEA60` default ON; default G-CTRL `b40b47bc / fb690fb1 / d3a20f4c`, B dumps unchanged).
`work/u9758` rebased onto `ffb209b32`, exe `fa9b48ff`. The amendment-1 numbers were measured on `bf451f914` (evidence in
`D:/MashedWT/u9758_data/verify/arms_bf451`). All gates re-run on the rebased exe with the same rules: G-NOOP (`B_DEF_*`
unchanged, `H_DEF` = `b40b47bc / fb690fb1 / d3a20f4c`), G-FWD / G-DRIFT (`B_SPLSBKNF60FAR1_K1`), G-B (`B_FA_*`), D2
(`D2_FA_1..3`), 13 tracks, `H_FA`. `MASHED_FWD_AT` stays default OFF (the flip is the user's decision).
