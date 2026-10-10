# RESULT: mode 7 leg M2b, the car registrant `FUN_0041f710`

Pre-registration: `PREREG_M2.md` §5 (`28b021d6`, measurements done and registered before code). Knob `MASHED_REG_CARS`
(default OFF, needs `MASHED_REGISTRY`).

## 0. Verdict first

> **Every M2b gate PASSES.** With `MASHED_REGISTRY + MASHED_PICKUPS_ORIG + MASHED_REG_CARS` the exe's registry holds, per
> frame, the 4 cars in slots 0..3 (car order) followed by the 5 pickups, as the original's does. Every car entry's position
> is **bitwise equal to the car record's `+0x958..+0x960` at publish on 55,429 / 55,429 entries**, which is the relation
> measured on the original (1,600 / 1,600, `o_cm1`). Knob OFF and knob ON: `ae67b13a / c8317ce2 / 31db9384`.

| gate | result |
|---|---|
| measurement (original, `o_cm1`) | registered car position == record `+0x958..+0x960` bitwise 1,600 / 1,600; slots 0..3 in car order on every frame |
| G-M2b-LIVE | 55,429 / 55,429 car entries bitwise == `VehRecord(v)+0x958`; type `0xff00+v`; radius `0x3f088c2d`; no callback; 0 slot/order violations |
| cars per frame | 4 on 13,857 frames; 1 on frame 0 (only the player passes the per-car gate on the first frame) |
| G-M2b-OFF / INERT | `M2B_CTRL` and `M2B_ON`: `ae67b13a / c8317ce2 / 31db9384` |

## 1. Why the position is exact

`FUN_0040da50` passes the body matrix (`FUN_0046d4a0`, `0x0040dac2`) to `FUN_00420870` (`0x0040dad0`), which replaces the
part-19 frame's modelling matrix with it (`FUN_004c1480(frame, matrix, 0)`, `0x004208ae`) and copies that frame's LTM
translation to `rec+0x258` (`0x004208ba..0x004208d5`). The visual record is never built in the exe; a fake 0x2ac record per
car carries the three fields `FUN_0041f710` reads.

## 2. Open

- **U-9218:** the radius `rec+0x264` (`0x3f088c2d`, 0.53338, the car's culling sphere read by the frustum test
  `FUN_004c1b40` at `0x00420081`) has no writer found: no literal in the image, no direct store in `0x0041e000..0x00421000`.
  The exe uses the measured bits. Other car models or tracks may differ.
- The exe's registry lacks the original's other registrants (types 0x0A, 0x11; `FUN_004532f0`, `FUN_00454350`, ...).
  0x11 entries fire `0x00486630` against cars (3,001 times in `o_rg4`); they matter for M3 only if that callback does.

## 3. Next: M3 (callbacks live)

Dispatch the recorded callbacks: `FUN_00458920` (pickup collected / bumped) -> the exe's `PowerupSystem::Activate`, with
`PickupField`'s own collection standing down under the knob. Pre-register the powerup gates first (collection count and
timing vs the original, respawn timing, the bump arm that U-9217 needs).

## 4. Raw

`o_cm1.msd.reg.tsv`, `decomp_m2{c,d,e}.txt`, `xrefs_m2b.txt`, `../d3_gatefire_20261008/M2B_{CTRL,ON}.*`
(`M2B_ON.registry.csv` has the per-entry `car_pos_eq` column).
