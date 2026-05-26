# FUN_004c9ad0 — D3D9 pre-reset resource release / RS cache reinit

**RVA:** 0x004c9ad0  
**Body:** 0x004c9ad0 – 0x004c9cc2  
**Status promoted:** C1 → C2 (batch-render-4-s5)  
**Confidence rubric:** C2 — decompilation read; callee/caller graph confirmed; semantics documented.

---

## Signature

```c
undefined4 FUN_004c9ad0(void);
```

Returns 1 on success.

---

## Callers

- `FUN_004c9cd0` @ 0x004c9cd0 — D3D9 device-lost/Reset handler (called when TestCooperativeLevel returns D3DERR_DEVICELOST).

## Callees

- `FUN_004cc7f0` @ 0x004cc7f0 — pool allocator
- `FUN_004ccc50` @ 0x004ccc50 — pool reset
- `FUN_004ccde0` @ 0x004ccde0 — subsystem cleanup
- `FUN_004d1d70` @ 0x004d1d70 — subsystem reset
- `FUN_004db550` @ 0x004db550 — subsystem cleanup
- `FUN_004dc8e0` @ 0x004dc8e0 — subsystem cleanup
- `FUN_004e08b0` @ 0x004e08b0 — subsystem cleanup

---

## Mechanics

Resets/invalidates all D3D9-managed render state and resource caches before a device Reset:

### 1. Render-state cache invalidation
```
DAT_006181c8 = 0xffffffff  // RS cache slot 0
DAT_006181cc = 0xffffffff  // RS cache slot 1 (clip plane)
DAT_006181d0 = 0xffffffff  // RS cache slot 2 (pixel shader)
DAT_006181d4 = 0xffffffff  // RS cache slot 3 (pixel shader const)
DAT_006181d8 = 0xffffffff  // RS cache slot 4 (material)
```

### 2. Texture/sampler state array reset
Loop `uVar2 = 0..0x3F step 0x10`:
```
DAT_007d40c0[uVar2] = 0xffffffff   // stage type
DAT_007d40c4[uVar2] = 0            // stage param 1
DAT_007d40c8[uVar2] = 0            // stage param 2
```
This is a 4-element array at `DAT_007d40c0` stride 0x10 = 16 bytes (4 DWORDs per entry), 4 entries total.

### 3. Texture transform cache
`DAT_007d4154 = 0` (matrix identity flag).  
If `DAT_007d4568 == 0`: allocate 0x40-entry pool (capacity=0xf, stride=0x10, flags=0x30411) → `FUN_004cc7f0(0x40, 0xf, 0x10, 0x30411)`. Zero `DAT_007d4158[0..0x103]`.  
Else: iterate `DAT_007d4158[0..0x40f step 4]`; for any non-zero entry call release fn `(DAT_007d3ff8+0x11c)` then clear entry; then call `FUN_004ccc50(pool)`.

### 4. Texture identity copy
Allocate new identity slot via `(DAT_007d3ff8+0x118)(DAT_007d4568, 0x30411)` → `DAT_007d4558`.  
Copy 0x10 DWORDs from `DAT_005d8b90` (identity matrix?) into the new slot.

### 5. D3D device state reset
- `SetTexture(stage 0..7, NULL)` via vtable+0x104
- `SetVertexShader(NULL)` via vtable+0x1a0
- `SetStreamSource(stream 0..3, ...)` via vtable+0x190 (decimal 400)
- `SetIndices(NULL)` via vtable+0x1ac
- `SetPixelShader(NULL)` via vtable+0x15c
- `SetFVF(0)` via vtable+0x170

### 6. Render-target/depth-stencil reset
- If `DAT_007d4578 != DAT_007d4118`: update cache and call `SetRenderTarget(0, DAT_007d4118)` (vtable+0x94)
- For stages 1..3: if cached != 0, set to NULL via `SetRenderTarget`
- If `DAT_007d4574 != DAT_007d4114`: update cache and call `SetDepthStencilSurface(DAT_007d4114)` (vtable+0x9c)

### 7. Sub-system teardowns
`FUN_004db550()`, `FUN_004e08b0()`, `FUN_004dc8e0()`, `FUN_004d1d70()`, `FUN_004ccde0()`.  
If `DAT_007d4598 != NULL`: call it (optional callback).

### Key globals

| Address | Role |
|---------|------|
| `DAT_006181c8..D8` | RS cache: clip-plane, PS, PS-const, material |
| `DAT_007d40c0` | Texture stage state array (4 entries × 3 DWORDs) |
| `DAT_007d4154` | Matrix identity flag |
| `DAT_007d4158` | Texture transform cache table (0x104 entries) |
| `DAT_007d4568` | Pool handle for texture transform entries |
| `DAT_007d4558` | Current identity matrix slot pointer |
| `DAT_005d8b90` | Identity matrix (0x10 DWORDs) |
| `DAT_007d4578..4584` | Render-target surface cache (stages 0..4) |
| `DAT_007d4574` | Depth-stencil surface cache |
| `DAT_007d4118` | Back-buffer surface ptr |
| `DAT_007d4114` | Depth-stencil surface ptr |
| `DAT_007d4598` | Optional post-reset callback fn-ptr |

---

## Notes

- Previously had C1 plate at `re/analysis/render_d3d_reset/004c9ad0.md` (S-0701, D-2020).
- No new UNCERTAINTIEs raised.
