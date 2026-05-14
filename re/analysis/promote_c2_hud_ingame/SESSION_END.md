# Session 76 — promote_c2_hud_ingame — SESSION END

Date: 2026-05-13  
Pool slot: Mashed_pool5  
Model: claude-sonnet-4-6

## Summary

Promoted 20 C1 hud rows to C2. All plates written to `re/analysis/promote_c2_hud_ingame/`.

## RVAs promoted

| RVA | Name | Source analysis | Notes |
|-----|------|-----------------|-------|
| 0x004c1c80 | FUN_004c1c80 | hud_ingame_d2 | viewport dims setter; ratio computation |
| 0x00402fb0 | FUN_00402fb0 | hud_ingame_d2 | sub-mode 0xb pulsing spinner; sin-driven alpha |
| 0x00428760 | FUN_00428760 | hud_ingame_d2 | 6→7 arg passthrough; blendMode=0 |
| 0x00428610 | FUN_00428610 | hud_ingame_d2 | viewport-scaled rect draw; coordMode + UV flip |
| 0x0042b8b0 | FUN_0042b8b0 | hud_ingame_d2 | screen width getter (DAT_0067ea54) |
| 0x0042b8c0 | FUN_0042b8c0 | hud_ingame_d2 | screen height getter (DAT_0067ea56) |
| 0x0041c2d0 | FUN_0041c2d0 | hud_ingame_d2 | 4-slot unconditional vtable dispatch via EAX |
| 0x0041b340 | FUN_0041b340 | hud_ingame_d2 | per-slot HUD dispatcher; car-type + element flags |
| 0x0041bc50 | FUN_0041bc50 | hud_ingame_d2 | 29-slot guarded vtable dispatch; guard+0xA0=render |
| 0x0041c9a0 | FUN_0041c9a0 | hud_ingame_d2 | 32-entry loop + color injection (0xFF323232) |
| 0x0041d410 | FUN_0041d410 | hud_ingame_d2 | 34-slot variant (adds 0x54–0x64 range vs 0041bc50) |
| 0x0041de80 | FUN_0041de80 | hud_ingame_d2 | game-mode 4/7/8/9 HUD; ESI-based [U-0927] |
| 0x0041e630 | FUN_0041e630 | hud_ingame_d2 | 6-slot 2-digit number renderer; EDI-based [U-0928] |
| 0x00427f00 | FUN_00427f00 | font_text | TEXT_FN: char*→UTF16→screen coords→draw dispatch |
| 0x005554d0 | FUN_005554d0 | font_text | text width calc; ASCII+extended advance tables |
| 0x00556ca0 | FUN_00556ca0 | font_text_d2 | font-ctx draw thunk; *(ctx+0x138)() indirect |
| 0x00556cc0 | FUN_00556cc0 | font_text (hooks.csv) | SetCurrentFont; DAT_00912a20 = param_1 |
| 0x00556cd0 | FUN_00556cd0 | font_text (hooks.csv) | GetCurrentFont; return DAT_00912a20 |
| 0x00556d70 | FUN_00556d70 | font_text_d2 | font style-state allocator; 0x78-byte pool |
| 0x00555360 | FUN_00555360 | font_text | font system init; alloc ctx/buf/table; set loader fn |

## Uncertainties preserved

- U-0927: ESI-based call convention in FUN_0041de80 — noted in plate.
- U-0928: EDI-based call convention in FUN_0041e630 — noted in plate.
- U-1072: DAT_00912a1c type in FUN_00555360 — not resolved, noted.

## Next step

Run `re-classify` for each RVA to update hooks.csv C1→C2.
