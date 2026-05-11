# Shared RwIm2D Vertex Buffer — `DAT_00898a20`

**Produced by:** Session 6 (frontend_struct_extract), 2026-05-11  
**Pool slot:** Mashed_pool5  
**Source plates:** hud_frontend/0x00472c60.md, 0x00472dc0.md, 0x004739f0.md, 0x00473540.md, 0x0042aae0.md, 0x0043c5b0.md; hud_ingame_d3/0x00450b10.md

---

## Overview

A statically allocated `RwIm2DVertex[4]` buffer at 0x00898a20, shared across all frontend 2D draw functions.  
All four Im2D draw helpers (FUN_00472c60, FUN_00472dc0, FUN_004739f0, FUN_00473540, FUN_00473c20, FUN_00450b10) write into this buffer before issuing `RwIm2DRenderPrimitive`.

RenderWare `RwIm2DVertex` layout (28 bytes = 7 floats):

| Field | Offset | Type | Notes |
|-------|--------|------|-------|
| X | +0 | float | Screen X coordinate |
| Y | +4 | float | Screen Y coordinate |
| Z | +8 | float | Reciprocal Z (= `*(DAT_007d3ff8 + 0x18)`) |
| RHW | +12 | float | Reciprocal homogeneous W (= 1.0f = 0x3f800000) |
| color | +16 | uint32 ARGB | Packed ARGB |
| U | +20 | float | Texture U coordinate |
| V | +24 | float | Texture V coordinate |

---

## Buffer layout at 0x00898a20

Each vertex is 0x1c (28) bytes. Buffer spans 4 × 0x1c = 0x70 bytes.

### Vertex 0 (base 0x00898a20)

| Address | Field | Observed writes / RVAs |
|---------|-------|----------------------|
| 0x00898a20 | X | Written by FUN_00472c60, FUN_00472dc0, FUN_00473540 |
| 0x00898a24 | Y | Written by FUN_00472c60, FUN_00472dc0 |
| 0x00898a28 | Z | `*(DAT_007d3ff8+0x18)` — written by FUN_00472c60 (~0x00472ca3), FUN_00472dc0, FUN_00473540 |
| 0x00898a2c | RHW | 0x3f800000 (1.0f) — written by FUN_00472c60, FUN_00473540 |
| 0x00898a30 | color | ARGB V0; byte-swap formula in FUN_00473540 (0x00473540): `(((p5._2_2_ >> 8) << 8 \| p5&0xff) << 8 \| (p5>>8&0xff)) << 8 \| p5._2_2_&0xff`. Mirror-set: `DAT_00898a68 = DAT_00898a30` (V2=V0). |
| 0x00898a34 | U | param_7 in FUN_004739f0 (V0 U coord) |
| 0x00898a38 | V | param_9 in FUN_004739f0 (V0 V coord) |

### Vertex 1 (base 0x00898a3c)

| Address | Field | Observed writes / RVAs |
|---------|-------|----------------------|
| 0x00898a3c | X | Written by FUN_00472c60, FUN_00473540 |
| 0x00898a40 | Y | Written by FUN_00472c60, FUN_00473540 |
| 0x00898a44 | Z | `*(DAT_007d3ff8+0x18)` |
| 0x00898a48 | RHW | 0x3f800000 |
| 0x00898a4c | color | ARGB V1; alternate byte-swap formula in FUN_00473540 (alpha byte excluded). Mirror-set: `DAT_00898a84 = DAT_00898a4c` (V3=V1). |
| 0x00898a50 | U | param_8 in FUN_004739f0 (V1 U coord) |
| 0x00898a54 | V | param_9 in FUN_004739f0 (V1 V coord) |

### Vertex 2 (base 0x00898a58)

| Address | Field | Observed writes / RVAs |
|---------|-------|----------------------|
| 0x00898a58 | X | Written by FUN_00472c60, FUN_00472dc0 |
| 0x00898a5c | Y | `FUN_0042b8c0() * param_6 * _DAT_005cc560` in FUN_00472dc0 |
| 0x00898a60 | Z | `*(DAT_007d3ff8+0x18)` |
| 0x00898a64 | RHW | 0x3f800000 |
| 0x00898a68 | color | ARGB V2 = copy of V0 color (`= DAT_00898a30`) |
| 0x00898a6c | U | param_7 in FUN_004739f0 (V2 U) |
| 0x00898a70 | V | param_10 in FUN_004739f0 (V2 V) |

### Vertex 3 (base 0x00898a74)

| Address | Field | Observed writes / RVAs |
|---------|-------|----------------------|
| 0x00898a74 | X | Written by FUN_00473540 |
| 0x00898a78 | Y | Written by FUN_00473540 |
| 0x00898a7c | Z | `*(DAT_007d3ff8+0x18)` |
| 0x00898a80 | RHW | 0x3f800000 |
| 0x00898a84 | color | ARGB V3 = copy of V1 color (`= DAT_00898a4c`) |
| 0x00898a88 | U | param_8 in FUN_004739f0 (V3 U) |
| 0x00898a8c | V | param_10 in FUN_004739f0 (V3 V) |

---

## Memory after the buffer (0x00898a90–)

| Address | Name | Notes |
|---------|------|-------|
| 0x00898a90..0x00898aeb | [uncharacterized] | 91 bytes between buffer end and menu-item table start |
| 0x00898aec | `g_menuItemTable` | Menu-item table; stride 0xd dwords = 52 bytes/entry; iterates until 0x00899104. Used by FUN_0043c5b0. |
| 0x00898aa0 | `g_playerIconIdxArray` | Stride unknown; `(&DAT_00898aa0)[iVar6 * 0x13]` used for icon-ID lookup in FUN_0042fe90. |
| 0x00898aa4 | `g_animFrameArray` | `(&DAT_00898aa4)[iVar6 * 0xd]` — per-element animation Y position in FUN_00431240. |
| 0x00898ab4 | `g_texturePtrArray` | `(&DAT_00898ab4)[iVar6 * 0x34]` — per-element texture pointer in FUN_00431240 (flag==1 path). |

---

## RW Device vtable pointer (`DAT_007d3ff8`)

| Field | Offset | Notes |
|-------|--------|-------|
| `g_rwDevice` | 0x007d3ff8 | Global pointer to RW device/engine object. All Im2D and render-state calls route through this. |
| `+0x18` | device+0x18 | Device-level Z_recip constant; written into every vertex Z field before draw. |
| `+0x20` | device+0x20 | vtable slot → render-state setter: args `(stateIndex, value)`. Observed: states 1, 6, 8, 9, 10, 11, 12, 20. |
| `+0x2c` | device+0x2c | vtable slot → DrawIndexed (textured quad, 2 tris); used by FUN_004b5750 [STUB S-3161]. |
| `+0x30` | device+0x30 | vtable slot → `RwIm2DRenderPrimitive(primType, verts_ptr, count)`. [UNCERTAIN U-0455] |

---

## Uncertainties

- [UNCERTAIN U-0454] Vertex struct fields +12..+24 (UV or padding): for FUN_0042aae0's vertex buffer at 0x0067ec30, offsets +12..+24 are not written within the function and inherit prior state. Meaning of those fields for the secondary buffer is unknown.
- [UNCERTAIN U-0455] `DAT_007d3ff8+0x30` with args `(4, ptr, 4)`: consistent with `RwIm2DRenderPrimitive(rwPRIMTYPETRISTRIP, verts, 4)` by librw analogy but not confirmed against vtable target disassembly.
- [UNCERTAIN U-0457] FUN_00473540: two different ARGB byte-extraction patterns for V0 vs V1 colors produce different values from same `param_5`. Semantic difference (alpha ramp, gradient, partial transparency) unknown without caller context.
