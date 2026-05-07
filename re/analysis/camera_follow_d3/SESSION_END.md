# SESSION_END — camera_follow_d3

**Session ID:** camera_follow_d3-20260506  
**Pool slot:** Mashed_pool9  
**Session type:** depth-3 RE sweep for camera_follow bucket  
**ID ranges in scope:** U=2027..2046, D=5980..6039, S=2020..2039  
**Parent bucket:** camera_follow_d2-cont1 (D-3640..D-3662)  
**Date:** 2026-05-06  

---

## Pre-flight results

- SHA-256 anchor: MATCH — BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
- Pool slot Mashed_pool9: existed, no active .lock; mtime updated to satisfy staleness guard
- Ghidra MCP: health_ping OK, ghidra_info OK (Ghidra 12.0.3)
- Project opened READ-ONLY: session_id=89051540f52548fc898cc44de1220c42

### d2-cont1 pre-check

Before analyzing, confirmed which D-3640..D-3662 entries from camera_follow_d2 had already been
resolved by other sessions (hooks.csv check):

| ID | RVA | Status before this session |
|---|---|---|
| D-3642 | 0x004e66d0 | Not yet in hooks.csv → analyze |
| D-3640 | 0x004752f0 | Not yet → analyze |
| D-3641 | 0x004b47e0 | Not yet → analyze |
| D-3643 | 0x004c0e50 | C1 (hud_ingame_d2) — skip |
| D-3644 | 0x0046c7b0 | C1 (race_results, ai_update_d3) — skip |
| D-3645 | 0x0046cbb0 | C1 (ai_update_d3, vehicle_damage_d2) — skip |
| D-3646 | 0x0046d4a0 | C1 (ai_update_d2) — skip |
| D-3647 | 0x004c3ac0 | C1 (ai_update_d2) — skip |
| D-3648 | 0x004d8350 | Not yet → analyze |
| D-3649 | DAT_00771530 | Data item, not a function — skip |
| D-3650 | 0x00472650 | C1 (effects_particle, ai_update_d2) — skip |
| D-3651 | 0x00426020 | Not yet → analyze |
| D-3652 | 0x0047d080 | Not yet → analyze |
| D-3653 | 0x0047d100 | Not yet → analyze |
| D-3654 | 0x0047d150 | Not yet → analyze |
| D-3655 | 0x004c4dc0 | Not yet → analyze |
| D-3656 | 0x004c3d60 | Not yet → analyze |
| D-3657 | 0x0055b650 | Not yet → analyze |
| D-3658 | 0x0055c4a0 | Not yet → analyze |
| D-3659 | 0x0055ac50 + 0x0055ac00 + 0x0055ad30 | Not yet → analyze all three |
| D-3660 | 0x0047ba20 | Not yet → analyze |
| D-3661 | 0x00477e60 | Not yet → analyze |
| D-3662 | 0x00477f00 | Not yet → analyze |

---

## Functions Analyzed

### D-3652 — FUN_0047d080 (0x0047d080)
**Body:** 0x0047d080..0x0047d0e8 (0x68 bytes)  
**Decompilation (literal):**
```c
void FUN_0047d080(int param_1, int param_2)
{
  if (((-1 < param_1) && (param_1 < 200)) && ((&DAT_006c71d8)[param_1] != 0)) {
    iVar1 = FUN_0057c210((&DAT_006c71d8)[param_1]);  // anim object ptr from inner-ref
    uVar2 = FUN_0055dec0(DAT_006ce274);               // context/camera object
    if (param_2 == 0) {
      if (*(int*)(iVar1 + 0x24) != 0) {
        FUN_00559ee0(uVar2, iVar1);                   // show: called when +0x24 != 0
      }
    } else if (*(int*)(iVar1 + 0x24) == 0) {
      *(uint*)(iVar1 + 8) = *(uint*)(iVar1 + 8) & 0xfffffff3;  // clear bits 2-3 of +8
      FUN_00559c40(uVar2, iVar1, 0xffffffff, 7);                // hide: 4 args
    }
  }
}
```
**Analysis:** Bounds-checks param_1 (0..199), reads inner-ref from DAT_006c71d8 (same table as FUN_0047d130), resolves anim object via FUN_0057c210. Gets context from DAT_006ce274 via FUN_0055dec0. param_2==0 path: calls FUN_00559ee0 when anim_obj+0x24 != 0. param_2!=0 path: clears bits 2-3 of anim_obj+8, then calls FUN_00559c40 with 4 args (0xffffffff, 7) when +0x24 == 0. The gate on +0x24 is an active/shown state flag.  
**Classification: C1** — cam-anim show/hide by index: param_2==0 shows via FUN_00559ee0, param_2!=0 hides via FUN_00559c40 with hard-coded mask args; gates on anim_obj+0x24 state.  
**New DEFERRED D-5980:** FUN_00559ee0 (0x00559ee0) — show/activate anim object; args (context, anim_obj).  
**New DEFERRED D-5981:** FUN_00559c40 (0x00559c40) — hide/deactivate anim object; 4 args (context, anim_obj, 0xffffffff, 7).

---

### D-3653 — FUN_0047d100 (0x0047d100)
**Body:** 0x0047d100..0x0047d123 (0x23 bytes)  
**Decompilation (literal):**
```c
void FUN_0047d100(int param_1)
{
  if (((-1 < param_1) && (param_1 < 200)) && ((&DAT_006c71d8)[param_1] != 0)) {
    FUN_004b5240();
  }
}
```
**Analysis:** Bounds-check + inner-ref non-null gate (same pattern as D-3652), then calls FUN_004b5240 with no apparent arguments. The absence of args is [UNCERTAIN] — may be a Ghidra calling-convention artifact (thiscall/fastcall registers not shown). The anim object resolved from DAT_006c71d8[param_1] is not visibly passed to FUN_004b5240.  
**Classification: C1** — cam-anim entry null-check gate; calls FUN_004b5240 when entry is live; possible register arg pass not visible in decompilation.  
**New DEFERRED D-5982:** FUN_004b5240 (0x004b5240) — called after cam-anim null guard; zero visible args in decompilation; role unknown.  
**[UNCERTAIN] U-2027:** FUN_0047d100 calls FUN_004b5240 with no visible args; Ghidra may have missed register arguments due to unknown calling convention. Resolution: listing_disassemble_function 0x0047d100 to check argument setup before CALL instruction.

---

### D-3654 — FUN_0047d150 (0x0047d150)
**Body:** 0x0047d150..0x0047d178 (0x28 bytes)  
**Decompilation (literal):**
```c
undefined4 FUN_0047d150(int param_1)
{
  if ((-1 < param_1) && (param_1 < 200)) {
    iVar1 = FUN_0057c210((&DAT_006c71d8)[param_1]);
    return *(undefined4*)(*(int*)(iVar1 + 0x10) + 8);
  }
  return 0;
}
```
**Analysis:** Bounds-check, resolves anim object via FUN_0057c210, returns *(*(anim_obj+0x10) + 8): double-deref — field at anim_obj+0x10 is a sub-pointer, and +8 from that sub-object is the returned value. Sibling of FUN_0047d130 (returns inner-ref directly) but indirects through the anim object's +0x10 sub-pointer.  
**Classification: C1** — cam-anim entry double-deref getter: returns *(*(anim_obj+0x10) + 8) for index 0..199.  
**No new DEFERRED.**

---

### D-3651 — FUN_00426020 (0x00426020)
**Body:** 0x00426020..0x00426025 (5 bytes)  
**Decompilation (literal):**
```c
undefined* FUN_00426020(void) { return &DAT_00646e58; }
```
**Analysis:** Zero-argument getter returning the address of DAT_00646e58 (0x00646e58). Called by FUN_00471ac0 (camera-anim trigger dispatcher) as "gets current scene/camera object." DAT_00646e58 is a global scene/camera object or scene root — the address itself is passed as a handle.  
**Classification: C1** — scene-root address getter: returns &DAT_00646e58 (0x00646e58); used by camera-anim dispatcher as scene/camera handle.  
**No new DEFERRED.**

---

### D-3642 — FUN_004e66d0 (0x004e66d0)
**Body:** 0x004e66d0..0x004e6708 (0x38 bytes)  
**Decompilation (literal):**
```c
int FUN_004e66d0(int param_1, code *param_2, undefined4 param_3)
{
  puVar2 = *(undefined4**)(param_1 + 8);
  do {
    if (puVar2 == (undefined4*)(param_1 + 8)) {
      return param_1;                                 // sentinel: list exhausted
    }
    puVar1 = (undefined4*)*puVar2;                    // save next link
    iVar3 = (*param_2)(puVar2 - 0x10, param_3);      // callback(element, param_3)
    puVar2 = puVar1;
  } while (iVar3 != 0);                              // stop when callback returns 0
  return param_1;
}
```
**Analysis:** RW circular linked-list ForAll traversal. param_1 is the list container; param_1+8 is the sentinel (circular list head stored at +8). Loop: reads next link from puVar2[0], calls param_2(puVar2-0x10, param_3) where puVar2-0x10 is the element start (link embedded at element+0x10), stops when callback returns 0. Standard RwLinkListForAllObjects / RpClumpForAllGeometries pattern. Callers (FUN_004b5580, FUN_004b5320, FUN_00474d60) pass LAB_* code labels as param_2 — these are iterator callbacks.  
**Classification: C1** — RW linked-list ForAll traversal: iterates circular list at param_1+8; calls param_2(element, param_3) per node (link at element+0x10); stops when callback returns 0. Matches RwLinkListForAllObjects.  
**No new DEFERRED** (param_2 is caller-provided callback; no fixed callees).

---

### D-3640 — FUN_004752f0 (0x004752f0)
**Body:** 0x004752f0..0x004756da (0x3ea bytes = 1002 bytes)  
**Decompilation (key literals):**
```c
// param_1 = out 4x4 matrix (16 floats); param_2 = path struct; param_3 = position along path
iVar1 = *(int*)(param_2 + 4);        // node count
local_7c = *(int*)(param_2 + 0x10);  // nodes array ptr
// Guard: param_3 < DAT_005d757c (near-zero ε at 0x005d757c)
//   → FUN_00532a60(param_1, local_7c): copy first node transform to output
// Guard: param_3 >= *(float*)(param_2 + 0xc) (total path length at +0xc)
//   → FUN_00532a60(param_1, last_node): copy last node transform to output
// Middle path: linear scan for segment where *(float*)(iVar9 + 0x28) >= param_3
//   Node stride: 0x24 (36 bytes)
//   Computes t = (param_3 - node[+4]) / (node[+0x28] - node[+4])  (local param in [0,1])
//   Debug assert: FUN_004752d0("tween is less than 0.0f!!\n"), FUN_004752d0("tween is greater than 1.0f!!\n")
//   FUN_00546e70(node+8, node+0x2c, &fStack_28): quaternion dot-product/slerp prep
//   Polynomial: cubic Hermite basis on fVar3 = (1-t)*fStack_8, fVar4 = t*fStack_8
//     fStack_38..fStack_2c = blended quaternion [x,y,z,w]
//   FUN_00482ae0(t, auStack_44, iVar6+0x18, iVar9+0x18, iVar9+0x3c, local_7c+0x18): spline position
//   Quaternion→rotation matrix: writes param_1[0..10] from quat components
//   FUN_004c51a0(param_1, auStack_44, 2): compose transform with position block
```
**Path node layout** (stride 0x24 = 36 bytes):
- +0: ? (4B)
- +4: segment start position (float)
- +8..+14: orientation start (quaternion or 3-float part A)
- +18: curve control data (3 floats; used by FUN_00482ae0 for spline position)
- +28: segment end position (float) — loop boundary test
- +2c..+38: orientation end (quaternion or 3-float part B)
- +3c: next curve control data

**Analysis:** Camera path segment interpolator, mode 0 (cubic/quaternion). Finds the path segment containing param_3 by linear scan; computes local t in [0,1]; evaluates Hermite polynomial on two quaternion control points from adjacent nodes; converts interpolated quaternion to 3×3 rotation matrix; composites with spline position via FUN_004c51a0. Clamps to first/last node when param_3 is outside [0, path_length].  
**Classification: C1** — camera path Hermite quaternion interpolator (mode 0): scans segment, blends quaternions, writes 4x4 rotation+position matrix to param_1.  
**New DEFERRED (>3 callees → camera_follow_d3-cont1):**
- D-5983: FUN_00532a60 (0x00532a60) — copies 64-byte transform block from node to output; path-node matrix copy
- D-5984: FUN_00546e70 (0x00546e70) — quaternion dot-product/slerp prep; args (quat_a, quat_b, &out_float)
- D-5985: FUN_00482ae0 (0x00482ae0) — spline position evaluator; args (t, out_pos, ctrl_a, seg_start, seg_end, ctrl_b)
- D-5986: FUN_004c51a0 (0x004c51a0) — transform composition; args (matrix, position_block, mode=2)

---

### D-3641 — FUN_004b47e0 (0x004b47e0)
**Body:** 0x004b47e0..0x004b487e (0x9e bytes)  
**Decompilation (literal):**
```c
void FUN_004b47e0(undefined4 param_1, int param_2, float param_3)
{
  iVar1 = *(int*)(param_2 + 0x10);          // nodes array ptr
  // if param_3 < ε → FUN_00532a60(param_1, iVar1): copy first node
  // if param_3 >= total_length → FUN_00532a60(param_1, last_node): copy last node
  // middle path:
  do { iVar2 = iVar1; iVar1 = iVar2 + 0x24; }
  while (*(float*)(iVar2 + 0x28) < param_3);  // same node stride 0x24, same sentinel +0x28
  FUN_00532b80(local_24, iVar2, iVar2 + 0x24, param_3, 0);  // spline interpolate between nodes
  FUN_00532a60(param_1, local_24);             // copy result to output
}
```
**Analysis:** Simpler version of D-3640. Same path struct layout (nodes at +0x10, count at +4, length at +0xc, node stride 0x24). In-range path: scans for segment, calls FUN_00532b80 to interpolate between node at iVar2 and node at iVar2+0x24 (spline), copies result to param_1. Mode 1 (spline) vs mode 0 (Hermite quaternion).  
**Classification: C1** — camera path spline segment interpolator (mode 1): scans segment, calls FUN_00532b80 for per-node spline blend, copies result to output.  
**New DEFERRED D-5989:** FUN_00532b80 (0x00532b80) — spline interpolation between two adjacent path nodes; args (out_64B, node_a, node_b, t, mode=0).  
(FUN_00532a60 counted at D-5983 already.)

---

### D-3648 — FUN_004d8350 (0x004d8350)
**Body:** 0x004d8350..0x004d83ce (0x7e bytes)  
**Decompilation (literal):**
```c
void FUN_004d8350(int param_1)
{
  bVar1 = *(byte*)(param_1 + 3);     // flags byte at +3
  if ((bVar1 & 4) != 0) {            // bit 2 = dirty
    // Copy 16 dwords (64 bytes): param_1+0x10 → param_1+0x50 (local matrix → LTM)
    for (iVar3 = 0x10; iVar3 != 0; iVar3--) { param_1[0x50/4+i] = param_1[0x10/4+i]; }
  }
  for (iVar3 = *(int*)(param_1+0x98); iVar3 != 0; iVar3 = *(int*)(iVar3+0x9c)) {
    bVar2 = *(byte*)(iVar3 + 3) | bVar1;   // inherit parent dirty
    if ((bVar2 & 4) != 0) {
      FUN_004c4600(iVar3+0x50, iVar3+0x10, *(int*)(iVar3+4)+0x50);  // compose child LTM
      *(byte*)(iVar3+3) &= 0xfb;           // clear child dirty bit 2
    }
    FUN_004d83d0(*(undefined4*)(iVar3+0x98), bVar2);  // recurse grandchildren
  }
  *(byte*)(param_1+3) = bVar1 & 0xfa;     // clear bits 1 and 2
}
```
**RwFrame layout confirmed:**
- +3: flags byte (bit 2 = LTM dirty, bit 1 = modelling dirty)
- +4: attached object ptr (used by child as parent world matrix source)
- +0x10..+0x4f: local modelling matrix (4×4 float, 64 bytes)
- +0x50..+0x8f: LTM / world matrix (4×4 float, 64 bytes)
- +0x98: first child ptr
- +0x9c: sibling link ptr

**Analysis:** RW frame hierarchy LTM propagation. When bit 2 set: copies local matrix to LTM. Traverses child chain (via +0x98/+0x9c). For each child with inherited dirty: calls FUN_004c4600 to compose child LTM = child_local × parent_LTM, clears child dirty, recurses into grandchildren via FUN_004d83d0. Clears parent dirty bits at end. Matches RwFrameUpdateObjects.  
**Classification: C1** — RW frame LTM hierarchy update (RwFrameUpdateObjects pattern): copies local→LTM when dirty (bit 2), propagates to child list, recurses via FUN_004d83d0.  
**New DEFERRED D-5987:** FUN_004c4600 (0x004c4600) — compose child LTM: args (child_LTM_out, child_local, parent_LTM).  
**New DEFERRED D-5988:** FUN_004d83d0 (0x004d83d0) — recursive grandchild LTM propagation: args (child+0x98, parent_dirty_flags).

---

### D-3660 — FUN_0047ba20 (0x0047ba20)
**Body:** 0x0047ba20..0x0047bb05 (0xe5 bytes)  
**Decompilation (literal):**
```c
undefined4 FUN_0047ba20(int param_1, int param_2, float* param_3, uint* param_4)
{
  uVar6 = (uint)*(byte*)(param_1 + 0xf0 + param_2 * 4);   // edge byte[0] = vertex index
  iVar1 = param_1 + uVar6 * 0xc;                           // vertex ptr (stride 12 = 3 floats)
  // delta from vertex position to test point:
  local_c = param_3[0] - *(float*)(param_1 + (uVar6*3 + 0x24)*4);  // Δx from vertex +0x90
  local_8 = param_3[1] - *(float*)(iVar1 + 0x94);                   // Δy
  local_4 = param_3[2] - *(float*)(iVar1 + 0x98);                   // Δz
  FUN_004c39b0(&local_c, &local_c);   // normalize Δ
  pfVar2 = (float*)(param_1 + (uint)*(byte*)(param_1 + 0xf2 + param_2*4) * 0xc);  // edge normal A
  pfVar3 = (float*)(param_1 + (uint)*(byte*)(param_1 + 0xf3 + param_2*4) * 0xc);  // edge normal B
  fVar4 = dot(pfVar2, delta);   fVar5 = dot(pfVar3, delta);
  if (param_4 != NULL) *param_4 = (uint)(fVar4 <= fVar5);  // side: 0=(A>B), 1=(A<=B)
  if (fVar5 * fVar4 < DAT_005d757c) return 1;  // opposite signs → intersection
  return 0;
}
```
**Camera-path node layout confirmed:**
- Normal vectors at param_1 + normal_idx * 0xc (normals first in node memory, stride 12 bytes)
- Vertex positions at param_1 + 0x90 + vertex_idx * 0xc (offset 0x90 from node base, stride 12)
- Edge table at param_1 + 0xf0, stride 4 bytes per edge:
  - byte[0] = vertex index (used for position lookup)
  - byte[2] = normal index A
  - byte[3] = normal index B

**Analysis:** Camera-path node edge intersection test. Normalizes the delta from the edge's vertex to the test point (vehicle position). Dots with two edge normals (A and B). Returns 1 if dot products have opposite signs (point is on the edge: one normal faces it, one faces away). Also outputs side flag (which normal has smaller dot product).  
**Classification: C1** — camera-path node edge signed-dot intersection test: normals at node+0, vertices at node+0x90, edge table at node+0xf0; returns 1 on sign-change intersection.  
**No new DEFERRED** (FUN_004c39b0 already analyzed).

---

### D-3661 — FUN_00477e60 (0x00477e60)
**Body:** 0x00477e60..0x00477ef9 (0x99 bytes)  
**Decompilation (literal):**
```c
void FUN_00477e60(float* param_1, float* param_2, float* param_3, float* param_4)
{
  // cross = (param_4 - param_3) × (param_2 - param_3)
  *param_1      = (param_4[1]-param_3[1])*(param_2[2]-param_3[2]) - (param_4[2]-param_3[2])*(param_2[1]-param_3[1]);
  param_1[1]    = (param_4[2]-param_3[2])*(param_2[0]-param_3[0]) - (param_4[0]-param_3[0])*(param_2[2]-param_3[2]);
  param_1[2]    = (param_4[0]-param_3[0])*(param_2[1]-param_3[1]) - (param_4[1]-param_3[1])*(param_2[0]-param_3[0]);
  FUN_004c39b0(param_1, param_1);                                    // normalize normal
  param_1[3] = -(param_1[2]*param_2[2] + *param_1**param_2 + param_1[1]*param_2[1]); // plane D
}
```
**Analysis:** Plane normal computation from two edge vectors. Cross product of (param_4-param_3) and (param_2-param_3) gives the normal perpendicular to the edge. Normalizes. Sets param_1[3] = -dot(normal, param_2) for the plane equation (nx*x + ny*y + nz*z + d = 0, d stored at +3). param_2 is the shared vertex; param_3 and param_4 are the edge endpoints. Output: normalized plane in 4-float (nx,ny,nz,d) form. Used by FUN_0047bb10 to build the edge-normal table.  
**Classification: C1** — edge plane normal builder: cross(param_4-param_3, param_2-param_3), normalize, plane-d = -dot(normal, param_2). Outputs 4-float plane.  
**No new DEFERRED** (FUN_004c39b0 already analyzed).

---

### D-3662 — FUN_00477f00 (0x00477f00)
**Body:** 0x00477f00..0x00477f45 (0x45 bytes)  
**Decompilation (literal):**
```c
void FUN_00477f00(float* param_1, float* param_2, float* param_3)
{
  *param_1 = *param_3; param_1[1] = param_3[1]; param_1[2] = param_3[2];
  FUN_004c39b0(param_1, param_1);   // normalize
  param_1[3] = -(param_1[2]*param_2[2] + *param_1**param_2 + param_1[1]*param_2[1]);  // plane D
}
```
**Analysis:** Simplified plane constructor. Copies param_3[0..2] as the normal direction, normalizes it, then computes D = -dot(normal, param_2) from a reference anchor point. Compared to FUN_00477e60: this takes a pre-existing direction vector (param_3) rather than computing the cross product. In FUN_0047bb10 caller context: param_3 = centroid delta vector (local_c), param_2 = node+0x138 (reference point), output goes to a plane array indexed by hit count.  
**Classification: C1** — plane constructor from direction + anchor: normalizes param_3, sets plane D = -dot(normalized_param_3, param_2); output: 4-float plane (nx,ny,nz,d).  
**No new DEFERRED.**

---

### D-3655 — FUN_004c4dc0 (0x004c4dc0)
**Body:** 0x004c4dc0..0x004c4ea0 (0xe0 bytes)  
**Decompilation (literal):**
```c
float* FUN_004c4dc0(float* param_1, float* param_2)
{
  if ((*(uint*)(DAT_007d4028 + 4 + DAT_007d3ff8) & (uint)param_2[3] & 0x20000) != 0) {
    memcpy(param_1, param_2, 64);  // identity/flagged fast path: direct copy
    return param_1;
  }
  if ((SUB41(param_2[3], 0) & 3) == 3) {  // orthonormal flags = 3
    // Transpose 3x3 rotation block: param_1[0,1,2] = param_2[0,4,8] etc.
    // Invert translation: param_1[0xc] = -(param_2[0xc]*R[0..2] + ...)  (-R^T * T)
    param_1[3] = 4.2039e-45;   // flags word set to tiny bit pattern
    return param_1;
  }
  FUN_004c4eb0(param_1, param_2);   // general matrix inversion fallback
  return param_1;
}
```
**Analysis:** RwMatrixInvert with 3 paths:
1. Fast: flagged bit 0x20000 in matrix flags word (param_2[3]) — direct copy.
2. Orthonormal (flags & 3 == 3): transpose 3×3 rotation, compute -R^T*T for translation. Classic rigid-body inverse.
3. General: FUN_004c4eb0 for non-orthonormal matrices.

Matrix layout: param_2[0..3] = row/col 0, [4..7] = row/col 1, [8..11] = row/col 2, [0xc..0xe] = translation xyz, [3] = flags word.  
**Classification: C1** — RwMatrixInvert: 3-path (copy→orthonormal-transpose→general-invert via FUN_004c4eb0).  
**New DEFERRED D-5990:** FUN_004c4eb0 (0x004c4eb0) — general 4×4 matrix inversion; args (out_matrix, in_matrix).

---

### D-3656 — FUN_004c3d60 (0x004c3d60)
**Body:** 0x004c3d60..0x004c3d85 (0x25 bytes)  
**Decompilation (literal):**
```c
undefined4 FUN_004c3d60(undefined4 param_1, undefined4 param_2, undefined4 param_3)
{
  (**(code**)(DAT_007d3ffc + 8 + DAT_007d3ff8))(param_1, param_2, param_3);
  return param_1;
}
```
**Analysis:** RW vtable dispatch at slot offset +8 of the secondary table (DAT_007d3ffc + 8 + DAT_007d3ff8). Sibling of FUN_004c3dc0 (analyzed in d2, slot +0x10). 3 args passed. Slot +8 = table slot 2. Candidate: RwV3dTransformVector or RwV3dTransformPoints (the 3-arg variant).  
**Classification: C1** — RW vtable vector/point transform dispatch (slot +8); 3 args; sibling of FUN_004c3dc0.  
**[UNCERTAIN] U-2028:** DAT_007d3ff8 vtable slot at +8: called by FUN_004c3d60 with 3 args; candidate RwV3dTransformVector (cf. FUN_004c3dc0 at slot +0x10). Resolution: reference_to DAT_007d3ff8 initializer; enumerate slot at +8.  
**No new DEFERRED.**

---

### D-3657 — FUN_0055b650 (0x0055b650)
**Body:** 0x0055b650..0x0055b74a (0xfa bytes)  
**Decompilation (literal key parts):**
```c
void FUN_0055b650(int* param_1, float* param_2, float* param_3)
{
  // param_1[0] = physics object base; param_1[1] = body index
  pfVar8 = (float*)(*(int*)(*(int*)(*param_1 + 0x10) + 0x30) + param_1[1] * 0x20);
  *pfVar8 += *param_2;       // force accumulator: X
  pfVar8[1] += param_2[1];   //                    Y
  pfVar8[2] += param_2[2];   //                    Z
  // lever arm r = param_3 - body_position
  iVar9 = param_1[1] * 0x40 + **(int**)(*param_1 + 0x10);
  fVar5 = *param_3 - *(float*)(iVar9 + 0x30);   // r.x
  fVar7 = param_3[1] - *(float*)(iVar9 + 0x34); // r.y
  fVar6 = param_3[2] - *(float*)(iVar9 + 0x38); // r.z
  // torque = r × F
  torque_buf[0] += param_2[2]*fVar7 - param_2[1]*fVar6;  // τ.x
  torque_buf[1] += fVar6*param_2[0] - param_2[2]*fVar5;  // τ.y
  torque_buf[2] += param_2[1]*fVar5 - fVar7*param_2[0];  // τ.z
}
```
**Physics body layout:**
- *param_1 (base): +0x10 → sub-ptr → +0x30 = force buffer base, +0x00 = body array base
- Body array stride: 0x40 per body (position at body+0x30..+0x38 = xyz)
- Force buffer stride: 0x20 per body (linear force[0..2], torque[4..6] at +0x10, +0x14, +0x18)

**Analysis:** Rigid-body force + torque accumulator. Adds linear force (param_2) at body index param_1[1]. Computes lever arm r = application point (param_3) − body position. Adds r × F as torque. Applied-force point determines torque asymmetry. Standard impulse/force application for physics engine.  
**Classification: C1** — physics force+torque accumulator: linear += param_2, torque += (param_3 − body_pos) × param_2.  
**No new DEFERRED.**

---

### D-3658 — FUN_0055c4a0 (0x0055c4a0)
**Body:** 0x0055c4a0..0x0055c4eb (0x4b bytes)  
**Decompilation (literal):**
```c
void FUN_0055c4a0(int param_1, undefined4 param_2)
{
  if (**(short**)(param_1 + 0x5c) == 8) {       // composite body type flag
    piVar1 = *(int**)(param_1 + 0x40);           // sub-objects array
    for (uVar2 = 0; uVar2 < (uint)piVar1[1]; uVar2++) {
      FUN_0055c4a0(*piVar1 + uVar2 * 0x60, param_2);  // recurse: sub-object stride 0x60
    }
  } else {
    *(undefined4*)(param_1 + 0x54) = param_2;    // write scalar to leaf +0x54
  }
}
```
**Analysis:** Recursively sets scalar at body+0x54. For composite objects (type 8 at *(param_1+0x5c)), recurses into sub-objects (count at piVar1[1], stride 0x60). Leaf bodies (non-composite): write param_2 directly. Called from FUN_00471780 with 0x3c23d70a (~0.01f), so +0x54 is likely a spin/angular damping constant.  
**Classification: C1** — physics body scalar setter at +0x54 (damping/spin): recurses over composite body hierarchy (type==8, stride 0x60 sub-objects).  
**No new DEFERRED.**

---

### D-3659a — FUN_0055ac50 (0x0055ac50)
**Body:** 0x0055ac50..0x0055ad14 (0xc4 bytes)  
**Decompilation summary:**  
- param_3 != 0 (activate): sets bit indexed by *(ushort*)(param_2+8) in bitfield at *(param_1+100).
- param_3 == 0 (deactivate): if constraint active in both bodies' bitfields and param_2[9] != 0, calls vtable+0x14 cleanup, clears bit in *(param_1+0x60), decrements *(param_1+0x4c). Always clears bit in *(param_1+100).  
**Analysis:** Physics constraint bitfield activate/deactivate. Two independent bitfields at param_1+0x60 and param_1+0x64 (100). The +0x60 field tracks "live" constraints; +0x64 tracks pending/activation state. On deactivate: calls constraint vtable+0x14 cleanup iff both bodies have constraint active (param_2[9] is the other body ptr).  
**Classification: C1** — constraint bitfield A (param_1+0x64) set/clear with vtable cleanup on deactivate; counter at param_1+0x4c.  
**No new DEFERRED.**

---

### D-3659b — FUN_0055ac00 (0x0055ac00)
**Body:** 0x0055ac00..0x0055ac4d (0x4d bytes)  
**Decompilation (literal):**
```c
void FUN_0055ac00(int param_1, int param_2, int param_3)
{
  uVar2 = *(ushort*)(param_2 + 0x20);   // constraint ID from param_2+0x20 (vs +8 in FUN_0055ac50)
  if (param_3 != 0) {
    *(uint*)(*(int*)(param_1 + 0x5c) + (uVar2>>5)*4) |= 1 << (uVar2 & 0x1f);
  } else {
    *(uint*)(*(int*)(param_1 + 0x5c) + (uVar2>>5)*4) &= ~(1 << (uVar2 & 0x1f));
  }
}
```
**Analysis:** Simpler bitfield set/clear. Uses constraint ID from param_2+0x20 (not +8 like FUN_0055ac50). Operates on bitfield at *(param_1+0x5c) (different field from FUN_0055ac50). No vtable callback. Sibling to FUN_0055ac50 but for a separate constraint bitfield.  
**Classification: C1** — constraint bitfield B (*(param_1+0x5c)) set/clear; ID from param_2+0x20.  
**No new DEFERRED.**

---

### D-3659c — FUN_0055ad30 (0x0055ad30)
**Body:** 0x0055ad30..0x0055add0 (0xa0 bytes)  
**Decompilation (literal):**
```c
int* FUN_0055ad30(int param_1, int* param_2)
{
  // Early exit: already active on other body (param_2[9])
  if ((param_2[9] != 0) && (bitfield_test(param_2[9]+0x60, param_2[8]) != 0)) {
    return param_2;
  }
  // Capacity check: param_1+0x4c < param_1+0x48
  if (*(uint*)(param_1 + 0x4c) < *(uint*)(param_1 + 0x48)) {
    iVar2 = (**(code**)(*param_2 + 0x10))(param_2);  // vtable+0x10 init callback
    if (iVar2 != 0) {
      bitfield_set(param_1+0x60, param_2[8]);  // mark live
      DAT_007dc8c0 = 0;
      *(int*)(param_1 + 0x4c)++;
      return param_2;
    }
    if (DAT_007dc8c0 == 0) DAT_007dc8c0 = 1;  // error flag
  }
  return NULL;
}
```
**Analysis:** Constraint attach with capacity check and vtable init. Guards: already connected to other body → return early. Capacity: counter at +0x4c must be < max at +0x48. Calls vtable+0x10 for constraint init; if successful marks in bitfield at +0x60, increments counter. DAT_007dc8c0 (0x007dc8c0) is a global constraint-overflow error flag.  
**Classification: C1** — constraint attach: capacity guard (param_1+0x48), vtable+0x10 init, bitfield set at param_1+0x60, counter at param_1+0x4c; DAT_007dc8c0 error flag.  
**No new DEFERRED.**

---

## Classification Summary

| RVA | Name | Prior Status | This Session | Notes |
|---|---|---|---|---|
| 0x0047d080 | FUN_0047d080 | D-3652 | C1 | cam-anim show/hide by inner-ref index |
| 0x0047d100 | FUN_0047d100 | D-3653 | C1 [UNCERTAIN] | cam-anim gate + FUN_004b5240 (U-2027) |
| 0x0047d150 | FUN_0047d150 | D-3654 | C1 | cam-anim sub-obj+8 double-deref getter |
| 0x00426020 | FUN_00426020 | D-3651 | C1 | scene-root address getter (DAT_00646e58) |
| 0x004e66d0 | FUN_004e66d0 | D-3642 | C1 | RW linked-list ForAll traversal |
| 0x004752f0 | FUN_004752f0 | D-3640 | C1 | camera path Hermite quaternion interpolator mode 0 |
| 0x004b47e0 | FUN_004b47e0 | D-3641 | C1 | camera path spline interpolator mode 1 |
| 0x0047ba20 | FUN_0047ba20 | D-3660 | C1 | camera-path node edge intersection test |
| 0x00477e60 | FUN_00477e60 | D-3661 | C1 | edge plane normal from cross product |
| 0x004d8350 | FUN_004d8350 | D-3648 | C1 | RwFrameUpdateObjects LTM hierarchy propagation |
| 0x00477f00 | FUN_00477f00 | D-3662 | C1 | plane constructor from direction + anchor |
| 0x004c4dc0 | FUN_004c4dc0 | D-3655 | C1 | RwMatrixInvert 3-path |
| 0x004c3d60 | FUN_004c3d60 | D-3656 | C1 [UNCERTAIN] | RW vtable vector-transform slot +8 (U-2028) |
| 0x0055b650 | FUN_0055b650 | D-3657 | C1 | rigid-body force+torque accumulator |
| 0x0055c4a0 | FUN_0055c4a0 | D-3658 | C1 | physics body spin/damping setter (+0x54) recursive |
| 0x0055ac50 | FUN_0055ac50 | D-3659a | C1 | constraint bitfield A activate/deactivate |
| 0x0055ac00 | FUN_0055ac00 | D-3659b | C1 | constraint bitfield B set/clear |
| 0x0055ad30 | FUN_0055ad30 | D-3659c | C1 | constraint attach with capacity + vtable init |

Total: 18 functions analyzed, all C1.

---

## New UNCERTAINTIES (U=2027..2028 used)

| ID | RVA | Type | Evidence Gap | Resolution |
|---|---|---|---|---|
| U-2027 | 0x0047d100 | calling-convention | FUN_004b5240 called with no visible args in decompilation; may be thiscall/fastcall artifact | listing_disassemble_function 0x0047d100; check register setup before CALL to FUN_004b5240 |
| U-2028 | 0x004c3d60 | structural | DAT_007d3ff8 vtable slot at +8: called by FUN_004c3d60 with 3 args; candidate RwV3dTransformVector (cf. FUN_004c3dc0 at slot +0x10) | reference_to DAT_007d3ff8; enumerate slot at +8 in vtable initializer |

---

## New STUBS (S=2020..2039)

No new stubs generated. This session is analysis-only; hook code has not been written for these functions, so no STUBS.md entries arise.

---

## New DEFERRED rows (D=5980..6039 range)

### In camera_follow_d3 main bucket (≤3 callees per source function):

| ID | RVA | Caller | Role / evidence | Bucket |
|---|---|---|---|---|
| D-5980 | 0x00559ee0 FUN_00559ee0 | FUN_0047d080 | show/activate cam-anim object; args (context, anim_obj); called when anim_obj+0x24 != 0 | camera_follow_d3 |
| D-5981 | 0x00559c40 FUN_00559c40 | FUN_0047d080 | hide/deactivate cam-anim object; 4 args (context, anim_obj, 0xffffffff, 7); called when +0x24 == 0 | camera_follow_d3 |
| D-5982 | 0x004b5240 FUN_004b5240 | FUN_0047d100 | called after cam-anim null check; no visible args in decomp; role unknown | camera_follow_d3 |
| D-5987 | 0x004c4600 FUN_004c4600 | FUN_004d8350 | compose child LTM = child_local × parent_LTM; args (child_LTM_out, child_local, parent_LTM) | camera_follow_d3 |
| D-5988 | 0x004d83d0 FUN_004d83d0 | FUN_004d8350 | recursive grandchild LTM propagation; args (child+0x98, parent_dirty_flags) | camera_follow_d3 |
| D-5989 | 0x00532b80 FUN_00532b80 | FUN_004b47e0 | spline blend between two adjacent path nodes; args (out_64B, node_a, node_b, param_3, 0) | camera_follow_d3 |
| D-5990 | 0x004c4eb0 FUN_004c4eb0 | FUN_004c4dc0 | general 4×4 matrix inversion; args (out_matrix, in_matrix) | camera_follow_d3 |

### In camera_follow_d3-cont1 bucket (FUN_004752f0 exceeded cap with 4 callees):

| ID | RVA | Caller | Role / evidence | Bucket |
|---|---|---|---|---|
| D-5983 | 0x00532a60 FUN_00532a60 | FUN_004752f0 + FUN_004b47e0 | copies 64-byte transform block from path node to output; path-node matrix copy | camera_follow_d3-cont1 |
| D-5984 | 0x00546e70 FUN_00546e70 | FUN_004752f0 | quaternion dot-product/slerp prep; args (quat_a, quat_b, &out_float) | camera_follow_d3-cont1 |
| D-5985 | 0x00482ae0 FUN_00482ae0 | FUN_004752f0 | spline position evaluator; args (t, out_pos_block, ctrl_a, seg_start, seg_end, ctrl_b) | camera_follow_d3-cont1 |
| D-5986 | 0x004c51a0 FUN_004c51a0 | FUN_004752f0 | transform composition; args (matrix, position_block, mode=2) | camera_follow_d3-cont1 |

---

## Unresolved Questions

1. FUN_0047d100 / FUN_004b5240 argument passing — U-2027.
2. FUN_004c3d60 vtable slot +8 identity — U-2028.
3. DAT_00646e58 (0x00646e58): exact type/role of scene root (returned by FUN_00426020). Need type_get on DAT_00646e58 or reference_from to find who writes it.
4. FUN_004e66d0 iterator callbacks at LAB_* addresses: what do the per-element callbacks do? (Out of scope for this session.)
5. Physics body layout: struct at *param_1 for FUN_0055b650 — full layout not yet characterized; +0x10, +0x30, +0x40 sub-pointers partially known.

---

## Pool Slot Release

No lock file created. Mashed_pool9 remains available.

---

## Notes on Re-Classify

The re-classify skill must be run for:
- D-3640..D-3642, D-3648, D-3651..D-3662: mark as C1 in hooks.csv for all 18 functions above
- Add U-2027 and U-2028 to UNCERTAINTIES.md
- Add D-5980..D-5990 to DEFERRED.md with buckets per table above
- No STUBS.md changes for this session
All tracker mutations must go through re-classify per project rules.
