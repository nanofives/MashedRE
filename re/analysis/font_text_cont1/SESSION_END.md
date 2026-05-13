# Session END — font_text_cont1-20260512

Session ID: font_text_cont1-20260512
Slot: Mashed_pool1 (released)
SHA-256 anchor: BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E ✓

## Summary

Drained 13 DEFERRED rows (D-3100..D-3112, all tagged `font_text-cont1`).
- 4 drift-skipped (D-3100/3101/3102 already C2; D-3103 already C1; D-3107 existing vehicle C1; D-3109 existing render C2)
- 7 primary D-rows resolved by new plates
- 6 bonus depth-1 teardown callees of FUN_00552b90 plated (filled cap to 13 total)

## Plates written (13)

| RVA | Name | Role |
|-----|------|------|
| 0x005571e0 | FontSys_InitFontPool | RwFreeList 256B entries → DAT_00912a2c |
| 0x00557250 | FontSys_InitDataPools | Two RwFreeList (8B + 20B) → DAT_00912a34/a38 |
| 0x00556d20 | FontSys_InitBuffers | Vtx buf 0x2400 → DAT_00912a04; RwFreeList 120B → DAT_00912a00 |
| 0x005507b0 | VFS_Open | Mode-string parser + prefix-dispatch file open |
| 0x00555af0 | FontCtx_LoadMetrics_Met | .met text-file glyph metrics loader |
| 0x00555ff0 | FontCtx_LoadMetrics_Atlas | Bitmap atlas glyph metrics scanner (METRICS3) |
| 0x00552b90 | FontSys_Shutdown | Full font subsystem teardown |
| 0x005571c0 | FontSys_ShutdownFontPool | Counterpart to FontSys_InitFontPool |
| 0x00557220 | FontSys_ShutdownDataPools | Counterpart to FontSys_InitDataPools |
| 0x00556ce0 | FontSys_ShutdownBuffers | Counterpart to FontSys_InitBuffers |
| 0x00554050 | FontCanvas_Shutdown | Counterpart to FontCanvas_Init |
| 0x00555280 | FontSys_ShutdownContextPool | Multi-pool font context teardown |
| 0x00555f20 | FontCtx_BuildExtTable | Post-metrics ext-char lookup table builder |

## New stubs: S-3690..S-3703 (14 total)

VFS_Open_Impl (S-3690), RwTexDictionaryGetCurrent (S-3691), RwTexDictionaryFindNamedTexture (S-3692), FontStreamReset? (S-3693), VFS_ReadLine (S-3694), GlyphBuf_Resize (S-3695), GlyphBuf_GetBase (S-3696), RwImageCreate (S-3697), RwImageDestroy (S-3698), tex-to-image bind (S-3699), GlyphBuf_SpecialFree (S-3700), FontChain_FreeList (S-3701), FontCtx_Free (S-3702), FontObj_Free (S-3703)

## New uncertainties: U-3684..U-3685

- U-3684: DAT_00912a00 120-byte pool entry struct not identified
- U-3685: VFS_Open flag bit semantics vs VFS_Open_Impl

## Key findings

- **Glyph entry layout confirmed** (0x20-stride): `[+0x00]` advance ratio (float), `[+0x04]` tex handle, `[+0x08]` u_left, `[+0x0c]` v_top, `[+0x10]` u_right, `[+0x14]` v_bottom, `[+0x1c]` page-index byte
- **Extended char table** at `ctx+0x124/128/12c` (base/size/ptr); built by FontCtx_BuildExtTable using temp global DAT_0091325c
- **Vertex buffer origin** confirmed: DAT_00912a04 = 9216 bytes allocated by FUN_00556d20 (FontSys_InitBuffers)
- **Two metrics paths**: text (.met file via VFS_ReadLine) vs bitmap (atlas scan via RwImage)
- **Font init chain** now fully mapped: InitDataPools(pos1) → InitGlyphNode(pos2) → InitBuffers(pos3) → ? (pos4) → InitFontPool(pos5) → InitRenderState(pos6) → SetActiveCamera(pos7)
- **S-1484 / S-2129** (VFS_Open_Impl) now have a caller plate (FUN_005507b0 = VFS_Open)
