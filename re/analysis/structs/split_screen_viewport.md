# SplitScreenViewportState — per-player viewport render context

**Session drafted:** split_screen_d2_cont1-20260513  
**Evidence plates:** FUN_0041f8f0 (C1), FUN_004228f0 (C1), FUN_00422ac0 (C2), FUN_00422af0 (C2), FUN_00422570 (C1)  
**Base:** `DAT_0063dc38` — array of per-player slots (ESI-based access)  
**Stride:** 0x2AC (684 bytes) per player slot  
**Max slots:** 2 (split-screen; possibly 4 for 4-player, [UNCERTAIN])

## Struct layout (offsets from slot base, ESI = &DAT_0063dc38[slot])

All offsets observed from FUN_0041f8f0 (ESI-based) unless noted.

| Offset | Type | Name | Source / Notes |
|--------|------|------|----------------|
| +0x00 | int[~0x28c/4] | camera_array[] | Array of int pointers; indexed by `camera_array_idx` at +0x28c |
| +0x258 | float | player_pos_x | Vehicle/player position X; used in look-at offset computation |
| +0x25c | float | player_pos_y | Vehicle/player position Y |
| +0x260 | float | player_pos_z | Vehicle/player position Z |
| +0x26c | int* | world_sector_ptr_ptr | `*(*(ESI+0x26c)+4)+0x10` → world sector for triangle query |
| +0x284 | float | view_scale_h | Horizontal scale multiplier for view-distance query |
| +0x288 | float | view_scale_v | Vertical scale multiplier for view-distance query |
| +0x28c | int | camera_array_idx | Index into camera_array at +0x00 |
| +0x290 | int | render_slot | Player render slot index (0 or 1); used as `param_1` to 00422ac0/af0/22570 |
| +0x298 | int | mode | Game mode flag: `==2` → special camera mode selection |
| +0x2a0 | int | camera_handle | Selected RwCamera handle (from DAT_0063e490 table) |
| +0x2a4 | int | camera_frame | `*(camera_handle+4)` — camera frame pointer |
| +0x2a8 | int | raster_handle | Parallel selection from DAT_0063d854 table |

## Second struct: GroundOverlayBatch (fragmented flat arrays)

A separate per-player state for the ground triangle overlay system. Not a contiguous struct — accessed as offsets into several global flat arrays with stride 0xf40 = 3904.

**slot index** = `*(ESI+0x290)` (render_slot from SplitScreenViewportState).

| Global | Stride | Role |
|--------|--------|------|
| DAT_00641318 + slot*0xf40 | 0xf40 | Triangle count (int) |
| DAT_0064131c + slot*0xf40 | 0xf40 | Current array write index (int) |
| DAT_00641320 + slot*0xf40 | 0xf40 | Camera handle (set by FUN_00422af0) |
| DAT_00641324 + slot*0xf40 | 0xf40 | Y-axis bias / elevation offset (float) |
| DAT_00641313 + slot*0xf40 | 0xf40 | Dirty byte (from x87 rounding) |
| DAT_006412e8 + slot*0xf40 | 0xf40 | Texture/color pointer array [4] (set by FUN_00422ac0) |
| DAT_006412ec + slot*0xf40 | 0xf40 | color array[1] |
| DAT_006412f0 + slot*0xf40 | 0xf40 | color array[2] |
| DAT_006412f4 + slot*0xf40 | 0xf40 | color array[3] |
| DAT_006403e8 + slot*0xf40 + count*0x3c | 0xf40 outer | Triangle vertex output buffer (0x3c stride = 15 undefined4 per triangle) |
| DAT_006403b8 + slot*0xc | 0xc | Cached camera X (separate stride) |
| DAT_006403bc + slot*0xc | 0xc | Cached camera Y |
| DAT_006403c0 + slot*0xc | 0xc | Cached camera Z |

## Camera selection table (global)

From FUN_0041f8f0 camera/raster selection block:

| Global | Role |
|--------|------|
| DAT_0063d850 | Guard object; `*(DAT_0063d850+4)` = position-source object |
| DAT_0063e490 | Camera handle table [2 entries]; `[0]` = default, `[1]` = mode==2 |
| DAT_0063d854 | Raster handle table [2 entries]; parallel to DAT_0063e490 |

## Uncertainties

| ID | Question |
|----|---------|
| U-2847 | Role of DAT_0063d850 as guard — is it a "scene active" flag or a specific camera object? |
| U-2848 | Are DAT_0063e490 / DAT_0063d854 indexed by mode==2 or by player slot? Two-entry table suggests mode-based |
