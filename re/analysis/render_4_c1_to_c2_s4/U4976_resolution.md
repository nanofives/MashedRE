# U-4976 RESOLVED — `DAT_007d3ff8 + 0x88` is RwStandard index 16 = `FUN_004d0290` (raster unlock)

**Session:** orch-iter25 2026-07-31. Slot `Mashed_pool9`, read-only.
**Question:** identity of the vtable slot dispatched by `FUN_004c7600`:
`(**(code **)(DAT_007d3ff8 + 0x88))(0, param_1, 0)`.

## The table is the RW device standards array

- `DAT_007d3ff8` is the active `RwDevice*` (RW engine global; `mashed.log` confirms
  `RwEngineInit`/`RwEngineOpen`). Value is 0 in a static dump — populated at runtime.
- `FUN_004c30b0` (RwEngineOpen path) copies the 75-dword default device from
  `&DAT_007d3ec8` into a heap device, then registers **0x1d (29) standards** via the
  device *system* function with request `0xb`:
  `FUN_004c2c90(deviceSystem, 0xb, DAT_007d3ff8 + 0x12, 0, 0x1d)`.
  `DAT_007d3ff8 + 0x12` (dword) = **device byte offset +0x48** = the standards array base.
- The device system function is `FUN_004c7a70` (D3D9; `Direct3DCreate9` in case 0).
  Its case `0xb` calls **`FUN_004c8e50`**, the standards installer.

## The installer names index 16

`FUN_004c8e50(param_1 = standardsBase, _, count)` fills every slot with the default
`FUN_005c9d00`, then overwrites 27 named slots via `param_1[index] = funcptr` from an
inline `{index, funcptr}` table. The entry that matters:

```
local_dc[0x1a] = (code *)0x10;          // index 16
local_dc[0x1b] = FUN_004d0290;          // -> installed at standardsBase[16]
```

`standardsBase[16]` = `(device + 0x48) + 16*4` = **device + 0x88** — the slot `FUN_004c7600`
dispatches. Therefore **slot +0x88 = `FUN_004d0290`**.

Cross-check on the sibling: `FUN_004c7620` dispatches slot **+0xa8** = index
`(0xa8-0x48)/4 = 24`; the table sets `local_dc[0x1e]=0x18 (24) -> LAB_004d05b0`, and
`FUN_004c7620` clears bits in the raster after (`*(raster+0x22) &= 0xe7`) — consistent
with a paired unlock. (Note: the earlier informal "+0xa8 = RwRasterUnlockMipLevel" tag was
never binary-grounded; the binding proven here is +0xa8 -> index 24 -> `LAB_004d05b0`.)

## What `FUN_004d0290` does — NOT a getter

`FUN_004d0290(int *param_1, undefined4 *param_2)` is the **raster-unlock** standard
(param_2 = the raster). Mechanically it:

- Writes raster fields: `param_2[3]=param_2[10]`, `param_2[4]=param_2[0xb]`,
  `param_2[6]=0`, `param_2[1]=0`, and clears bits: `*(param_2+0x22) &= 0xf9`.
- Walks the raster **parent chain**: `do { p8=p1; p1=*p8; } while (p8 != *p8);` — an
  unbounded pointer chase; on a seeded/garbage pointer it dereferences arbitrary memory.
- Drives the **live D3D9 device** `DAT_007d4110`: `(**(*DAT_007d4110 + 0x78))(...)`
  (locked-bits upload to the surface) and `Release` (`+8`) on surface objects
  `DAT_007d4118`/`param_1`.
- Conditionally calls `FUN_004c5dd0` (the +0x2c-slot sibling) on the chain root.

## Consequence for `0x004c7600` — NOT READY

`FUN_004c7600` is a **raster-unlock wrapper** (`RwRasterUnlock`-shape), not a read-only
getter. A synthetic early-window force-call with a seeded pointer:

1. takes the null-error path when `param_2[1] == 0` (a zero buffer) — both sides return 0,
   a **degenerate false GREEN**; or
2. with a non-zero seed, AVs walking the raster parent chain over garbage; or
3. if it survived, corrupts the live renderer via the D3D9 device vtable.

So harness_safety is **DESTROYS_DEVICE / AV**, not SAFE. `int_scalar` was the right *shape*
but the row cannot go through the synthetic getter lane. It is not cleanly A/B-viable either
— it needs a genuinely locked raster with a valid parent chain and a live device
(AB_UNENUMERABLE-class). **Verdict: stays C2; do not author into the getter lane.**

## Tracker action (PENDING — not applied here)

The U-4976 row lives in `UNCERTAINTIES.md`, which another live session owns this checkout.
This note is the evidence; the U-row close must go through `re-classify` **after**
coordinating via the multi-session skill. Do not hand-edit `UNCERTAINTIES.md`.
