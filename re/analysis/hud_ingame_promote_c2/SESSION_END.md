# Session END — hud_ingame_promote_c2

**Date:** 2026-05-11  
**Pool slot:** Mashed_pool3 (read-only)  
**Model:** claude-sonnet-4-6  
**Bucket dir:** re/analysis/hud_ingame_promote_c2/

## Outcomes

- 16 C1 → C2 promotions (hooks.csv updated in-session)
- 16 per-RVA .md files written to this bucket
- 1 HUD state draft: `hud_state_draft.md`

## Key findings

1. **C1 note correction in 0x0040dfc0:** The `DAT_0063ba8c == 7` branch routes
   default/6/10 sub-modes to `FUN_0041ccc0` (then `FUN_0041d870` on break) — NOT
   FUN_0041b630/FUN_0041c0c0 as the C1 notes stated. FUN_0041db80 is only called in
   the `{5,6}` branch.

2. **No shared HUD pointer found:** The "common DAT_007xxxxx pointer" assumption in
   the mission does not hold. HUD state is a large BSS block accessed as direct static
   globals in the 0x0063xxxx range (~7.5 KB span). No pointer indirection observed.

3. **Array structure identified:** 4 data arrays of 2 sizes (4-entry × 0x74, 2-entry ×
   0x16c, 4-entry × 0x114, 2-entry × 0x160) dominate the HUD state block. Arrays A+B
   are used in the `{5,6}` path; Arrays C+D are used in the `{7}` path. Full struct
   fields (beyond enable flags) deferred to session 6.

4. **Type resolutions from Ghidra:**
   - Array loop guards: `int32_t` at +0x6c (Array_A) and +0x110 (Array_C)
   - Screen dimension returns in FUN_00428450: `int16_t` (short) from FUN_0042b8b0/b8c0
   - FUN_0041ded0 param_1: `undefined4` (4 bytes), caller passes int literal 0 or 1
   - FUN_0041db80 timer DAT_0063d588: `undefined4` — type `[UNCERTAIN]` (int32_t or float32)
   - All array loop pointer casts: `(int32_t)puVar1 < endpoint` pattern

5. **Camera object fields found in FUN_00403160:**
   - `*(camera_handle + 0x68)`: undefined4 — saved/restored as viewport w (float32 0.8f)
   - `*(camera_handle + 0x6c)`: undefined4 — saved/restored as viewport h (float32 0.8f)
   These were not in the C1 notes.

## Files produced

- 0x0040dfc0.md — HUD_INGAME_FN: corrected dispatch tree
- 0x00403160.md — sub-mode 0xb viewport handler; camera+0x68/+0x6c fields
- 0x0041a3e0.md — guard DAT_0063c628 → FUN_0041c2d0
- 0x0041b630.md — 4-entry loop 0x0063c8d0/0x74, int32_t+0x6c guard
- 0x0041c0c0.md — 2-entry loop 0x0063cab8/0x16c, unconditional
- 0x0041c300.md — guard DAT_0063cdbc → FUN_0041c2d0
- 0x0041ccc0.md — 4-entry loop 0x0063ce20/0x114, int32_t+0x110 guard
- 0x0041d870.md — 2-entry loop 0x0063d298/0x160, unconditional
- 0x0041db80.md — threshold dispatch + vtable objects; U-0579 body bounds
- 0x0041ded0.md — guard DAT_0063d5e8, undefined4 param passthrough
- 0x0041e850.md — guard DAT_0063d7e0 → FUN_0041e630
- 0x00426ba0.md — getter DAT_0066d704 (5 bytes)
- 0x0042f500.md — getter DAT_0067ea64 (5 bytes)
- 0x0042f6a0.md — getter DAT_0067e9fc (5 bytes)
- 0x00428450.md — spinning-coin animator; int16_t screen dims
- 0x00450b10.md — Im2D quad draw leaf; full vertex layout
- hud_state_draft.md — HUD state block layout for session 6

## Scribe sweep instructions

For each .md file (16 plates): Ghidra bookmark at RVA with confidence C2 and bucket `hud_ingame_promote_c2`. No renames needed (names unchanged from C1). No new stubs or deferred items introduced.
