---
rva: 0x0048ebf0
name_in_ghidra: FUN_0048ebf0
size_bytes: 0x1f1
confidence: C2
callees_depth1: [FUN_00472650, FUN_0048ebc0]
opened_in_slot: Mashed_pool12
session_date: 2026-05-25
---

## Mechanical description
- Spawns one fire-column burst entry into the explosion ring buffer slot at index param_3.
- param_1 = float* world position (3 floats: X/Y/Z); param_2 = float intensity/scale; param_3 = int ring-buffer index.
- Ring buffer base: DAT_0076d994 (dword array), stride 0x122 dwords, 0x488 bytes per entry.
- Entry layout offsets cited (all relative to ring base at stride param_3 * 0x122):
  - +0x4 (DAT_0076d998): active-entry count.
  - +0x8 (DAT_0076d9a8): stored-position stack pointer.
  - +0x12 (DAT_0076d9ac): next write slot index.
  - +0x50..+0xfc: per-particle position arrays (X/Y/Z triples).
  - +0x2ce (DAT_0076dbc8/+0xcc): per-particle size pairs.
  - +0x2d4 (DAT_0076dbc8+): per-particle intensity pairs.
- If the ring already has stored positions (`DAT_0076d998[idx*0x122] != 0` at 0x0048ec08):
  - Decrements stored-position pointer (DAT_0076d9a8) at 0x0048ec13.
  - Pops a previously-stored XYZ from DAT_0076dc88 (iVar4*3+idx*0x122) into *param_1 at 0x0048ec1b.
  - Divides param_2 by a random float in [0.8, 1.2] via FUN_00472650 at 0x0048ec34.
  - Zeros DAT_0076ddc8[idx*0x122+iVar4] (eviction flag) at 0x0048ec49.
- Computes scaled intensity: `param_2 * DAT_005cd088`; clamps to DAT_005cc320 = 1.0f at 0x0048ec50.
- If no stored positions (`DAT_0076d998 == 0` at 0x0048ec5d): writes the world position and two intensity values (param_2 * DAT_005cd110 repeated) into the ring at offsets DAT_0076daf0/+4/+8 and DAT_0076dafc/+0/+4; zeroes DAT_0076db04 at 0x0048ec82.
- Writes position XYZ to DAT_0076db08 triple (iVar6*3+idx*0x122) at 0x0048ec88.
- Writes intensity pair to DAT_0076dbc8 (iVar4*2) at 0x0048ecac.
- Zeroes DAT_0076dc68 and sets DAT_0076dc08 = 0x437f0000 (255.0f) at 0x0048ecb4.
- Writes random height offsets (via FUN_00472650) into DAT_0076dc28/db68/db6c/db70 at 0x0048ecbd..0x0048ece8.
- Increments DAT_0076d9ac (write slot) at 0x0048ecec.
- Calls FUN_0048ebc0(param_3) to get particle count iVar6 at 0x0048ecf2.
- Loops from uVar5=stored*iVar6 to uVar5<stored*iVar6+iVar6 filling per-particle positions and intensities.
  - Inner-ring velocity: scaled random via FUN_00472650 written to +0x18/+0x19/+0x1a triples.
  - Outer-ring XZ jitter: param_2 * DAT_005ce4b8/005cc9c0; Y: param_2 * DAT_005cc9a0/005cc8f0.
  - Writes angle/size floats into DAT_0076dad0 and DAT_0076da74.
- Clears DAT_0076d99c[idx*0x122] = 0; increments DAT_0076d998 at 0x0048edd4.
- Callers: FUN_00490500 (0x00490500).

## Constants
| RVA cited at | Raw hex | Signed dec | Notes |
|---|---|---|---|
| 0x0048ec08 | 0x0076d998 | — | active-entry count array base |
| 0x0048ec08 | 0x122 | 290 | ring buffer entry stride in dwords |
| 0x0048ec08 | 0x488 | 1160 | ring buffer entry stride in bytes |
| 0x0048ecb4 | 0x437f0000 | — | float 255.0 (lifetime/fade max) |
| 0x0048ec50 | 0x005cd088 | — | intensity scale constant |
| 0x0048ec50 | 0x005cc320 | — | intensity clamp max (1.0f) |

## Uncertainties
- [UNCERTAIN] Full semantic names of all per-particle arrays not established; offsets cited mechanically only.
- [UNCERTAIN] FUN_00472650 semantics confirmed as random-float-in-range from prior analysis but not verified in this session.

## Stubs encountered
- none
