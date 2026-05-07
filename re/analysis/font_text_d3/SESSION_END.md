# font_text_d3 SESSION_END

Session: font_text_d3-20260506  
Slot: Mashed_pool14 — RELEASED  
Ended: 2026-05-06

## Summary

**Resolved from d2:** U-1487 U-1488 U-1490 U-1491 U-1492 U-1493 (all closed)  
**D-4362 resolved:** FUN_005551d0 = FontCtx_Alloc  
**D-4363 resolved:** FUN_00552d10/df0/da0/e40 = FontMatrix_Push/SetTranslation/SetScale/FlushMatrix  
**RW SDK confirmed:** FUN_004c5010 = RwMatrixScale (C2), FUN_004c51a0 = RwMatrixTranslate (C2)  
**D-4360 partial:** Im2D vertex stride=36, vertex[+0x18]=ARGB, [+0x1c]=U, [+0x20]=V; glyph entry stride=32, [+0x04]=tex, [+0x08]=u_left, [+0x0c]=v_top, [+0x14]=v_bottom

**New in trackers:**  
- hooks.csv: +7 rows (FontCtx_Alloc, FontMatrix_Push, FontCtx_SetTranslation, FontCtx_SetScale, FontCtx_FlushMatrix, RwMatrixScale, RwMatrixTranslate)  
- STUBS.md: +8 rows (S-2120..S-2127)  
- UNCERTAINTIES.md: +8 rows (U-2127..U-2134)  
- DEFERRED.md: +11 rows (D-6280..D-6290)

## Queue for depth-4

Bucket: font_text_d3-cont1 (i.e., font_text_d4)  
Priority D range: D-6280..D-6290  
Top items:
1. D-6280: LAB_00554940 remaining loop (0x00554df0..0x00555130) — resolve U-2128/2129/2130/2133
2. D-6287: FUN_00552d70 FontMatrix_Pop (quick decomp)
3. D-6289: FUN_005c4ad0 glyph buffer alloc (quick decomp)
4. D-6290: ESI prologue trace in LAB_00554940 (listing 0x00554940..0x00554965)
5. D-6283: Vertex fields +0x0c..+0x17 (listing 0x00554b70..0x00554b82)
