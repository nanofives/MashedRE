# render_lighting_alt — Session FFFF (2026-05-03)
# Slot: Mashed_pool1  Strategy: 3 (shader path)

## Anchor

MASHED.exe SHA-256: BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E ✓

## Context

Session OO (render_lighting_d2) already found RpLight objects (RpLightCreate @ 0x004e4dd0,
RpWorldAddLight @ 0x004e4810) used for scene lighting in the course loader path.

This session found the **complementary shader-based lighting path** — specular environment
mapping via D3D9 camera-space reflection/normal vector generation.

## Key finding: D3DRS_LIGHTING = 0 (always off)

`FUN_004d5480(0x89, 0)` called from the D3D9 render state init function `FUN_004d5bc0`
(0x004d5bc0) at startup. D3D9 fixed-function lighting is **disabled throughout**.

`FUN_004d5480` (0x004d5480): D3D9 render state cache dirty-list writer. Args: (state, value).
Writes to `(&DAT_007d57f8)[state*2]`, marks dirty if changed, enqueues index to
`(&DAT_007d5168)[DAT_007d6c14]`.

## LIGHT_FN: FUN_00541b50 (0x00541b50)

Per-material specular environment-map setup function. Called from both render callbacks.

**Signature (inferred):** `undefined4 FUN_00541b50(undefined4 *param_1, int stage, int param_3)`
- `param_1`: geometry/atomic pointer
- `stage`: D3D9 texture stage index
- `param_3`: optional secondary object for relative transform

**Logic:**

```
iVar1 = FUN_004cff00(*param_1)   // read per-material reflectivity nibble
if iVar1 != 0:
    // Specular reflection path
    D3DTSS_TEXTURETRANSFORMFLAGS[stage] = D3DTTFF_COUNT3 (3)
    D3DTSS_TEXCOORDINDEX[stage]         = D3DTCI_CAMERASPACEREFLECTIONVECTOR (0x30000)
    pfVar2 = FUN_004c0ed0(camera)       // get camera matrix (param+0x50)
    local_c0..local_b4 = negated rows 0..1, row 2, zeros, 1.0f
    FUN_004cb330(stage + D3DTS_TEXTURE0, &local_c0)  // SetTransform + cache
else:
    // Normal-vector path
    D3DTSS_TEXTURETRANSFORMFLAGS[stage] = D3DTTFF_COUNT2 (2)
    D3DTSS_TEXCOORDINDEX[stage]         = D3DTCI_CAMERASPACENORMAL (0x10000)
    if param_3 != 0:
        FUN_004c4dc0, FUN_004c4600: compute (cam_inv * obj) matrix
        FUN_004cb330(stage + D3DTS_TEXTURE0, result)
    else:
        FUN_004cb330(stage + D3DTS_TEXTURE0, &DAT_005e4848)  // identity
```

**Returns:** always 1.

## Lighting system summary

MASHED uses two coexisting lighting systems:

1. **RpLight scene lighting** (found in session OO): RpLightCreate (rpLIGHTDIRECTIONAL,
   rpLIGHTAMBIENT) called from course loader FUN_00479330; `Lights_Filename` user data key
   registered at 0x00440c80+; drives RenderWare world illumination.

2. **D3D9 shader specular** (found in this session): Per-material specular highlight via
   camera-space reflection/normal vector generation. No D3D9 fixed-function lights (D3DRS_LIGHTING=0).
   Material "shininess" flag read via FUN_004cff00. Texture matrix set via SetTransform.

The D3DX9 shader assembler is embedded (strings at 0x005db9d4 and 0x005e2d2f).
Pixel shaders set via SetPixelShader (vtable 0x1AC), vertex shader via SetVertexShader (0x170).
FUN_00541d40 (opaque path) and FUN_005412d0 (alpha path) are the RW pipeline callbacks.

## Functions covered

| RVA        | Role                                      | Size   |
|------------|-------------------------------------------|--------|
| 0x00541b50 | LIGHT_FN: specular env-map setup          | 489 B  |
| 0x00541d40 | Opaque shader render callback (RW pipe node)| 373 B |
| 0x005412d0 | Alpha/transparent shader render callback  | 2161 B |
| 0x004d5bc0 | D3D9 render state init (D3DRS_LIGHTING=0) | ~570 B |
| 0x004d5480 | D3D9 RS dirty-list writer                 | 66 B   |
| 0x004d54f0 | Texture stage state dirty-list writer     | 89 B   |
| 0x004d6e70 | Texture binder + sampler state setter     | ~386 B |
| 0x004cff00 | Per-material reflectivity flag reader     | 17 B   |
| 0x004c0ed0 | Camera matrix accessor (returns ptr+0x50) | 30 B   |
| 0x004cb330 | Texture transform matrix cache (SetTransform)| 343 B |

## Shader constant names (registered at 0x00440c80+)

Via function 0x0047b980 (constant-to-variable binder):
- `"Ambient_RGB"` @ 0x005cde64 — per-geometry ambient color user-data key
- `"Lights_Filename"` @ 0x005cde70 — per-clump light DFF filename user-data key
- `"Modify_Fog"` @ 0x005cde4c — fog modifier flag
- `"Setup_Fog"` @ 0x005cde58 — fog setup flag

## Callee notes

- `FUN_004cff00` (0x004cff00): returns `*(byte*)(DAT_00911ae4 + 9 + param_1) & 0xf` —
  per-material nibble flag (reflectivity/shininess).
- `FUN_004c0ed0` (0x004c0ed0): checks lazy-update flag at `*(param_1+0xa0)+3`; returns
  `param_1 + 0x50` (camera LTM matrix start in RwCamera).
- `FUN_004cb330` (0x004cb330): matrix change-check + `SetTransform(device, state, matrix)`
  (vtable offset 0xb0 = IDirect3DDevice9::SetTransform); caches at `(&DAT_007d4158)[state]`.
- `FUN_004d6e70` (0x004d6e70): calls SetTexture (0x104) and SetSamplerState (0x114) for
  address wrap modes and filter modes from lookup table at 0x005d8ca8.
- `FUN_004d54f0` (0x004d54f0): TSS dirty-list writer mirroring FUN_004d5480 for render states.

## D3D9 vtable offsets confirmed

| Offset | Index | Method              |
|--------|-------|---------------------|
| 0x0b0  | 44    | SetTransform        |
| 0x104  | 65    | SetTexture          |
| 0x114  | 69    | SetSamplerState     |
| 0x144  | 81    | DrawPrimitive       |
| 0x148  | 82    | DrawIndexedPrimitive|
| 0x15c  | 87    | SetVertexDeclaration|
| 0x164  | 89    | SetFVF              |
| 0x170  | 92    | SetVertexShader     |
| 0x1ac  | 107   | SetPixelShader      |
| 0x1b4  | 109   | SetPixelShaderConstantF |

## Depth-2 DEFERRED (D=4780..4839, bucket render_lighting_alt-cont1)

Unanalyzed callees of the 10 covered functions (not needed to understand LIGHT_FN):
- FUN_004c4600 (0x004c4600): matrix operation (mult/transform) — callee of FUN_00541b50
- FUN_004c4dc0 (0x004c4dc0): matrix invert — callee of FUN_00541b50
- FUN_004d8350 (0x004d8350): lazy-update helper — callee of FUN_004c0ed0
- FUN_004d6ce0 (0x004d6ce0): called from FUN_004d6e70 (texture filtering?)
- FUN_004d53b0 (0x004d53b0): pre-draw state flush — callee of FUN_00541d40
- FUN_004d7100 (0x004d7100): alpha-test toggle — callee of FUN_00541d40
- FUN_004d71f0 (0x004d71f0): alpha-test state reader — callee of FUN_00541d40
- FUN_004dcbb0 (0x004dcbb0): material/blend lookup — callee of FUN_00541d40
- FUN_004dcd30 (0x004dcd30): blend state setter — callee of FUN_00541d40
- FUN_00540340 (0x00540340): simple draw path — callee of FUN_00541d40
- FUN_005422c0 (0x005422c0): pre-draw setup — callee of FUN_00541d40
- FUN_004ec130 (0x004ec130): called from 0x00543512 parent of both callbacks
