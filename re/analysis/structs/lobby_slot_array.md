# Lobby / Player-Slot-State Array — `DAT_007f0a44` family

**Produced by:** Session 6 (frontend_struct_extract), 2026-05-11  
**Pool slot:** Mashed_pool5  
**Source plates:** hud_frontend_d4/0x0042ebe0.md, 0x00430830.md; c0_promotion_frontend_a/0x0042ebe0.md; hud_frontend_d4/notes.md

---

## Overview

A per-player-slot state array beginning at 0x007f0a44.  
`FUN_00430830` reads fields from this array using `param_1` (player slot index) and a `param_2` mode selector.  
`FUN_0042ebe0` iterates a larger record starting at 0x007f0a48 with a stride of 0x78 dwords (480 bytes) and then reads the per-slot header.

Two stride interpretations have been observed from different callers; see [UNCERTAIN U-3458] below.

---

## Per-slot record (base 0x007f0a44, stride 0xc bytes per slot)

This view is used by `FUN_00430830` (RVA 0x00430830).

| Byte offset | Address (slot 0) | Name (tentative) | Width | First read RVA | Notes |
|-------------|-----------------|-----------------|-------|----------------|-------|
| +0x00 | 0x007f0a44 | `slotStateA` | int32 | 0x00430843 | modes 2, 3, 10 — read by FUN_00430830 |
| +0x04 | 0x007f0a48 | `slotStateB` | int32 | 0x00430858 | mode 4 — read by FUN_00430830 |
| +0x08 | 0x007f0a4c | `slotStateC` | int32 | 0x0043086e | mode 6 — read by FUN_00430830 |
| +0x10 | 0x007f0a54 | `slotStateD` | int32 | 0x00430863 | mode 5 — read by FUN_00430830 |
| +0x1c | 0x007f0a60 | `slotStateE` | uint32 | 0x0043087d | mode 7; access is `*(uint32*)(&DAT_007f0a60 + param_1 × 0x30)` → stride 0x30 in this mode |
| +0x20 | 0x007f0a64 | `slotStateF` | uint32 | 0x0043088e | mode 8; same 0x30-byte stride |
| +0x24 | 0x007f0a68 | `slotStateG` | int32 | 0x0043089f | mode 9 — read by FUN_00430830 (stride 0xc) |

**Return value of FUN_0042ebe0 (validity test):**  
`((&DAT_007f0a44)[param_1 * 0xc] != 0) && !bVar1`  
→ slot is "ready/present" if `slotStateA[slot] != 0` and the 480-byte record scan found no disqualifying condition.

---

## Large record at 0x007f0a48 (stride 0x78 dwords per record)

`FUN_0042ebe0` walks from `&DAT_007f0a48` with int* stride 0x78 (= 0x1e0 bytes per step), upper bound 0x7f0c28.  
`0x7f0c28 − 0x7f0a48 = 0x1e0` → exactly 1 record span (0 iterations before bound; or bound is inclusive of end-of-first-record).

Within each 480-byte record, the function tests fields at word offsets 0x0b through 0x6f (int* arithmetic); when any such field meets a condition, `bVar1 = true`.

- [UNCERTAIN U-3458] The 120-dword (480-byte) record at 0x007f0a48 relationship to the 0xc-byte per-slot array at 0x007f0a44 is unclear. Whether this is one large slot-0 record or a parallel data structure is unresolved. Evidence needed: xrefs to 0x007f0a48 and surrounding write-site analysis.
- [UNCERTAIN U-3180] Semantics of the `slotState` field values (A through G). Whether these are player-ready flags, lobby state, connection state, or AI-select codes is unknown. Evidence needed: write sites for these arrays.

---

## Vehicle unlock flag table (`DAT_007f0e50`)

| Address | Name (tentative) | Stride | Entry count | Notes |
|---------|-----------------|--------|-------------|-------|
| 0x007f0e50 | `g_vehicleUnlockTable` | 0xc bytes (12 bytes) per vehicle | [UNCERTAIN] | Each vehicle has a 12-byte record with unlock flags at specific byte offsets. |

### Per-vehicle record layout (12 bytes, RVA FUN_0042ef40)

| Byte offset | Address (vehicle 0) | `param_2` selector | Name (tentative) | Notes |
|-------------|--------------------|--------------------|-----------------|-------|
| +0 | 0x007f0e50 | 2 (default) | `unlockType2` | byte; == 0x01 → unlocked |
| +1 | 0x007f0e51 | 3 / 0x3e9 | `unlockType3` | byte; == 0x01 → unlocked |
| +2 | 0x007f0e52 | 4 / 0x3ea | `unlockType4` | byte; == 0x01 → unlocked |
| +5 | 0x007f0e55 | 5 / 0x3ed | `unlockType5` | byte; == 0x01 → unlocked |
| +11 | 0x007f0e5b | 10 / 0x3f3 | `unlockType10` | byte; == 0x01 → unlocked |
| +3,+4,+6..+10 | — | — | [uncharacterized] | Not accessed in FUN_0042ef40 |

Note: `param_2` values 1000-offset (0x3e9=1001, 0x3ea=1002, 0x3ed=1005, 0x3f3=1011) map to the same byte offsets as their plain counterparts (3, 4, 5, 10).

- [UNCERTAIN U-3176] No bounds check on vehicle index visible; max vehicle count unknown. Evidence needed: `g_vehicleCount` global (observed at 0x0067f17c in FUN_0042ee40).

---

## Input device type table (`DAT_007e96fc`)

| Address | Name (tentative) | Stride | Entry count | Notes |
|---------|-----------------|--------|-------------|-------|
| 0x007e96fc | `g_inputDeviceTypes` | 0x80 bytes (128) per player | 4 (likely) | `(&DAT_007e96fc)[param_1 * 0x80]` — first byte = device type for player param_1. |

Observed values: [UNCERTAIN U-3454] — mode 2 = keyboard (inferred from string at 0x005cd778), mode 1 = gamepad by exclusion. Not confirmed against input subsystem.

---

## Lap-time record array (`DAT_008989e0`)

| Address | Name (tentative) | Stride | Notes |
|---------|-----------------|--------|-------|
| 0x008989e0 | `g_lapTimeRecords` | 0xc bytes (12) per record | `__thiscall`: index = (in_EAX + param_1 × 2) × 0xc |

### Per-record layout (12 bytes, RVA FUN_00430b30)

| Byte offset | Address (record 0) | Name (tentative) | Width | Notes |
|-------------|-------------------|-----------------|-------|-------|
| +0 | 0x008989e0 | `field0` | uint32 | Likely minutes or total-frames |
| +4 | 0x008989e4 | `field1` | uint32 | Likely seconds |
| +8 | 0x008989e8 | `field2` | uint32 | Likely centiseconds |

Note: field semantics (minutes/seconds/centisecs) are consistent with FUN_0042d300's decomposition but not proven from FUN_00430b30 alone.
