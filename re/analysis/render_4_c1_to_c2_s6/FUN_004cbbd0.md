# FUN_004cbbd0 — FrustumTestAABB C1→C2

**RVA:** 0x004cbbd0
**Body:** 0x004cbbd0..0x004cbc51
**Session:** batch-render-4-s6
**Pool:** Mashed_pool14
**U-id:** U-5038

## Decompilation summary

Tests an axis-aligned bounding box (AABB) against a 6-plane frustum. Returns 1 if box is not outside any plane, 0 if outside.

- At 0x004cbbd0: `pfVar1 = (float*)(param_1 + 0x94)` — frustum planes at +0x94 (same layout as FUN_004cbb70).
- Loop body (6 iterations, stride +5 floats per plane):
  - For each plane, selects the "negative vertex" of the AABB using the sign bits of each normal component:
    - `*(byte*)((int)pfVar1 + 0x12)` — sign byte for Z component; if non-zero, picks `param_2 + N*-0xc + 0x14` (min Z); else `param_2 + 0x14` (max Z). [UNCERTAIN: the sign-byte extraction at raw pfVar1+0x12 is a compact n-vertex selection technique; exact byte layout of the float requires verification.]
    - Similarly `*(byte*)(pfVar1 + 4)` for X at `+0xc`/`+0x0`; `*(byte*)((int)pfVar1 + 0x11)` for Y at `+0x10`/`+0x4`.
  - Computes `dot = pfVar1[0]*neg_x + pfVar1[1]*neg_y + pfVar1[2]*neg_z`.
  - Tests `dot > pfVar1[3]` — if true, box is outside plane → return 0.
  - Else continue.
- All 6 pass → return 1.

AABB layout at `param_2`: two vec3s at offsets [+0x0..+0x8] (min) and [+0xc..+0x14] (max). [UNCERTAIN: stride -0xc and +0x14 suggest interleaved layout not a simple min/max pair; raw values cited.]

## Callers (1)
- `FUN_004ec130` (0x004ec130)

## Callees (0)
Leaf.

## C2 evidence
- Full decompilation read; n-vertex selection and plane-dot test fully traced.
- [UNCERTAIN] items on sign-byte encoding and AABB interleaved layout noted.

## Line count
~25 decompiled lines — within 300-line cap.
