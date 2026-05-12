# Vehicle Damage State Struct — Initial Draft

**Status:** C1 evidence (all fields observed in decompilation; no runtime validation)  
**Session:** vehicle_damage_d4 (2026-05-12)  
**Sources:** vehicle_damage d1–d4 sessions, race_results, race_results_d2, vehicle_update

---

## Per-car state block

One contiguous block per car. 16 cars maximum (indices 0–15).

**Base address:** `0x008815a4`  
**Stride:** `0x341 dwords` = `0xd04 bytes` (3332 bytes per car)

Car N block starts at `0x008815a4 + N * 0xd04`.

| Offset (bytes) | Symbol | Type | Values | Source |
|---|---|---|---|---|
| +0x000 | active_flag | DWORD | 0 = slot unused; 1 = car active; 0xffffffff = OOB | FUN_0046c7b0 (read), FUN_0046c5c0 (zeroed on destroy) |
| +0x00C | state | DWORD | 0 = normal; 1 = destroyed | FUN_0046c770 (read), FUN_0046c790 (write) |
| +0x0B0C | control_source | DWORD | Written from `DAT_007f1030` by FUN_0046c5c0 on destroy | FUN_0046c5c0 |
| +0xA4C | spinout_flag | DWORD | 0 = alive; 2 = slide/spin | FUN_0046cbb0 *param_2 (read) |
| +0xA50 | secondary_field | DWORD | Purpose unknown | FUN_0046cbb0 *param_3 (read) |

> Note: offset +0x924 holds a pointer to a per-car transform sub-object (see below).  
> Byte offsets above are within a single car's 0xd04-byte block.

### Evidence for stride

`FUN_0046cbb0` explicitly: `DAT_00881f90 + param_1 * 0xd04` and stride note `0x341 × 4 = 0xd04`. All other getters use the same `[param_1 * 0x341]` dword-index pattern.

### Field cross-references

`FUN_0046c5c0` (Car deactivate): zeroes `active_flag` at +0x000; writes `DAT_007f1030` into `DAT_008820b0[car * 0xd04]` — that address is base + offset **+0xB0C** = `0x008820b0 − 0x008815a4 = 0xB0C`.

`FUN_0046c790` (State write): writes `param_2` to `DAT_008815b0[car * 0x341]` — byte offset **+0x00C** = `0x008815b0 − 0x008815a4 = 0xC`.

---

## Per-car transform sub-object

**Base address:** `0x00881ec8`  
**Stride:** same 0xd04 bytes per car (same block, offset +0x924 from block base)  
> `0x00881ec8 − 0x008815a4 = 0x924`

Sub-object layout (relative to sub-object pointer):

| Offset | Field | Notes |
|---|---|---|
| +0x30 | pos_x | Float; read by FUN_0046d4a0 callers (FUN_0040e180, FUN_00441c80) |
| +0x34 | pos_y | Float |
| +0x38 | pos_z | Float |
| +0x80 | sub_transform_index | Dword; multiplied by 0x10 to get sub-transform offset; used by FUN_0046d4a0 |

FUN_0046d4a0 returns `*(ptr + sub_transform_index * 0x10)` as the actual transform pointer; callers then read +0x30/+0x38 as X/Z.

---

## Per-car score/position block

**Base address:** `0x008a9640`  
**Stride:** `0x30c bytes` (780) per car

| Offset | Symbol | Type | Notes |
|---|---|---|---|
| +0x00 | pos_rounded_a | DWORD | Rounded position value; written by FUN_00408a70 field +0x00 |
| +0x04 | pos_rounded_b | DWORD | Same rounded value; purpose of duplication unknown (U-2169) |
| +0xA8 | pos_float | FLOAT | Raw float read/written by FUN_00408a50 / FUN_00408a70 |
| +0xB0 | pos_rounded_c | DWORD | Same rounded value, third copy |
| +0xB4 | pos_rounded_d | DWORD | Same rounded value, fourth copy |

**Separate score arrays** (stride 4 bytes per car):

| Base | Symbol | Notes |
|---|---|---|
| `0x008a94e0` | score_accumulated | += param_2 each FUN_0040b290 call; clamped ≥ 0 |
| `0x008a9500` | score_filtered | Receives sign-clamped delta |
| `0x008a9510` | score_const6000 | Written constant 6000 per FUN_0040b290 call; purpose unknown (U-1298) |
| `0x008a9520` | score_raw_delta | Raw param_2 written each call |
| `0x008a9570` | score_previous | Previous value of score_accumulated |

**Other score/race globals (stride 4, car-indexed):**

| Address | Symbol | Notes |
|---|---|---|
| `0x008a94c0` | finish_order_head | Sentinel/head of finish-order array |
| `0x008a94c4` | finish_order | Car-index written per FUN_0040eee0; -1 = empty slot |
| `0x008a94d0` | car_count | Active player/car count; loop bound in FUN_0040e340, FUN_00410d10 mode 7 |
| `0x0089a880` | race_float_arr | Flat float[N] per car; compared against mode-specific thresholds in FUN_00410d10 |

---

## Per-car damage/spinout globals (flat arrays)

| Address | Symbol | Stride | Notes |
|---|---|---|---|
| `0x00898980` | collision_event | — | Single float; written by Rwp37 collision callback (writer unresolved — no static xref); sentinel 10.0f = collision this step |
| `0x008a96e8` | precisepos_float | 0x30c per car | Float position; read by FUN_00408a50, written by FUN_00408a70 |

---

## Spectator/camera slot data

When a car is eliminated, FUN_00419760 sets up a spectator slot in a parallel region.

**Slot base:** `DAT_0063c018`, stride **0x6c bytes** (108) per car  
**Linked-list head:** `DAT_0063bf30`, stride **0x54 bytes** per node (max 4 nodes)

| Offset within slot | Field | Notes |
|---|---|---|
| +0x00 | skin_handle_a | From `DAT_005f32b0[skin_idx * 8]` |
| +0x04 | skin_handle_b | From `DAT_005f32b4[skin_idx * 8]` |
| +0x10 | skin_handle (slot) | Written by FUN_00419760 from DAT_005f3298 lookup |
| +0x34 | zeroed_a | Reset by FUN_00418a30 |
| +0x38..+0x4c | zeroed_b..f | Reset by FUN_00418a30 |
| +0x50 | sub_transform_idx | Read by FUN_00418a30 as table index |
| +0x54 | player_slot | Written by FUN_00419760 from DAT_007f1a14 |
| +0x64 | sentinel | Set to 0xffffffff (-1) by FUN_00418a30 |

**Additional per-car field:**

| Address | Stride | Value | Notes |
|---|---|---|---|
| `DAT_0063c04c` | 0x6c per car | 2 | Written by FUN_00419760 on elimination |

---

## Open uncertainties

- **U-1854**: Writer of `DAT_00898980` (collision_event float) is unresolved. Requires Frida write-watchpoint at 0x00898980 during a live collision.
- **U-1855**: Exact values of spinout_flag beyond 0=alive and 2=slide not confirmed.
- **U-1856**: Secondary field at +0xA50 (`DAT_00881f94`) — no caller reads its value in any analysed session.
- **U-1857**: Relationship between the active-flag block at 0x008815a4 and the spinout block at 0x00881f90 within the same 0xd04-byte stride not yet confirmed via a single contiguous memory_read.
- **U-2169**: Four position fields (+0x00, +0x04, +0xb0, +0xb4) receive same rounded integer; distinction unknown.
- **U-3600**: `DAT_007f1a1c` car→skin-index mapping — write sites unknown.
- **U-3601**: Full spectator linked-list node structure (stride 0x54) — only +0x50 slot-pointer field observed.
