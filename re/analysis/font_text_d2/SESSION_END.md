# SESSION_END — font_text_d2-20260503-1620

Session: font_text_d2 (depth-2 of DDD font_text)  
Slot: Mashed_pool6 (read-only)  
Completed: 2026-05-03

---

## Summary

21 functions decompiled or listed. 18 new hooks.csv rows (all C1/stub). 7 new UNCERTAINTIES (U-1487..U-1493). 14 new STUBS (S-1480..S-1485, multiple callees per row). 6 new DEFERRED rows (D-4360..D-4365).

## Key findings

- **LAB_00554940** (U-1067): 496-instruction per-string glyph renderer confirmed unrecognized. Full Im2D quad loop deferred (D-4360) pending Im2D vertex layout confirmation.
- **LAB_00555910** (U-1068): 552-instruction .met parser confirmed unrecognized. Opens file in read mode via RW-VFS (FUN_005507b0, "r"). METRICS section parsers deferred (D-4361).
- **FUN_00552840** resolved (S-1064): SetFontRotation — Z-axis rotation in degrees via FUN_004c4d20 (π/180 at 0x005cd7a8=0x3c8efa35 confirmed).
- **FUN_00427680**: ESI = implicit output float[2] (screen xy). New U-1487.
- **Font init chain** (FUN_00552b60 → 7 sub-fns) fully traced: allocates Im2D vertex buffer (0x2400=384 verts × 24 bytes), scratch buffer (256 bytes), style pool (0x78 bytes), two plugin objects, identity matrix, camera viewport.
- **FUN_005507b0**: RW virtual filesystem open — parses mode flags "r"/"w"/"b"/"a"/"+" from strings at 0x617378/0x623dd8..de4; dispatches via linked list at DAT_007dc754.
- **FUN_00553f40**: Im2D vertex lock — copies stride-0x18 (24-byte) vertex data, recurses on linked list.

## IDs used

- U-1487..U-1493 (7 of 20 allocated)
- S-1480..S-1485 (6 rows, multiple entries; 14 callee entries total; 20 of 20 allocated slots have headroom)
- D-4360..D-4365 (6 of 60 allocated)

## Open items for depth-3

| Priority | Target | What | Bucket |
|----------|--------|------|--------|
| HIGH | 0x005551d0 FUN_005551d0 | U-1490: entry guard for LAB_00555910; D-4362 | font_text_d2-cont1 |
| HIGH | LAB_00554940 per-char loop (0x00554a5b+) | Im2D quad emission; needs Im2D vertex struct layout | font_text_d2-cont1 |
| HIGH | LAB_00555910 METRICS parsers (0x00555a04+) | .met format; need font36.piz extraction first | font_text_d2-cont1 |
| MED | FUN_00552df0, FUN_00552d10, FUN_00552da0, FUN_00552e40 | Im2D render-state setters; D-4363 | im2d_render_d3 |
| MED | FUN_004c4a50 | angle-axis matrix builder; D-4364 | render_pipeline_d3 |
| LOW | FUN_00550580 | VFS file-open impl; D-4365 | vfs_io_d3 |

## Depth-3 prompt (font_text_d2-cont1)

Recommended slot: Mashed_pool6 or any free pool slot.  
Bucket: `re/analysis/font_text_d2/`  
ID-range: U=1494..1506  S=1486..1499  D=4366..4419  
Subsystem: hud

Targets:
1. Decompile FUN_005551d0 (0x005551d0) — resolve U-1490.
2. Listing continuation of LAB_00554940 from 0x00554a5b (per-char Im2D loop body).
3. Listing continuation of LAB_00555910 from 0x00555a04 (METRICS1/2/3 parsers) — requires font36.piz extraction first (use /piz skill).
4. Decompile FUN_00552df0, FUN_00552d10, FUN_00552da0, FUN_00552e40 (Im2D state setters).
5. Confirm U-1487 (ESI implicit in FUN_00427680) via callers listing.
6. Confirm U-1488 (double FUN_00553f40 call in FUN_005540d0) via listing.
7. Confirm U-1493 (FUN_004c57a0 EAX return) via epilogue listing.
