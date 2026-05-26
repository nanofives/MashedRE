# FUN_004cbb70 — FrustumTestSphere C1→C2

**RVA:** 0x004cbb70
**Body:** 0x004cbb70..0x004cbbc1
**Session:** batch-render-4-s6
**Pool:** Mashed_pool14
**U-id:** U-5037

## Decompilation summary

Tests a sphere against a 6-plane frustum. Returns 1 if sphere is inside (or overlaps) all planes, 0 if outside any plane.

- At 0x004cbb70: `iVar2 = 6` (plane count); `pfVar1 = (float*)(param_1 + 0x94)` — frustum planes at offset +0x94 in the frustum context struct.
- Loop body (6 iterations, +5 floats per plane = 20 bytes):
  - At 0x004cbb7c: computes `dot = pfVar1[0]*sphere_xyz[0] + pfVar1[1]*sphere_xyz[1] + pfVar1[2]*sphere_xyz[2]`.
  - At 0x004cbb82: tests `dot - pfVar1[3] < -sphere_xyz[3]` (signed distance < -radius).
  - If true → return 0 (sphere outside this plane).
  - Else: `pfVar1 += 5`, `iVar2--`.
- If all 6 planes pass → return 1.

Plane layout at `param_1 + 0x94`: 6 planes × 5 floats = 120 bytes. Per plane: [nx, ny, nz, d, <pad>].
Sphere layout at `param_2`: [x, y, z, radius].

## Callers (1)
- `FUN_004ec130` (0x004ec130)

## Callees (0)
Leaf.

## C2 evidence
- Full decompilation read; plane-dot formula and frustum layout at offset +0x94 cited.
- Loop structure and all float offsets cited by address.
- No UNCERTAIN items.

## Line count
~18 decompiled lines — within 300-line cap.
