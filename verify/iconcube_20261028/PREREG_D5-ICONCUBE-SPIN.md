# PREREG D5-ICONCUBE-SPIN (D-11076 cube half, L-45)

Ledger step D5-ICONCUBE-SPIN, branch `orch/D5-ICONCUBE-SPIN` from `orch/integration` @ `d92bb3a5e`.
Scope: the pickup IconCube's spin and skin in the standalone. The PUGlow glow is D5-ICONCUBE-GLOW
and is out of scope. No new original-side capture (the original does not boot: no audio endpoint).

## 1. What the original does (static, cited)

Decomp from `re/tools/decomp_pc.py` (Mashed_pool0), constants read from `MASHED.exe.unpatched`.

- **Init** `FUN_004587a0` @ `0x004587a0`: `PowerUpIcons.txd` into `DAT_0068b9ac`, `FUN_004c5c80(0)`
  (no current dictionary, so the DFF streams untextured), `IconCube.dff` read once, then per pool entry
  (25, `0x0068b198 + i*0x50`): `+0x00` = a new clump from the stream (`FUN_004b3e40`), `+0x08` =
  `0x3f800000` (1.0), `+0x0c/+0x10/+0x14` = 0 / `0x3f800000` / 0. `+0x04` is not written.
- **Skin** `FUN_00458dd0` @ `0x00458dd0`: `FUN_004e8090(FUN_004b4080(entry+0x00), FUN_00458630(entry+0x24))`.
  `FUN_00458630` @ `0x00458630` looks the type up in `DAT_0068b9ac` (`FUN_004c5c00`): 2 and 0x10
  `flamethrower`, 6 / 0xc / 0xd `mine` (`0x005ce594`), 7 `mortar`, 9 `machinegun`, 10 `depthcharge`,
  0xb `missile`, 0x11 `shotgun`, 0x12 `flare`, 0x13 `oil` (`0x005ce4fc`), 0x15 `chaos`, 0x16 `airstrike`,
  any other type 0 (no texture). The skin is always a function of the entry's current `+0x24`.
- **Pose** `FUN_00458a40` @ `0x00458a40` (already verbatim in `Gameplay/PickupTick_naked.inc`, default ON):
  `+0x38 += +0x44` (x/y/z), velocity `*= 0.95` (`0x005cc9dc`), `RwFrameTranslate(frame, +0x38, 0)`
  (`0x00458a8e`, REPLACE), `+0x04 += +0x08` (`0x00458a93..0x00458aa6`), `RwFrameRotate(frame, +0x0c,
  +0x04, 1)` (`0x00458aac`, PRECONCAT). So the frame is `Rot(+0x0c, +0x04 deg)` with translation `+0x38`.
  The port's `PK_FrameRotate` counts the call and does not model the rows (no sim reader).
- **Draw** `FUN_00458b10` @ `0x00458b10` (from `FUN_0045b990` @ `0x0045b990`): RwRenderStateSet(8,0)
  (6,1), the glow flush `FUN_004770a0(&DAT_0068b968)`, then (8,1) (6,1), so the cubes draw with ZWRITE 1 /
  ZTEST 1. Loop i = count-1 down to 0 (count `DAT_0068b9a8`): `+0x20 == 1`, `d = +0x38 - campos`
  (camera frame +0x40), `dot(d, cam at (+0x30)) > [0x005d757c]` (0.0) and `|d|^2 < [0x005ce5d4]`
  (800.0), then `RpClumpRender` (`FUN_004e6680`) on `+0x00`.

## 2. Change (default OFF)

- Knob `MASHED_ICONCUBE` (default OFF; needs the pool tick, `MASHED_PICKUPS_ORIG`, which is default ON).
- New exe-only TU `D3d9Render/PickupIconCube.{h,cpp}`: at pickup init it writes FUN_004587a0's
  `+0x08` / `+0x0c..+0x14` on the 25 entries and registers `ICONCUBE.DFF` (Common/POWERUPS/Powerups.piz)
  once per `FUN_00458630` texture name from `POWERUPICONS.TXD` plus one untextured copy. Per frame it
  runs FUN_00458b10's loop on the pool and queues one librw instance per kept entry, pose built from
  `+0x0c` / `+0x04` / `+0x38` with the port's `RwMatrixRotate` (`0x004c4d20`).
- `LibRw/RwRaceSubmit`: new queue kind `kInstAfterFx`, drawn after `RaceFx_DrawWorldFx`
  (the original's cube draws follow RWObjShad / smoke / exp_cloud2 / PUGlow in the d1r1 records).
  With no such instance queued the draw sequence is unchanged.
- It reads pool state only. It makes no RNG call. Its only pool writes are FUN_004587a0's
  `+0x08..+0x14`, which feed `+0x04` (read at `0x0045920d` into a sprite no-op) and `PK_FrameRotate`
  (counted only).

## 3. Reference

`verify/d1r1_20261016/evidence/training_orig_row_{00884,00924,00959}.draws.json` (original, Training,
existing capture) and the trace `log/d1r1/orig_training/trace.csv`. Cube rows there: 12 tri, `rs`
`[1,1,4,0,0,5,0,5,6,2,0,0]`, textures mine / mortar / oil, world matrix a pure rotation about +Y.

## 4. Gates and predictions

- **G-OFF** knob OFF: G-CTRL and B hashes equal `merge_train.json` refs
  (G-CTRL `5943c5f1/364350ae/52052890`, B `3a03670c/9fcdbd3a/53778664`).
- **G-ON-SIM** knob ON: the same six hashes (the cube draws feed nothing back into the sim).
- **G-DRAW-OFF** knob OFF, Training replay rows 884/924/959: no draw with texture in the
  FUN_00458630 name set and 12 tri (no change from the d1r1 standalone side).
- **G-DRAW-ON** knob ON, same replay: the standalone emits one 12-tri cube draw per original cube draw
  of the same frame, same texture, in the same order, `rs` equal to `[1,1,4,0,0,5,0,5,6,2,0,0]`,
  translation within 1e-3 of the original's `w[12..14]`.
- **P-SPIN** the cube world matrix is `RotY(theta)`: theta_stand - theta_orig is the same constant c on
  every cube of every row. GREEN at |c| < 0.01 deg. A non-zero constant is reported with its value as a
  tick-count offset (sim frame alignment, not the draw), not as a draw-side pass.
- **P-ORDER** the cube draws follow the standalone's object-shadow / world-fx draws and precede the HUD.

## 5. Not claimed

No C-level change (no Frida diff possible: the original does not boot). No default flip (decision row).
