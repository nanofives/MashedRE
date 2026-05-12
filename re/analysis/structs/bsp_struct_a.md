---
struct_name: BSP_AssetTable (BSP struct A)
base_address: DAT_006bf1c8
size_bytes: 0x2ae0
session_date: 2026-05-11
source_rvas: [0x0047a1b0, 0x0047a280, 0x0047a320, 0x0047a3a0, 0x0047a4a0,
              0x0047a540, 0x0047a580, 0x0047a5b0, 0x0047a5e0, 0x0047a610,
              0x0047a6b0, 0x0047a6f0, 0x0047a720, 0x0047a790, 0x0047a880,
              0x0047a8b0, 0x0047aa20, 0x0047aa50, 0x0047a1e0, 0x00478cb0]
---

## Overview

`DAT_006bf1c8` is a 0x2ae0-byte (10,976-byte) array of 0x40-byte (64-byte) filename slots, plus a companion handle/data region beyond the slot array. Zeroed wholesale by FUN_00478cb0 (0x00478cb0) at track-load reset time.

Each 0x40-byte slot holds a null-terminated ASCII filename (max 63 chars + NUL). Individual Lua C handlers registered at `LAB_00440bc0` populate specific slot indices and groups by calling into COURSE.LUA during track init (via `FUN_0047a020` → `FUN_0047b9b0`).

## Slot map (byte offset = slot_index × 0x40)

| Slot index | Byte offset | Handler RVA | Notes |
|---|---|---|---|
| 0 | 0x0000 | 0x0047a6f0 | Track name / base ID string |
| 5 | 0x0140 | 0x0047a5b0 | Unknown filename |
| 6 | 0x0180 | 0x0047a6b0 | Unknown filename + sets BSP-B flag at +0x1c |
| 10 | 0x0280 | 0x0047a5e0 | Unknown filename |
| 14+ | 0x0380+ | 0x0047a610 | DFF group, base slot 0xe, counter at BSP-B+4 |
| 30 | 0x0780 | 0x0047a540 | Unknown filename + sets BSP-B flag at +0x18 |
| 34 | 0x0880 | 0x0047a580 | Unknown filename |
| 38+ | 0x0980+ | 0x0047a3a0 | Filename group, base slot 0x26, counter at BSP-B+8 |
| 102 | 0x1980 | 0x0047a1b0 | Unknown filename (fixed slot, no counter) |
| 103+ | 0x19c0+ | 0x0047a280 | BSP filename group, base slot 0x67, counter at BSP-B+0xc |
| 120+ | 0x1e00+ | 0x0047a4a0 | Filename group, base slot 0x78, counter at BSP-B+0x10 |
| 136 | 0x2200 | 0x0047a880 | Unknown filename (fixed slot) |
| 137+ | 0x2240+ | 0x0047a8b0 | Filename group, base slot 0x89, counter at BSP-B+0x14 |
| 145+ | 0x2440+ | 0x0047aa50 | Filename group, base slot 0x91, counter at BSP-B+0x1d4 |
| 163+ | 0x28c0+ | 0x0047a320 | DFF group, base slot 0xa3, counter at BSP-B+0x218; companion handles at +0x2ac0 |
| (sky) | — | 0x0047a1e0 | Sky DFF: written at `(DAT_006bf1d4 + 1) * 0x40 + base` or `(FUN_004a2c48() + 1) * 0x40 + base`; counter DAT_006bf1d4 |

## Companion handle/data region (within DAT_006bf1c8)

| Byte offset | Description |
|---|---|
| 0x2ac0 | Dword array: resolved DFF handles parallel to slot-group base 0xa3 (stride 4; indexed by BSP-B+0x218 counter) |

## Notes on BSP struct B (DAT_006bf1cc, separate 0x21c-byte struct)

| Byte offset | Type | Description |
|---|---|---|
| +0x0 | int | Base pointer of BSP struct A (= DAT_006bf1c8) — written by FUN_0047a020 |
| +0x4 | int | Counter: DFF group base 0xe (written by 0x0047a610) |
| +0x8 | int | Counter: filename group base 0x26 (written by 0x0047a3a0) |
| +0xc | int | Counter: BSP filename group base 0x67 (written by 0x0047a280) |
| +0x10 | int | Counter: filename group base 0x78 (written by 0x0047a4a0) |
| +0x14 | int | Counter: filename group base 0x89 (written by 0x0047a8b0) |
| +0x18 | int | Flag: set to 1 by FUN_0047a540 (slot 30 written) |
| +0x1c | int | Flag: set to 1 by FUN_0047a6b0 (slot 6 written) |
| +0xd4 | int[] | Flag array: each element set to 1 by FUN_0047aa20 using FUN_004a2c48() as index |
| +0x1d4 | int | Counter: filename group base 0x91 (written by 0x0047aa50) |
| +0x1d8 | float | Ambient R component (written by FUN_0047aad0 at 0x0047aad0) |
| +0x1dc | float | Ambient G component |
| +0x1e0 | float | Ambient B component |
| +0x1e4 | float | 0x3f800000 = 1.0f — ambient alpha / intensity (written by 0x0047aad0) |
| +0x218 | int | Counter: DFF group base 0xa3 (written by 0x0047a320) |

## Unknown structs referenced by Lua C handlers

- `DAT_006bf1d8` (base), `DAT_006bf1dc` (counter): written by FUN_0047a720 and FUN_0047a790. Distinct from BSP struct A and B. See [UNCERTAIN U-3571].

## Uncertainties

- [UNCERTAIN U-3570] Slot-to-asset-type mapping is inferred from handler patterns only. The actual semantic meaning of each slot group (which group = track DFF, which = BSP collision data, which = AI navmesh, etc.) is unknown without Lua source side or runtime observation. Evidence missing: COURSE.LUA disassembly or runtime Frida tracing of slot contents.
