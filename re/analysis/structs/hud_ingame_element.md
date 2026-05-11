# HUD Ingame Element Structs — `0x0063cab8` / `0x0063ce20`

**Produced by:** Session 6 (frontend_struct_extract), 2026-05-11  
**Pool slot:** Mashed_pool5  
**Source plates:** hud_ingame_d2/0x0041bc50.md, 0x0041c9a0.md, 0x0041d410.md, 0x0041de80.md; hud_ingame_d3/0x00428450.md

---

## Overview

The in-game HUD is organized as arrays of typed element-objects.  
Each element has a "guard" field (non-zero = visible/active) and a "render-object" pointer that is dispatched via `vtable[0x48]` to draw.  
Two distinct struct layouts have been identified, differing in guard/render base offsets and stride.

---

## Struct A: `HudIngameElement_16C` (stride 0x16C = 364 bytes)

**Array base:** 0x0063cab8 (2 entries)  
**Dispatcher:** FUN_0041bc50 (29 active slots), FUN_0041d410 (34 active slots)  
**Called from:** FUN_0041c0c0 (outer loop over 0x0063cab8, stride 0x16c, 2 entries)

### Dispatch pattern

```c
// guard_off ∈ [0x10..0x90]; render_off = guard_off + 0xA0
if (*(int32*)(in_EAX + guard_off) != 0)
    (**(code **)(*(int32*)(in_EAX + render_off) + 0x48))(*(int32*)(in_EAX + render_off));
```

### Guard slots (offsets into the struct)

Guard region: offsets 0x10 – 0x90 (int32 at each)  
Render-object pointer region: guard_off + 0xA0 (int32 ptr at each)

| Guard offset | Render offset | Present in FUN_0041bc50 | Present in FUN_0041d410 |
|-------------|---------------|------------------------|------------------------|
| 0x10 | 0xB0 | yes | yes |
| 0x14 | 0xB4 | yes | yes |
| 0x18 | 0xB8 | yes | yes |
| 0x1C | 0xBC | yes | yes |
| 0x20 | 0xC0 | yes | yes |
| 0x24 | 0xC4 | yes | yes |
| 0x28 | 0xC8 | yes | yes |
| 0x2C | 0xCC | yes | yes |
| 0x30 | 0xD0 | yes | yes |
| 0x34 | 0xD4 | yes | yes |
| 0x38 | 0xD8 | yes | yes |
| 0x3C | 0xDC | yes | yes |
| 0x40 | 0xE0 | yes | yes |
| 0x44 | 0xE4 | yes | yes |
| 0x48 | 0xE8 | yes | yes |
| 0x4C | 0xEC | yes | yes |
| 0x50 | 0xF0 | yes | yes |
| 0x54 | 0xF4 | NO (gap in bc50) | yes (d410 adds) |
| 0x58 | 0xF8 | NO | yes |
| 0x5C | 0xFC | NO | yes |
| 0x60 | 0x100 | NO | yes |
| 0x64 | 0x104 | NO | yes |
| 0x68 | 0x108 | yes | yes |
| 0x6C | 0x10C | yes | yes |
| 0x70 | 0x110 | yes | yes |
| 0x74 | 0x114 | yes | yes |
| 0x78 | 0x118 | yes | yes |
| 0x7C | 0x11C | yes | yes |
| 0x80 | 0x120 | yes | yes |
| 0x84 | 0x124 | yes | yes |
| 0x88 | 0x128 | yes | yes |
| 0x8C | 0x12C | yes | yes |
| 0x90 | 0x130 | yes | yes |

**Note:** Guard slots 0x54–0x64 are absent from FUN_0041bc50 (base game HUD) but present in FUN_0041d410 (larger HUD variant, loop base 0x0063d298 stride 0x160).

**Confirmed struct size:** Highest guard 0x90, render 0x130, render width 4 → last byte at 0x133. Caller loop stride 0x16C confirms 0x16C as the allocatable unit.

### Unobserved fields

| Range | Notes |
|-------|-------|
| +0x00..+0x0F | Not used as guard slots in observed dispatchers; may be header/count/flags |
| +0x94..+0x9F | Gap between last guard (0x90) and start of render region (0xB0) |
| +0x134..+0x16B | Tail padding or additional fields not yet observed |

---

## Struct B: `HudIngameElement_114` (stride 0x114 = 276 bytes)

**Array base:** 0x0063ce20 (4 entries)  
**Dispatcher:** FUN_0041c9a0  
**Called from:** FUN_0041ccc0 (outer 4-entry loop 0x0063ce20, stride 0x114, when `entry+0x110 != 0`)

### Dispatch pattern

```c
// 32-entry main loop: guard array at EAX+0x00, render array at EAX+0x80 (delta = 0x80)
for (i = 0..31): 
    if (*(int32*)(in_EAX + i*4) != 0 && !skip(i))
        (**(code **)(*(int32*)(in_EAX + 0x80 + i*4) + 0x48))(*(int32*)(in_EAX + 0x80 + i*4));
```

Skipped loop indices: 0, 3, 5, 6, 7, 8, 9, 10, 23 (0x17).

### Fixed extra slots (explicit, outside loop)

| Guard offset | Render offset | Notes |
|-------------|---------------|-------|
| 0x08 | 0x88 | With color injection: 0xFF323232 (opaque charcoal) written 3 levels deep before draw |
| 0x14 | 0x94 | Same color injection |
| 0x1C | 0x9C | Explicit post-loop |
| 0x20 | 0xA0 | |
| 0x24 | 0xA4 | |
| 0x28 | 0xA8 | |
| 0x2C | 0xAC | |
| 0x30 | 0xB0 | |
| 0x64 | 0xE4 | First explicit (before loop) |

### Field at +0x50

`ESI[0x14]` (= `*(int32*)(EAX + 0x50)`) is used as an inner index for conditional dispatch:  
`ESI[ESI[0x14] + 2]` — doubly-indirect render-object selection.  
Meaning of field +0x50 is unknown [UNCERTAIN].

### Active guard (`entry+0x110`)

FUN_0041ccc0 only calls FUN_0041c9a0 when `*(entry + 0x110) != 0` — this is an enable flag at struct+0x110.

---

## Struct C: `HudIngameElement_160` (stride 0x160 = 352 bytes)

**Array base:** 0x0063d298 (2 entries)  
**Dispatcher:** FUN_0041d410 (same dispatch table as Struct A variant, but different base/stride)  
**Called from:** FUN_0041d870

---

## Common render-object vtable dispatch

All three struct types route through `vtable[0x48]` for draw:

```c
(**(code **)(render_obj + 0x48))(render_obj);
```

`vtable[0x48]` = draw/render method. Color injection (when present) writes into:  
`render_obj + 0x18 → ptr → +0x20 → ptr → +4` = color dword field (ARGB 0xFF323232 = opaque charcoal).

---

## Globals cross-referencing HUD element arrays

| Address | Name (tentative) | Notes |
|---------|-----------------|-------|
| 0x0063cab8 | `g_hudIngameElem_A` | Struct A; 2 × 0x16C bytes |
| 0x0063ce20 | `g_hudIngameElem_B` | Struct B; 4 × 0x114 bytes |
| 0x0063d298 | `g_hudIngameElem_C` | Struct C; 2 × 0x160 bytes |
| 0x0063d5e8 | `g_elemArrayActiveFlag` | Non-zero guard checked before calling FUN_0041de80 |
| 0x00771964 | `g_debugOverlayHandle` | Non-zero → FUN_00428760(handle, 50.0, 30.0, 240.0, 120.0, 0) called from FUN_00403160 |
| 0x00771960 | `g_coinTexturePtrPtr` | Pointer to texture handle; dereferenced by FUN_00428450 for spinning-coin draw |
| 0x0067d974 | `g_coinSpinAngle` | float; spinning-coin angle accumulator; `+= DAT_005cc56c` per call |
| 0x007f1a1c | `g_renderSlotSelector` | int; used as `ESI[g_renderSlotSelector + 7]` index in FUN_0041de80 |

---

## Uncertainties

- [UNCERTAIN U-0927] FUN_0041de80: Ghidra could not determine object pointer register; appears in `unaff_ESI`. Non-standard call convention — ESI set by caller before call. True calling convention unknown.
- [UNCERTAIN] Struct B field +0x50 semantics: used as inner index for doubly-indirect dispatch. Meaning unknown without tracing writers.
- [UNCERTAIN] Guard region +0x00..+0x0F in Struct A: not used as guard slots by observed dispatchers; whether these hold struct header data (e.g., active element count, type tag) is unknown.
