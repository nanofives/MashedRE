# Camera Path & RwCamera Struct Layouts

**Session:** struct_extract_phase5 (2026-05-12)
**Evidence sources:** camera_follow_d2-20260503 (27 C1 plates), camera_follow_d3-20260506 (18 C1 plates), re/analysis/camera_follow_d2/SESSION_END.md, re/analysis/camera_follow_d3/SESSION_END.md
**Confidence:** C1 — fields mechanically confirmed from decompilation across multiple plates

---

## RwCamera struct (object passed to camera functions)

Observed from FUN_004c1b10, FUN_004910c0, FUN_00491340, FUN_004c1b40.

Camera handle obtained via `FUN_004671a0(0)` (camera getter, camera_follow subsystem).

| Offset | Width | Type | First RVA | Tentative name |
|--------|-------|------|-----------|----------------|
| +0x04 | 4 | ptr | FUN_004c1b10 | Attached RwFrame ptr (optional; 0=no frame); read at `0x004c1b19` |
| +0x14 | 4 | int32 | FUN_004c1a70 body | Projection mode (2=parallel; checked by recompute fn) |
| +0x50 | 4 | float | FUN_004c1b10 body | Saved/restored view parameter (temporarily set to 0x3f4ccccd ≈ 0.8f in mode-B particle update) |
| +0x68 | 4 | float | FUN_004910c0 body | Viewport width (read as fVar3) |
| +0x6c | 4 | float | FUN_004910c0 body | Viewport height (read as fVar4) |
| +0x80 | 4 | float | FUN_004c1a70 body | Near clip plane |
| +0x84 | 4 | float | FUN_004c1b10 (write param_2) | Far clip plane or view-window height (candidate RwCameraSetFarClipPlane or SetViewWindow) |
| +0x8c | 4 | float | FUN_004c1a70 (written) | Frustum scale X (derived from near/far + viewport) |
| +0x90 | 4 | float | FUN_004c1a70 (written) | Frustum scale Y |
| +0x94 | 30×float | float[6][5] | FUN_004c1b40 loop base | 6 frustum planes; stride 5 floats (nx,ny,nz,d,padding) per plane |
| +0xa0 | 4 | ptr | FUN_004c0ed0 (reads flags byte at +3) | Attached object or sub-object ptr; bit 0 of *(+0xa0+3) = LTM-dirty flag |

`DAT_007d3ff8+0x18` (`g_rwDevice+0x18`): viewport zRecip constant read by FUN_004c1b10 for projection setup.

---

## RwFrame struct (hierarchy node, confirmed from FUN_004d8350)

Matched to RwFrameUpdateObjects pattern.

| Offset | Width | Type | First RVA | Notes |
|--------|-------|------|-----------|-------|
| +0x03 | 1 | byte | FUN_004d8350 | Flags byte; bit 1=modelling-dirty, bit 2=LTM-dirty |
| +0x04 | 4 | ptr | FUN_004d8350 child-LTM compose | Attached object ptr; used as parent world-matrix source |
| +0x10..+0x4f | 64 | float[16] | FUN_004d8350 | Local modelling matrix (4×4) |
| +0x50..+0x8f | 64 | float[16] | FUN_004d8350 (written when bit2 set) | LTM / world matrix (4×4); copied from local when dirty |
| +0x98 | 4 | ptr | FUN_004d8350 child-chain loop | First child ptr (linked list head) |
| +0x9c | 4 | ptr | FUN_004d8350 sibling iteration | Sibling ptr (next in parent's child list) |

LTM propagation: `FUN_004d8350` copies +0x10→+0x50 when bit 2 set, then recurses children via `FUN_004d83d0`.
Child LTM composition: `FUN_004c4600(child+0x50, child+0x10, parent+0x50)`.

---

## Camera path struct (param_2 in path interpolators)

Passed by pointer to FUN_004756e0, FUN_004752f0, FUN_004b47e0.

| Offset | Width | Type | First RVA | Notes |
|--------|-------|------|-----------|-------|
| +0x04 | 4 | int32 | FUN_004752f0 body | Node count |
| +0x0c | 4 | float | FUN_004756e0 body (wrap check) | Total path length (float); wrap boundary for accumulated position |
| +0x10 | 4 | ptr | FUN_004752f0 body (`local_7c = *(param_2+0x10)`) | Nodes array base ptr |

---

## Camera path node (stride 0x24 = 36 bytes)

Confirmed from FUN_004752f0 (Hermite quaternion interpolator, D-3640) and FUN_004b47e0 (spline interpolator, D-3641).

| Offset | Width | Type | Notes |
|--------|-------|------|-------|
| +0x00 | 4 | ? | Not accessed in observed functions |
| +0x04 | 4 | float | Segment start position (float along path); used for segment scan boundary in FUN_004752f0 |
| +0x08..+0x14 | 12 | float[3] | Orientation part A (first quaternion / control point) |
| +0x18..+0x23 | 12 | float[3] | Position control data; used by FUN_00482ae0 (spline position evaluator) |
| +0x28 | 4 | float | Segment end position (float along path); loop-sentinel test `*(node+0x28) >= param_3` |
| +0x2c..+0x38 | 12 | float[3] | Orientation part B (second quaternion / control point) |
| +0x3c..+0x47 | 12 | float[3] | Next curve control data |

First/last node clamping via `FUN_00532a60(param_1, first_or_last_node)` when param_3 outside [0, path_length].

---

## Camera-path intersection node (from FUN_0047ba20, D-3660)

Used by camera-path spatial queries in FUN_0047bb10 (edge-intersection + centroid).

| Offset | Width | Notes |
|--------|-------|-------|
| +0x00 | N×12 | Normal vectors (float3 each, stride 12 bytes); N = max normal index seen |
| +0x90 | N×12 | Vertex positions (float3 each, stride 12 bytes from +0x90) |
| +0xf0 | N×4 | Edge table (4 bytes per edge: byte[0]=vert idx, byte[1]=unused, byte[2]=normal A idx, byte[3]=normal B idx) |
| +0x138 | 4? | Reference point (passed to FUN_00477f00 as anchor) |
| +0x148 | 4 | Edge count (loop bound in FUN_0047bb10) |

Edge intersection test returns 1 when dot products of edge normals A and B with (point − vertex) have opposite signs.

---

## Camera-anim management tables (global arrays, 200 entries each)

| Address | Count | Element size | Notes |
|---------|-------|-------------|-------|
| `0x006c71d8` | 200 | 4 (ptr) | Inner-ref / animation-object handle table; index 0..199; getter FUN_0047d130 |
| `0x006c9438` | 200 | 1 (byte) | Camera-anim entry flags (bits 1, 2, 4 tested); getter FUN_0047ce00 |
| `0x006c9758` | 200 | 1 (byte) | Camera-anim entry ID table; getter FUN_0047ce80 (returns 0xffffffff on out-of-bounds) |

`FUN_0057c210`: animation-object pointer table dereference via `*(DAT_007dc8d8 + param_1)` (base ptr DAT_007dc8d8).

---

## Per-camera-anim entry table (FUN_00471ac0 dispatcher)

| Address | Notes |
|---------|-------|
| `DAT_006905c8` | Count of camera-anim objects |
| `DAT_0069064c` | Camera-anim object array; stride 0x23 ints per entry |
| `DAT_00690ab0` | Per-frame function table; stride 0x21 entries; indexed by (ID−2000)×0x21 |

Per-entry layout (stride 0x23 ints = 0x8c bytes, from FUN_00471ac0):

| Offset | Notes |
|--------|-------|
| `piVar10[0]` | Current frame index |
| `piVar10[1]` | Total frame count |
| `piVar10[2]` | Active flag (0 = inactive) |
| `piVar10[-4]` | [UNCERTAIN] path/mode discriminant |
| `piVar10[-6]` | Entry index (passed to FUN_0047d150 and FUN_0047d130) |

---

## Camera particle system (FUN_00491340 / FUN_004910c0)

4-layer × 224-entry particle array.

| Address | Notes |
|---------|-------|
| `DAT_00771534` | Particle system enabled flag (non-zero = active) |
| `DAT_00771530` | Particle array base; 4 layers × 0x1c00 bytes each |
| `DAT_0077152c` | Path table base; stride 12 bytes per node (respawn positions) |
| `DAT_006146b4` | Drag constant (friction/drag scale factor) |
| `DAT_007f100c` | Camera follow time constant |

Per-particle struct within each layer (stride 8 floats = 32 bytes):

| Field index | Notes |
|------------|-------|
| [0..2] | Position xyz |
| [3..5] | Velocity xyz (normalized for drag) |
| [6] | Frame counter; reset when > 7, triggers respawn |
| [7] | [UNCERTAIN U-1248] |

Scene root global: `DAT_00646e58` (returned by FUN_00426020; passed as scene/camera handle to camera-anim dispatcher).

---

## Open uncertainties

| U-ID | Gap |
|------|-----|
| U-2027 | FUN_0047d100 calls FUN_004b5240 with no visible args (may be thiscall/fastcall artifact) |
| U-2028 | DAT_007d3ff8 vtable slot at +8: candidate RwV3dTransformVector, unconfirmed |
| U-1247 | DAT_007d3ff8 vtable slot at +0x10 (FUN_004c3dc0): candidate RwV3dTransformPoints, unconfirmed |
| U-1248 | Particle struct field [7] and full layout at DAT_00771530 |
