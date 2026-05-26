# FUN_004b5af0 — C1→C2 plate

**RVA:** 0x004b5af0  
**Body:** 0x004b5af0–0x004b5d84  
**Batch:** batch-render-3-s5  
**U-id:** U-4803  
**Date:** 2026-05-26  

## Signature (Ghidra)

```c
void FUN_004b5af0(float *param_1, undefined1 *param_2, float param_3)
```

## Mechanical description

Renders a 3D axis-aligned cross/star (3 orthogonal line segments) using RW Im3D primitives.

- `param_1` — 4×4 matrix (float[16]), column-major: `param_1[0xC..0xE]` is translation (X,Y,Z), rows 0,1,2 are axis vectors.
- `param_2` — RGBA color (4 bytes); if NULL uses default red `{0xFF,0,0,0xFF}`.
- `param_3` — scale factor.
- Builds 6 vertices (±scale along each of 3 axes from translation):
  - `local_d8..local_ac`: X-axis segment (translation ± scale×col0)
  - `local_6c..local_64`: Y-axis segment (translation ± scale×col1)
  - `local_24..local_1c`: Z-axis segment (translation ± scale×col2)
- Copies RGBA into each vertex color via CONCAT31.
- Alpha blend flag: if `param_2 != NULL && param_2[3] != -1` → use `0x10`, else `0x12`.
- Calls `DAT_007d3ff8+0x20` with `(0xC,1)` if alpha enabled.
- Calls `DAT_007d3ff8+0x20` with `(1,0)`.
- Calls `FUN_004cd070` with `(&local_d8, 6, 0, uVar3)` — submit 6 verts.
- If nonzero: `FUN_004cd430(0,1)`, `FUN_004cd430(2,3)`, `FUN_004cd430(4,5)`, then `FUN_004cd140()`.
- Restores alpha blend if enabled.

## Key constants / addresses

| Address | Value/Role |
|---------|-----------|
| param_1[0xC] | translation X |
| param_1[0xD] | translation Y |
| param_1[0xE] | translation Z |
| default color | `{0xFF,0,0,0xFF}` (red, opaque) |
| `FUN_004cd070` | Im3D primitive begin |
| `FUN_004cd430` | line index pair |
| `FUN_004cd140` | Im3D primitive end/render |

## Callers

- `FUN_004270f0` (0x004270f0)
- `FUN_00483a70` (0x00483a70)

## Callees

- `FUN_004cd070` (0x004cd070)
- `FUN_004cd140` (0x004cd140)
- `FUN_004cd430` (0x004cd430)

## Line count

~95 lines — within 300-line cap.

## C2 promotion rationale

Fully legible: 3-axis star/cross draw at matrix translation with scale, 6-vertex Im3D line-list, 3 index pairs. All offsets cited.
