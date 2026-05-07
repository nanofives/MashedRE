# SESSION_END — render_d3d_reset_d2-20260503-0351

**Date:** 2026-05-03  
**Pool slot used:** Mashed_pool3 (session 0da6a338773a451aa0105c735620d487)  
**Note:** Mashed_pool2 had stale Ghidra lock; used already-open pool3 session instead.

## Functions analyzed (14 total — all C1)

| RVA | Role |
|-----|------|
| 0x004cc7f0 | VD wrapper → FUN_004cc820 with literal flags 1/0 |
| 0x004ccc50 | VB pool walker; frees empty nodes; promotes non-empty to head |
| 0x004ccde0 | Outer driver over DAT_007d45cc list → FUN_004ccc50 |
| 0x004d1d70 | D3D resource release by type-byte (list DAT_007d46e0, array DAT_00911ae4) |
| 0x004db550 | Pre-reset release: VD handle + 2 D3D resource ptrs |
| 0x004dc8e0 | 4-slot render-target teardown array |
| 0x004e08b0 | 1 D3D release + 3 VD cache releases |
| 0x004dc9e0 | VB re-create after reset; 256KB slots; 1 or 4 by cap flag |
| 0x004cb8a0 | Vertex-declaration create+cache (D3DDECL_END=0x11, CreateVertexDeclaration=vtable+0x158) |
| 0x004cba80 | VD cache release (searches by ptr, calls COM Release) |
| 0x004d5480 | Deferred SetRenderState setter (desired/dirty arrays) |
| 0x004d54f0 | Deferred SetTextureStageState setter (stride 0x21=33/stage) |
| 0x004d5570 | Direct SetSamplerState setter (stride 0xe=14/sampler; vtable+0x114) |
| 0x004d53b0 | RS+TSS state flush (drains dirty queues; shadows at DAT_007d54b0/DAT_007d5e88) |

## Key findings

- **D3D device pointer confirmed:** `DAT_007d4110` = `IDirect3DDevice9*`  
  - vtable+0x0e4 (slot 57) = `SetRenderState`  
  - vtable+0x10c (slot 67) = `SetTextureStageState`  
  - vtable+0x114 (slot 69) = `SetSamplerState`  
  - vtable+0x158 (slot 86) = `CreateVertexDeclaration`

- **Two-layer deferred state system:** desired-value cache → dirty queue → shadow compare → D3D call (SetRenderState / SetTextureStageState).

- **VD cache:** `DAT_007d414c` (0xc-byte entries), `DAT_007d4144` (count), `DAT_007d4148` (capacity).

- **Capability flag:** `DAT_00911fbc & 0x10000` gates 1-slot vs 4-slot VB/RT allocation. Written by FUN_004cfa00 (D3D format-capability table builder).

## Deferred (depth-3)
- D-3400: FUN_004cc820 (S-1160)
- D-3401: FUN_004dcaa0 (S-1161)

## Open uncertainties
- U-1167: type-byte encoding in FUN_004d1d70
- U-1168: DAT_007d4114 sentinel role
- U-1169: DAT_007d7098 guard flag in FUN_004db550
- U-1170: DAT_00911fbc & 0x10000 capability meaning
- U-1171: FUN_004cc820 flag args (1, 0)

## Trackers mutated
- hooks.csv: +14 rows
- STUBS.md: +4 rows (S-1160..S-1163)
- UNCERTAINTIES.md: +5 rows (U-1167..U-1171)
- DEFERRED.md: closed D-2020..D-2023; added D-3400..D-3401
