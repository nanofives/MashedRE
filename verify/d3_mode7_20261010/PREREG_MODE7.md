# PREREG: mode 7 (`FUN_00414c30`) on a faithful world-object registry: scope + leg M1 gates

Date 2026-10-10. USER DECISION (this session): the faithful route, option (1) "full broker", staged M1..M4.
Committed before any code. Evidence: `decomp_registry.txt`, `decomp_registry_leaves.txt` (this folder,
`decomp_pc.py` on a read-only pool clone), `../d3_modes37_20261009/FINDINGS.md`, worker survey (recorded in §1).

## 1. What the registry is in the original

- Array `0x006dccb8`, 300 slots of 0x8c (`0x00484d0e` cap 0x12c), write index `DAT_006e70cc`, published count
  `DAT_006e70d8` (read by `FUN_00484c70`), grid-built flag `DAT_006e70d4`, grid-enable flag `DAT_006e70c8`.
- Registrar `FUN_00484cf0(desc)` (231 B): if `d4 == 0 && c8 != 0` builds the grid (`FUN_004847d0`, 628 B:
  11 x 11 cells of 0x1b8 at `0x006cfcb8..0x006dccb0`, oriented by `FUN_00467210(0)` (+0x20..+0x28) and centred
  on `FUN_00443090()` = `&DAT_00897ff0`, `DAT_006dccb0` = that pointer); copies the 8-dword descriptor
  (pos xyz, radius, type, data, +0x18, callback +0x1c) into slot `cc`, stores `cc` as a short at +0x20, zeroes
  +0x24..+0x64 and the 9-dword pair bitmap +0x68..; if `c8`, inserts into grid cells (`FUN_00484ac0`, 424 B, via
  `FUN_00484a50`, 108 B, EAX = cell) and returns 0 without advancing `cc` when that fails; else `cc++`.
- 13 registrars (worker survey): `FUN_00459000` pickups (type 7, r 0.5, callback `FUN_00458920`), `FUN_0041f710`
  cars (type `0xff00 + v`), `FUN_00419a00`, `FUN_0044bbc0`, `FUN_0044c490`, `FUN_004532f0`, `FUN_00454350`,
  `FUN_004570a0`, `FUN_00458080`, `FUN_0045a530`, `FUN_0045bba0`, `FUN_00481a30`, `FUN_00486830`.
- Once per frame `FUN_0040fc00` calls `FUN_00485070` (`0x0040fd07`, 52 B): the pair pass (`FUN_00484de0`,
  394 B, grid; `FUN_00484f70`, 254 B, O(N^2) when `c8 == 0`): for each pair with different `type` (+0x10),
  at least one callback, bit not yet set, and `|p_a - p_b|^2 < (r_a + r_b)^2`, calls `cb_a(a, b)` and `cb_b(b, a)`
  (`0x00484edb` / `0x00484ee9`; `0x0048500e` / `0x0048501c`) and sets both pair bits. Then `d8 = cc; cc = 0;
  d4 = 0`. Init `FUN_00484c90` (83 B) at race reset (`FUN_004111c0`, `FUN_0040cfd0`).
- Pickup collection is a callback of this pass: `FUN_00458920(pickup, car)` -> state 2, `FUN_0045c010(car, type)`.
- The exe has none of it: `.bss` zeros, no registrar, reset only in the `.asi` (`Lane2/L2_00485070.cpp`), pickups
  collected by `D3d9Render/PickupField.cpp:249/275` into `PowerupSystem::Activate`.

## 2. Legs (each default-OFF, each its own gates before the next)

| leg | content | acceptance |
|---|---|---|
| **M1** registry core | verbatim naked `FUN_00484cf0`, `FUN_004847d0`, `FUN_00484ac0`, `FUN_00484a50`, `FUN_00485070`, `FUN_00484de0`, `FUN_00484f70`, `FUN_00484c90` (+ `FUN_004c39b0` LUT copy). Callbacks go to a recorder (no effect). Exe: init at race reset, publish per frame, behind `MASHED_REGISTRY`; no registrant yet | §3 |
| **M2** registrants | pickup pool at the original layout + `FUN_00459000` logic half (sprite calls no-op, `PickupField` still draws) + car registrant `FUN_0041f710` | registry contents vs the original over a race |
| **M3** broker live | callbacks dispatched: `FUN_00458920` -> `Activate`; `PickupField` collection stands down under the knob | powerup collect counts + respawn timing vs the original |
| **M4** consumer | `FUN_00414c30` + `FUN_00414490` + wiring + score (car 1: 49 mode-7 calls) | as P3 |

## 3. Leg M1 gates (registered now)

- **G-M1-CAPTURE.** New `scenario_launch.py --reg-probe K`, original, Training (o_wobj1 recipe): entry hook on
  `FUN_004847d0` (inputs: `c8`, the 3 floats `FUN_00467210(0)+0x20..+0x28`, `DAT_00897ff0..f8`), entry + leave
  on `FUN_00485070` every K-th frame (K = 30): `cc`, `c8`, `d4`, slots `[0, cc)` raw, the grid block
  `0x006cfcb8..0x006dccb4` when `c8 != 0`, and at leave `d8`, `cc`, slots `[0, d8)` raw. Read-only.
- **G-M1-REG (transcription).** Per captured frame, from `cc = d4 = 0`, replay each slot's descriptor (call order =
  slot order) through the port registrar (grid inputs replayed). PASS = slots `[0, cc)` and the grid block
  byte-identical to the entry dump, 100% of frames. [UNCERTAIN until measured] a registration that failed in the
  original leaves no slot to replay; if `c8 != 0` and a frame's cell counts disagree, that is the first suspect.
- **G-M1-PASS (transcription).** Then the port `FUN_00485070` with the recorder: slots `[0, d8)`, `d8`, `cc`, `d4`
  byte-identical to the leave dump, 100% of frames, and every recorded `(cb, a, b)` has both pair bits set.
  Callbacks that mutate a slot in the original would show here as a mismatch, reported per field.
- **G-M1-OFF / G-M1-INERT.** Knob OFF and knob ON: `ae67b13a / c8317ce2 / 31db9384` (no registrant exists yet,
  so the pass sees `cc = 0` every frame).
- Predictions: G-M1-REG and G-M1-PASS PASS at PC_24 and at PC_53 alike (the only float work is the overlap
  test and, with the grid, `floor`/`_ftol2` cell indices and one `FUN_004c39b0`); G-M1-INERT PASS by construction.
- Order of the publish relative to the AI tick in `FUN_0040fc00` is NOT needed for M1; it is recorded as an M4
  input (`FUN_00414c30` reads the published count `d8` while slots `[0, cc)` are being rewritten).
