# SESSION_END — camera_follow_d2-20260503-HHMM

**Session ID:** camera_follow_d2-20260503  
**Pool slot:** Mashed_pool6  
**Session type:** depth-2 RE sweep for camera_follow bucket  
**ID ranges in scope:** U=1247..1266, D=3640..3699, S=1240..1259  
**Date:** 2026-05-03  

---

## Pre-flight results

- SHA-256 anchor: MATCH — BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
- Pool slot Mashed_pool6: existed, no active .lock
- Ghidra MCP: health_ping OK, ghidra_info OK (Ghidra 12.0.3)
- Project opened READ-ONLY: session_id=4c8f0b3d6c7f4e68a1b05068eaf3badc

---

## Functions Analyzed

All DEFERRED rows D-1540..D-1559 (camera_follow depth-2 callees) were decomped and analyzed.

### D-1559 — FUN_0042b930 (0x0042b930)
**Body:** 0x0042b930..0x0042b935 (6 bytes)  
**Decompilation:**
```c
undefined4 FUN_0042b930(void) { return DAT_0067ecb0; }
```
**Analysis:** Zero-parameter getter. Returns 32-bit value at DAT_0067ecb0 (0x0067ecb0).
Called by FUN_004671a0 and FUN_00467210; discriminant value 3 routes to alt vehicle getter.
Caller context (0067ecac receives param_1 from FUN_00432080 and FUN_004331a0 per U-1073) shares same base address block — DAT_0067ecac and DAT_0067ecb0 are 4 bytes apart, suggesting DAT_0067ecb0 is a state field in the same struct as DAT_0067ecac.  
**Classification: C1** — decompilation complete, role as game-mode/race-state discriminant getter confirmed structurally; no Frida diff yet.  
**New DEFERRED:** none (5-byte body, no callees).  
**S-0549 can be cleared** — body fully characterized.

### D-1560 — FUN_0042f510 (0x0042f510)
**Body:** 0x0042f510..0x0042f515 (6 bytes)  
**Decompilation:**
```c
undefined4 FUN_0042f510(void) { return DAT_0067f190; }
```
**Analysis:** Zero-parameter getter. Returns 32-bit value at DAT_0067f190 (0x0067f190).
Used as alternative vehicle-0 handle when FUN_0042b930()==3 and param_1!=-1.  
**Classification: C1** — decompilation complete; semantic meaning of DAT_0067f190 still requires cross-ref (U-0907 already filed).  
**S-0550 can be cleared.**

### D-1540 — FUN_004756e0 (0x004756e0)
**Body:** 0x004756e0..0x00475763 (0x83 bytes)  
**Decompilation (literal):**
```c
void FUN_004756e0(float param_1, int param_2, float *param_3, float param_4, int param_5)
{
  if ((param_1 != 0.0) && (param_2 != 0)) {
    uVar1 = *(undefined4*)((int)param_1 + 4);   // object handle at param_1+4
    param_1 = param_4 + *param_3;               // accumulated position offset
    if (*(float*)(param_2 + 0xc) < param_1) {
      param_1 = param_1 - *(float*)(param_2 + 0xc); // wrap: subtract path length at param_2+0xc
    }
    if (param_5 == 0) { FUN_004752f0(local_40, param_2, param_1); }
    else              { FUN_004b47e0(local_40, param_2, param_1); }
    *param_3 = param_1;
    FUN_004c1480(uVar1, local_40, 0);           // sets transform on object
  }
}
```
**Analysis:** Camera path node per-tick updater. Accumulates position offset (param_4 + *param_3), wraps if exceeds path length at param_2+0xc. Dispatches to FUN_004752f0 (param_5==0) or FUN_004b47e0 (param_5!=0) to compute a 64-byte transform block (local_40). Writes accumulated position back to *param_3. Calls FUN_004c1480 to apply the transform to the object at param_1+4.  
**Classification: C1** — structure fully characterized. Role: camera node transform updater with wrap-around position accumulation.  
**New callees (depth-3, new DEFERRED D-3640..D-3641):**
- FUN_004752f0 (0x004752f0) — computes 64-byte transform block; path mode 0
- FUN_004b47e0 (0x004b47e0) — computes 64-byte transform block; path mode 1 (spline)
- FUN_004c1480 (0x004c1480) — sets transform/matrix on object; candidate RW frame set-matrix  
**S-0544 can be cleared.**

### D-1541 — FUN_00475010 (0x00475010)
**Body:** 0x00475010..0x0047503c (0x2c bytes)  
**Decompilation:**
```c
undefined4 FUN_00475010(undefined4 param_1) {
  local_4 = &stack0x00000008;
  local_8 = &LAB_00474f50;
  FUN_00474fb0(param_1, &local_8);
  return param_1;
}
```
FUN_00474fb0 calls FUN_004e66d0(param_1, &LAB_00474f90, param_2) — LAB_00474f90 and LAB_00474f50 are code labels.  
**Analysis:** Sets up a two-pointer stack frame and calls FUN_00474fb0 which dispatches to FUN_004e66d0 with a code-label argument. Pattern matches a C++ exception-frame or SEH setup / destructor registration (stack unwinding record). No time-delta arithmetic visible at this level; the actual per-entry work is inside LAB_00474f50/LAB_00474f90.  
**Classification: C1** — body characterized as exception/cleanup wrapper; inner code at LAB_00474f50..LAB_00474f90 not yet decomped.  
[UNCERTAIN] Whether this is C++ exception framing or a custom cleanup callback — evidence missing: need listing_disassemble_range 0x00474f50..0x00474fb0 to read inner block.  
**New DEFERRED D-3642:** FUN_004e66d0 (0x004e66d0) — called from exception/cleanup wrapper; role unknown.  
**S-0545 can be cleared (structure known).**

### D-1542 — FUN_004c4d20 (0x004c4d20)
**Body:** 0x004c4d20..0x004c4dba (0x9a bytes)  
**Decompilation (literal):**
```c
param_3 = param_3 * _DAT_005cd7a8;  // scale angle by constant at 0x005cd7a8
fVar1 = FUN_004c3b90(param_2[2]*param_2[2] + param_2[1]*param_2[1] + *param_2**param_2); // rsqrt
local_c = (float)(fVar1 * *param_2);   // normalize x
local_8 = (float)(param_2[1] * fVar1); // normalize y
local_4 = (float)(param_2[2] * fVar1); // normalize z
fVar1 = fsin(param_3);
fVar2 = fcos(param_3);
FUN_004c4a50(param_1, &local_c, 1.0f - fVar2, fVar1, param_4);
return param_1;
```
DAT_005cd7a8 constant at 0x005cd7a8 (degrees-to-radians or similar scale factor — raw value not read yet).  
FUN_004c3b90 is an inverse-square-root (rsqrt) approximation using the RW lookup table at DAT_007d3ffc+4+DAT_007d3ff8.  
FUN_004c4a50 builds and applies a rotation matrix from axis (local_c, local_8, local_4), sin(angle), 1-cos(angle), param_4 (combine mode).  
**Analysis:** Rotation-matrix builder from axis-angle representation: normalizes axis vector using rsqrt, computes sin/cos, calls Rodrigues rotation formula FUN_004c4a50.  
**Classification: C1** — RW math function identified by pattern. Candidate: RtQuatConvertFromMatrix / RwMatrixRotate variant.  
**S-0546 can be cleared.**

### D-1543 — FUN_004c3dc0 (0x004c3dc0)
**Body:** 0x004c3dc0..0x004c3de5 (0x25 bytes)  
**Decompilation:**
```c
undefined4 FUN_004c3dc0(undefined4 param_1, undefined4 param_2, undefined4 param_3) {
  (**(code**)(DAT_007d3ffc + 0x10 + DAT_007d3ff8))(param_1, param_2, param_3);
  return param_1;
}
```
**Analysis:** Vtable dispatch via DAT_007d3ffc+0x10+DAT_007d3ff8. DAT_007d3ffc is the RW plugin system pointer (shared with FUN_004c3b90). Offset +0x10 into the secondary table is slot 4 (0x10/4). Passes 3 args (dest, src, matrix?) — candidate RwV3dTransformPoints or equivalent RW vector transform thunk.  
**Classification: C1** — identified as RW vtable vector-transform dispatch; exact slot meaning [UNCERTAIN]: evidence gap = need to enumerate DAT_007d3ff8 vtable initializer to confirm slot-4 function pointer.  
**New UNCERTAINTY U-1247:** DAT_007d3ff8 vtable slot at +0x10 (offset 0x10 into secondary table): called by FUN_004c3dc0 with 3 args; candidate RwV3dTransformPoints but unconfirmed. Resolution: reference_to DAT_007d3ff8 to find initializer; enumerate slot at +0x10.  
**S-0547 can be cleared (structure known).**

### D-1544 — FUN_004c39b0 (0x004c39b0)
**Body:** 0x004c39b0..0x004c3abe (0x10e bytes)  
**Decompilation (literal key parts):**
```c
// Computes squared magnitude of *param_2 vector
param_2 = (float*)(param_2[2]*param_2[2] + param_2[1]*param_2[1] + *param_2**param_2);
// rsqrt via RW lookup: DAT_007d3ffc + 4 + DAT_007d3ff8
fVar2 = (float)(...lookup-rsqrt-of-squared-magnitude...);
// Normalize: write result[0..2] = *pfVar1 * rsqrt
*param_1 = ... * *pfVar1;
param_1[1] = pfVar1[1] * ...;
param_1[2] = pfVar1[2] * ...;
// Error path at 0x004c3a8x: if fVar2 < DAT_005d757c, calls FUN_004d7ff0(0x19) + FUN_004d8480
```
**Analysis:** In-place vector normalization. Writes normalized vector to *param_1 from *param_2 (3-float input). Uses RW rsqrt lookup table. Error guard: if magnitude below threshold at DAT_005d757c (0x005d757c), calls error handler FUN_004d7ff0(0x19) + FUN_004d8480. Matches RwV3dNormalize behavior.  
**Classification: C1** — normalized-vector writer. Confirmed RW vector normalize pattern.  
**S-0548 can be cleared.**

### D-1545 — FUN_004a2c48 (0x004a2c48)
**Body:** 0x004a2c48..0x004a2cbc (0x74 bytes)  
**Decompilation:** Bankers-round (round-half-to-even) for x87 float10 → ulonglong. Takes implicit in_ST0 (x87 FPU top). Rounds to nearest even 64-bit integer.  
**Analysis:** CRT-style float-to-int64 rounding helper. Called from FUN_00426810 (camera-path position lerp). Used where alpha position must be discretized to an integer index or counter.  
**Classification: C1** — CRT/compiler intrinsic. No game logic.  
**No new DEFERRED.**

### D-1546 — FUN_004924c0 (0x004924c0)
**Body:** 0x004924c0..0x004924dd (0x1d bytes)  
**Decompilation:**
```c
void FUN_004924c0(undefined1 param_1, undefined1 param_2, undefined1 param_3) {
  DAT_006147b4 = param_1;
  DAT_006147b5 = param_2;
  DAT_006147b6 = param_3;
}
```
**Analysis:** Writes 3 bytes (param_1..param_3) to consecutive addresses 0x006147b4, 0x006147b5, 0x006147b6. Matches RGB byte setter pattern. Already noted in U-0912: "3 consecutive bytes at DAT_006147b4/b5/b6 set by (param_1, param_2, param_3); likely RGB colour but not confirmed." Decompilation confirms exact body.  
**Classification: C1** — 3-byte write to global; semantics (RGB?) requires caller trace per U-0912.  
**No new DEFERRED.**

### D-1547 — FUN_004c1b10 (0x004c1b10)
**Body:** 0x004c1b10..0x004c1b3b (0x2b bytes)  
**Decompilation:**
```c
int FUN_004c1b10(int param_1, undefined4 param_2) {
  *(undefined4*)(param_1 + 0x84) = param_2;  // write to camera object+0x84
  FUN_004c1a70(param_1);                      // recompute projection from +0x84 and +0x80
  if (*(int*)(param_1 + 4) != 0) {
    FUN_004c0e50(*(int*)(param_1 + 4));        // update attached frame if present
  }
  return param_1;
}
```
FUN_004c1a70 (0x004c1a70) reads param_1+0x84 (far clip or field-of-view), param_1+0x80 (near clip or FOV), param_1+0x14 (projection mode: 2=parallel), reads viewport from DAT_007d3ff8+0x18..0x1c, and writes derived frustum scale at param_1+0x8c and param_1+0x90.  
**Analysis:** FUN_004c1b10 is a camera projection parameter setter. It writes param_2 to offset +0x84 (far clip / view-window height candidate), then calls FUN_004c1a70 to recompute the projection matrix scale coefficients at +0x8c and +0x90 from near/far clip and viewport extents. Matches RwCameraSetFarClipPlane pattern (or RwCameraSetViewWindow). FUN_004c0e50 (0x004c0e50) updates an attached frame dirty flag at object+0xa0.  
**Classification: C1** — camera projection-parameter setter confirmed; RW camera layout matches (near=+0x80, far/viewwindow=+0x84).  
**S-0548 (path-lerp callee) can be cleared.**  
**New DEFERRED D-3643:** FUN_004c0e50 (0x004c0e50) — called when param_1+4 != 0; takes the attached frame pointer; reads flag at +0xa0+3; may call FUN_004d8350 (0x004d8350); role: dirty-flag update for RW frame. Depth-3 here.

### D-1548 — FUN_0040e180 (0x0040e180)
**Body:** 0x0040e180..0x0040e330 (0x1b0 bytes)  
**Decompilation (key literals):**
```c
// Outer loop: iVar2 = 0..3 (4 vehicles), offset start=0x34, stride=4
// PTR_PTR_005f2770 + local_2c — slot validity array at 0x005f2770
// FUN_0046c7b0(iVar2) returns 1 when vehicle iVar2 is active
// FUN_0046cbb0(iVar2, &local_38, local_28) — fills local_38=0 means ???
// FUN_0046d4a0(&local_34, iVar2) — gets position struct ptr into local_34
// local_24 = *(local_34+0x30), local_20 = *(local_34+0x34), local_1c = *(local_34+0x38) = XYZ
// Inner loop: compares distance between all pairs
// FUN_004c3ac0(&local_18) — distance magnitude
// Tracks maximum distance pair: iVar3, local_3c
// Outputs: *param_1 = near vehicle index, *param_2 = far vehicle index
```
**Analysis:** Finds the pair of active vehicles with maximum 3D distance. Iterates all vehicle pairs (4×4, inner/outer), computes 3D distance via FUN_004c3ac0, tracks maximum. Writes near/far vehicle indices to *param_1 and *param_2. Used by FUN_00471ac0 (camera-anim trigger) as "timing seed" (output goes to FUN_00407a20 which reads per-vehicle tick counter at DAT_008a9648+vehicle*0xc3).  
**Classification: C1** — vehicle-pair distance maximizer, outputs two indices.  
**New DEFERRED D-3644:** FUN_0046c7b0 (0x0046c7b0) — vehicle-active test; returns 1 if vehicle index active.  
**New DEFERRED D-3645:** FUN_0046cbb0 (0x0046cbb0) — fills local_38; role unknown; called per vehicle.  
**New DEFERRED D-3646:** FUN_0046d4a0 (0x0046d4a0) — gets position struct ptr for vehicle index.  
**New DEFERRED D-3647:** FUN_004c3ac0 (0x004c3ac0) — 3D vector magnitude; takes float[3] ptr.  
Cap: 4 new DEFERRED rows for this function → push overflow to camera_follow_d2-cont1 per cap rule.

### D-1549 — FUN_00407a40 (0x00407a40)
**Body:** 0x00407a40..0x00407a50 (0x10 bytes)  
**Decompilation:**
```c
undefined4 FUN_00407a40(int param_1) {
  return *(undefined4*)(&DAT_008a9640 + param_1 * 0x30c);
}
```
**Analysis:** Array getter. Reads 4-byte value at DAT_008a9640 + param_1 * 0x30c (stride 0x30c = 780 bytes). Per-vehicle tick counter A (frame count at slot 0 of 780-byte vehicle struct starting at 0x008a9640).  
**Classification: C1** — simple array index read.  
**No new DEFERRED.**

### D-1550 — FUN_00407a20 (0x00407a20)
**Body:** 0x00407a20..0x00407a30 (0x10 bytes)  
**Decompilation:**
```c
undefined4 FUN_00407a20(int param_1) {
  return (&DAT_008a9648)[param_1 * 0xc3];
}
```
**Analysis:** Array getter. Reads 1-byte value at DAT_008a9648 + param_1 * 0xc3 (stride 0xc3 = 195). DAT_008a9648 = DAT_008a9640 + 8. So this reads byte at offset 8 within each entry of a sub-table indexed by param_1, stride 195. DAT_008a9640 is the 780-byte vehicle struct array; DAT_008a9648 is offset 8 within that.  
[UNCERTAIN] The stride 0xc3 vs 0x30c for FUN_00407a40 suggests these are indexing different arrays at the same base — or 0x30c = 4 * 0xc3, meaning FUN_00407a40 reads a 4-byte word and FUN_00407a20 reads byte offset within a sub-table.  
**Classification: C1** — simple array index read.

### D-1551 — FUN_0047ce80 (0x0047ce80)
**Body:** 0x0047ce80..0x0047ce9a (0x1a bytes)  
**Decompilation:**
```c
undefined4 FUN_0047ce80(int param_1) {
  if ((-1 < param_1) && (param_1 < 200)) {
    return (&DAT_006c9758)[param_1];
  }
  return 0xffffffff;
}
```
**Analysis:** Bounds-checked array getter. Returns byte at DAT_006c9758 + param_1 for index 0..199; returns 0xffffffff on out-of-bounds. Array at 0x006c9758, 200 entries, 1 byte each — camera-anim entry ID table.  
**Classification: C1** — simple bounds-checked getter.  
**S-0553 can be cleared.**

### D-1552 — FUN_0047ce00 (0x0047ce00)
**Body:** 0x0047ce00..0x0047ce19 (0x19 bytes)  
**Decompilation:**
```c
undefined4 FUN_0047ce00(int param_1) {
  if ((-1 < param_1) && (param_1 < 200)) {
    return (&DAT_006c9438)[param_1];
  }
  return 0;
}
```
**Analysis:** Bounds-checked array getter. Returns value at DAT_006c9438 + param_1 for index 0..199; returns 0 on out-of-bounds. Array at 0x006c9438 — camera-anim entry flags table (bits 1, 2, 4 tested by caller).  
**Classification: C1** — simple bounds-checked getter.  
**S-0554 can be cleared.**

### D-1553 — FUN_0047d130 (0x0047d130)
**Body:** 0x0047d130..0x0047d149 (0x19 bytes)  
**Decompilation:**
```c
undefined4 FUN_0047d130(int param_1) {
  if ((-1 < param_1) && (param_1 < 200)) {
    return (&DAT_006c71d8)[param_1];
  }
  return 0;
}
```
**Analysis:** Bounds-checked array getter. Returns value at DAT_006c71d8 + param_1 for index 0..199; returns 0 on out-of-bounds. Array at 0x006c71d8 — inner ref / animation-object handle table for camera-anim entries. Used in FUN_00471ec0 dispatch case 2 to get inner ref passed to FUN_0057c210.  
**Classification: C1** — simple bounds-checked getter.

### D-1554 — FUN_0057c210 (0x0057c210)
**Body:** 0x0057c210..0x0057c21d (0xd bytes)  
**Decompilation:**
```c
undefined4 FUN_0057c210(int param_1) {
  return *(undefined4*)(DAT_007dc8d8 + param_1);
}
```
**Analysis:** Array getter using base pointer DAT_007dc8d8 (0x007dc8d8) with byte offset param_1. Returns a 4-byte handle/pointer. Used to retrieve animation object by inner ref index. DAT_007dc8d8 is the animation object table base pointer (a pointer to pointer pattern).  
**Classification: C1** — pointer-table dereference.

### D-1555 — FUN_004c0ed0 (0x004c0ed0)
**Body:** 0x004c0ed0..0x004c0eee (0x1e bytes)  
**Decompilation:**
```c
int FUN_004c0ed0(int param_1) {
  if ((*(byte*)(*(int*)(param_1 + 0xa0) + 3) & 1) != 0) {
    FUN_004d8350(*(int*)(param_1 + 0xa0));
  }
  return param_1 + 0x50;
}
```
**Analysis:** Takes a vehicle sub-object ptr (param_1). Reads *(param_1+0xa0) → a sub-ptr. Tests bit 0 of byte[3] of that sub-ptr (flags byte). If set, calls FUN_004d8350 (0x004d8350) on the sub-ptr. Returns param_1 + 0x50 — a fixed offset into the vehicle sub-object.  
The return value (param_1+0x50) is used by FUN_0047c160 as arg to FUN_0047bb10 as `param_2` (3-float position array context). So offset +0x50 from vehicle sub-object is a 3-float position or transform block.  
**Classification: C1** — frame dirty-check + offset accessor; pattern matches RW frame accessor (returns frame ptr at fixed offset).  
**New DEFERRED D-3648:** FUN_004d8350 (0x004d8350) — called conditionally; takes sub-ptr at param_1+0xa0; role unknown.

### D-1556 — FUN_004c1b40 (0x004c1b40)
**Body:** 0x004c1b40..0x004c1ba7 (0x67 bytes)  
**Decompilation:**
```c
undefined4 FUN_004c1b40(int param_1, float *param_2) {
  uVar4 = 2;
  pfVar2 = (float*)(param_1 + 0x94);
  iVar3 = 6;
  do {
    iVar3--;
    // dot product of plane normal pfVar2[0..2] with point param_2[0..2] minus plane-d pfVar2[3]
    fVar1 = pfVar2[2]*param_2[2] + *pfVar2**param_2 + pfVar2[1]*param_2[1] - pfVar2[3];
    if (param_2[3] < fVar1) return 0;   // outside (beyond far side)
    if (-param_2[3] < fVar1) uVar4 = 1; // partial intersection
    pfVar2 += 5;                         // next plane (stride 5 floats = 20 bytes)
  } while (iVar3 != 0);
  return uVar4; // 2=fully inside, 1=partial, 0=outside
}
```
**Analysis:** Frustum/sphere containment test. Tests a point (param_2, 3 floats + radius at param_2[3]) against 6 planes at param_1+0x94, stride 5 floats (normal xyz + d + padding). Returns: 0=outside, 1=partial, 2=fully inside. This is a standard 6-plane frustum test or sphere-vs-AABB test. Called by FUN_0047c160 (camera-path node loop) as spatial containment test for vehicle vs camera path node range.  
**Classification: C1** — 6-plane spatial containment test, fully characterized.

### D-1557 — FUN_00491340 (0x00491340)
**Body:** 0x00491340..0x00491482 (0x142 bytes)  
**Decompilation (key literals):**
```c
FUN_004671a0(0);
pfVar5 = (float*)FUN_00467210(0);  // vehicle sub-obj ptr
if (DAT_00771534 != 0) {           // enabled flag at 0x00771534
  // outer loop: local_10 = 0, 0x1c00, 0x2800, ... while < 0x7000 (4 iters, stride 0x1c00)
  // inner loop: iVar6 = 0..0xe0-1 (224 entries), pfVar8 stride +8 floats (32 bytes)
  // FUN_004c39b0(&local_c, pfVar8+3): normalize particle velocity vector pfVar8[3..5]
  // DAT_006146b4 (0x006146b4) — scale constant used as friction/drag factor
  // pfVar8[0..2] -= local_c..local_4 (position -= normalized_velocity * drag)
  // pfVar8[6] = frame counter +1 per tick; if >7: reset, reposition from path table
  // Reposition: reads from DAT_0077152c (0x0077152c) path table, stride 12 bytes
  // Matrix multiply: pfVar5[0..15] — 4x4 matrix from FUN_00467210(0)
}
```
**Analysis:** Camera mode A (DAT_007f108b==0 path per FUN_00491490). Per-frame particle-system update for a 4-layer × 224-entry particle array at DAT_00771530 (0x00771530). Each particle: position[0..2], ??? [3], velocity[3..5], frame_counter[6], ??? [7] (stride 8 floats). On frame_counter > 7: respawns from path table at DAT_0077152c (path node positions), transformed by vehicle 0's frame matrix from FUN_00467210(0). Drag applied per tick via DAT_006146b4.  
**Classification: C1** — camera-attached particle system update (camera mode A): per-frame particle motion + boundary respawn.  
**New DEFERRED D-3649:** DAT_00771530 (0x00771530) — particle array base pointer; layout: 4 layers × 0x1c00 bytes each; inner stride 8 floats (32 bytes) = 224 entries per layer.  
**New UNCERTAINTY U-1248:** Particle array layout at DAT_00771530: outer stride 0x1c00, inner stride 8 floats (32 bytes), 0xe0 (224) entries per layer; [6] is frame counter reset threshold 7; field [3..5] treated as velocity by FUN_004c39b0 call; remaining fields uncharacterized.

### D-1558 — FUN_004910c0 (0x004910c0)
**Body:** 0x004910c0..0x00491331 (0x271 bytes)  
**Decompilation (key literals):**
```c
iVar6 = FUN_004671a0(0);
uVar10 = *(undefined4*)(iVar6 + 0x84);  // save camera +0x84 value
pfVar7 = (float*)FUN_00467210(0);        // vehicle sub-obj frame
fVar3 = *(float*)(iVar6 + 0x68);         // camera viewport width (at +0x68)
fVar4 = *(float*)(iVar6 + 0x6c);         // camera viewport height (at +0x6c)
local_4 = 0x3dcccccd;                    // ~0.1 in float
if (DAT_00771534 != 0) {
  uVar8 = FUN_004671a0(0, 0x3f4ccccd);  // set camera value to ~0.8
  FUN_004c1b10(uVar8);                   // apply projection
  // loop: 4 layers × 224 particles, stride 8 floats (same layout as FUN_00491340)
  // FUN_004c39b0(&local_1c, pfVar11+3) — normalize velocity
  // Apply drag, move position
  // FUN_004671a0(0, &local_10) — test particle position against camera
  // FUN_004c1b40(uVar8) — containment test; if outside (0): respawn
  //   Respawn: FUN_00472650 (random float in range), position from frame matrix pfVar7
  //   Update counters: DAT_0086a480, DAT_0086a484, DAT_0086a488 (wrap at 0x1ef=495)
  uVar10 = FUN_004671a0(0, uVar10);
  FUN_004c1b10(uVar10);  // restore camera +0x84 value
}
```
**Analysis:** Camera mode B (DAT_007f108b != 0 path). Same particle system as FUN_00491340 but with camera containment test. Temporarily sets camera to ~0.8 view value, tests each particle against camera frustum using FUN_004c1b40; if outside, respawns at random offset from vehicle frame (FUN_00472650 random float ±viewport range). Updates wrap counters at DAT_0086a480/a484/a488 (mod 496). Restores camera state.  
**Classification: C1** — camera mode B particle update: same particle array, adds camera-frustum boundary respawn.  
**New DEFERRED D-3650:** FUN_00472650 (0x00472650) — returns random float in [param_1, param_2] range.

### D-1548 additional callee — FUN_00471ac0 (0x00471ac0)
This was S-0552 (depth-2 of FUN_00471ec0). Already decomped for completeness.

**Body:** 0x00471ac0..0x00471ce2 (0x222 bytes)  
**Decompilation (key literals):**
```c
FUN_0040e180(&local_10, local_4);        // get near/far vehicle indices
local_1c = FUN_00407a20(local_10);       // get tick counter for near vehicle
if ((int)local_1c < 0) local_1c = 0;
// outer loop: local_14 = 0..DAT_006905c8-1, piVar10 stride 0x23 ints
// piVar10[2] != 0: entry active
// *piVar10 = frame index; piVar10[1] = frame count
// iVar1 = piVar10[*piVar10 + -0x10]: lookup at base-0x10 offset
// if iVar1 >= 2000: dispatch via function table at DAT_00690ab0, stride 0x21 entries
// Inner: FUN_00426020() — gets current camera/scene object
// function table entry at (iVar1-2000)*0x21 called with camera object
// piVar10[2] = 0: clear active flag
// Per-entry sub-loop: FUN_0047d080, FUN_0047d100 — set visibility
// FUN_0047ce40(puVar9[-4]): AI polygon resolver
```
**Analysis:** Camera-anim trigger dispatcher. Called when a camera-anim trigger fires (from FUN_00471ec0). Gets near/far vehicle tick counters via FUN_0040e180 + FUN_00407a20. Iterates DAT_006905c8 (0x006905c8) camera-anim objects at DAT_0069064c (0x0069064c), stride 0x23 ints. For each active entry: advances frame counter (wraps at entry's frame count), dispatches per-frame function from table at DAT_00690ab0 (0x00690ab0) indexed by (ID-2000)*0x21. Sets visibility flags per camera-anim child objects via FUN_0047d080 + FUN_0047d100. Resets particle path nodes via FUN_0047ce40.  
**Classification: C1** — camera-anim sequence dispatcher: per-anim-event frame advance + function dispatch + visibility control.  
**New DEFERRED D-3651:** FUN_00426020 (0x00426020) — gets current scene/camera object; no args; returns handle.  
**New DEFERRED D-3652:** FUN_0047d080 (0x0047d080) — sets visibility on camera-anim object; (id, flag).  
**New DEFERRED D-3653:** FUN_0047d100 (0x0047d100) — secondary visibility setter; (id, flag).  
Cap count: 3 new DEFERRED here (D-3651..D-3653), combined with prior = already pushed to cont1.

### FUN_00471780 (0x00471780) — S-0551 (per-entry updater in outer loop of FUN_00471ec0)
**Body:** 0x00471780..0x00471abe (0x33e bytes)  
**Decompilation (key literals):**
```c
// param_2+0x250c: count of camera-anim path entries
// Loop: pfVar8 starting at param_2+0x2444, stride 7 floats (28 bytes)
// pfVar8[-1] != 0: entry active
// pfVar8[-6]: entry index for FUN_0047d150 and FUN_0047d130
// pfVar8[-2]: path node index → lookup at param_1+0x105b0 + node*4
// pfVar8[-4] == 0: FUN_004b47e0 else FUN_004752f0 (same dispatch as FUN_004756e0)
// *pfVar8: accumulated position along path
// pfVar8[-3] == 0: branch for Y-velocity calculation
// FUN_004c4dc0(local_40, iVar4): extracts 4x4 matrix from path node
// FUN_004c3d60(local_c0, &local_50, local_40): transforms position vector by matrix
// Clamps local_c0[0] to ±0.95 range
// FUN_0055b650(piVar3, &local_a4, local_90): applies force/spring to physics object
// DAT_005cc72c (0x005cc72c): force scale constant
// DAT_007f100c (0x007f100c): camera follow time constant
// FUN_0055c4a0(local_94, 0x3c23d70a): sets spin constant on physics obj (~0.01f)
```
**Analysis:** Camera-path physics spring updater. For each active path entry (stride 7 floats at param_2+0x2444): evaluates position along path node (using FUN_004752f0 or FUN_004b47e0), transforms to local frame via FUN_004c4dc0 + FUN_004c3d60, applies spring force to attached physics object (FUN_0055b650) with clamped heading ±0.95. Uses constants at DAT_005cc72c (spring force scale) and DAT_007f100c (follow time constant). Related to camera spline-follow spring physics.  
**Classification: C1** — camera-path spring physics updater per anim entry.  
**New DEFERRED D-3654:** FUN_0047d150 (0x0047d150) — returns something from entry index (like FUN_0047d130 but different).  
**New DEFERRED D-3655:** FUN_004c4dc0 (0x004c4dc0) — extracts 4x4 matrix from path node ptr.  
**New DEFERRED D-3656:** FUN_004c3d60 (0x004c3d60) — transforms position vector by matrix.  
**New DEFERRED D-3657:** FUN_0055b650 (0x0055b650) — applies spring/force to physics object.  
**New DEFERRED D-3658:** FUN_0055c4a0 (0x0055c4a0) — sets spin constant on physics obj.  
**New DEFERRED D-3659:** FUN_0055ac50 (0x0055ac50) / FUN_0055ac00 (0x0055ac00) / FUN_0055ad30 (0x0055ad30) — audio or physics setup calls.  
Cap: >3 new DEFERRED → push all D-3654..D-3659 to camera_follow_d2-cont1.

### FUN_0047bb10 (0x0047bb10) — S-0555 (per-node computation in FUN_0047c160)
**Body:** 0x0047bb10..0x0047bc81 (0x171 bytes)  
**Decompilation (key literals):**
```c
// param_1: node ptr; *(param_1+0x148): edge count
// loop: uVar7=0..edge_count-1, pbVar8=param_1+0xf0 stride 4 bytes (edge index pairs)
// FUN_0047ba20(iVar4, uVar7, param_2, &local_28): edge intersection test
// if hit: local_28=0 → bVar1=*pbVar8, bVar2=pbVar8[1]; else swap
// pfVar6 = param_1 + (bVar1+0xc)*0xc: vertex position lookup (stride 12 bytes, base offset 0xc*12=0x90)
// Accumulate: local_24+=*pfVar6, local_20+=pfVar6[1], local_1c+=pfVar6[2]
// FUN_00477e60(param_3, param_2, pfVar6, param_1+(bVar2+0xc)*0xc): per-edge compute
// Final: fVar3 = 1.0/local_2c; local_c..local_4 = centroid delta from param_2
// FUN_00477f00(local_2c*0x10 + param_3, param_1+0x138, &local_c): output centroid+offset
// *param_4 = local_2c (hit count output)
```
**Analysis:** Camera-path node edge-intersection query. Tests camera-path node edges against point param_2 (vehicle position). For each intersecting edge: accumulates vertex positions to compute centroid. Calls FUN_00477e60 per intersecting edge with both endpoint vertices. Outputs centroid delta and hit count. Used by FUN_0047c160 per node after containment test passes.  
**Classification: C1** — node edge intersection + centroid computation.  
**New DEFERRED D-3660:** FUN_0047ba20 (0x0047ba20) — edge intersection test: (node, edge_idx, point_ptr, &side_flag).  
**New DEFERRED D-3661:** FUN_00477e60 (0x00477e60) — per-edge computation with two vertex ptrs.  
**New DEFERRED D-3662:** FUN_00477f00 (0x00477f00) — output centroid+offset result.

### FUN_004c4a50 (0x004c4a50) — callee of FUN_004c4d20
**Body:** 0x004c4a50..0x004c4d1c (0x2cc bytes)  
**Analysis:** Rodrigues rotation matrix builder. Builds rotation matrix from axis (param_2[0..2]), sin=param_4, 1-cos=param_3. Combine mode param_5: 0=overwrite (copy 16 floats to param_1), 1=pre-multiply, 2=post-multiply via RW vtable at DAT_007d4028+8+DAT_007d3ff8. Error string literal at 0x0061811c: `"Invalid_combination_type"`. Matches RwMatrixRotate combine modes.  
**Classification: C1** — confirmed RwMatrixRotate (or equivalent) with 3 combine modes.

### FUN_004c3b90 (0x004c3b90) — rsqrt helper
**Body:** 0x004c3b90..0x004c3be6 (0x56 bytes)  
**Analysis:** Fast inverse square root via RW lookup table at DAT_007d3ffc+4+DAT_007d3ff8. Single float input, returns float10. Guard: if input==0, returns 0. Uses bit-manipulation RSqrt approximation table.  
**Classification: C1** — RW rsqrt approximation, confirmed by table access pattern.

---

## Classification Summary

| RVA | Name | Prior Status | This Session | Notes |
|---|---|---|---|---|
| 0x0042b930 | FUN_0042b930 | S-0549 D-1559 | C1 confirmed | game-mode-state getter |
| 0x0042f510 | FUN_0042f510 | S-0550 D-1560 | C1 confirmed | alt-vehicle-ptr getter |
| 0x004756e0 | FUN_004756e0 | S-0544 D-1540 | C1 | camera node transform updater |
| 0x00475010 | FUN_00475010 | S-0545 D-1541 | C1 [UNCERTAIN] | exception/cleanup wrapper |
| 0x004c4d20 | FUN_004c4d20 | S-0546 D-1542 | C1 | axis-angle rotation builder |
| 0x004c3dc0 | FUN_004c3dc0 | S-0547 D-1543 | C1 [UNCERTAIN] | RW vtable vector-transform |
| 0x004c39b0 | FUN_004c39b0 | S-0548 D-1544 | C1 | RW vector normalize |
| 0x004a2c48 | FUN_004a2c48 | D-1545 | C1 | CRT float→int64 round |
| 0x004924c0 | FUN_004924c0 | D-1546 | C1 | 3-byte global RGB setter |
| 0x004c1b10 | FUN_004c1b10 | D-1547 | C1 | RW camera projection setter |
| 0x0040e180 | FUN_0040e180 | D-1548 | C1 | vehicle-pair max-distance finder |
| 0x00407a40 | FUN_00407a40 | D-1549 | C1 | per-vehicle tick counter A getter |
| 0x00407a20 | FUN_00407a20 | D-1550 | C1 | per-vehicle tick counter B getter |
| 0x0047ce80 | FUN_0047ce80 | S-0553 D-1551 | C1 | cam-anim entry ID getter (0..199) |
| 0x0047ce00 | FUN_0047ce00 | S-0554 D-1552 | C1 | cam-anim entry flags getter (0..199) |
| 0x0047d130 | FUN_0047d130 | D-1553 | C1 | cam-anim inner-ref getter (0..199) |
| 0x0057c210 | FUN_0057c210 | D-1554 | C1 | anim-object pointer-table getter |
| 0x004c0ed0 | FUN_004c0ed0 | D-1555 | C1 | RW frame dirty-check + offset+0x50 |
| 0x004c1b40 | FUN_004c1b40 | D-1556 | C1 | 6-plane spatial containment test |
| 0x00491340 | FUN_00491340 | D-1557 | C1 | camera mode A particle updater |
| 0x004910c0 | FUN_004910c0 | D-1558 | C1 | camera mode B particle updater |
| 0x00471ac0 | FUN_00471ac0 | S-0552 | C1 | camera-anim trigger dispatcher |
| 0x00471780 | FUN_00471780 | S-0551 | C1 | camera-path spring physics updater |
| 0x0047bb10 | FUN_0047bb10 | S-0555 | C1 | node edge-intersection + centroid |
| 0x004c4a50 | FUN_004c4a50 | (callee) | C1 | RW Rodrigues rotation matrix builder |
| 0x004c3b90 | FUN_004c3b90 | (callee) | C1 | RW rsqrt approximation |
| 0x004c1a70 | FUN_004c1a70 | (callee) | C1 | camera projection coefficients recompute |

---

## New UNCERTAINTIES (U=1247..1266 range)

| ID | RVA | Type | Evidence Gap | Resolution |
|---|---|---|---|---|
| U-1247 | 0x004c3dc0 FUN_004c3dc0 | structural | DAT_007d3ff8 vtable slot at +0x10: candidate RwV3dTransformPoints unconfirmed | reference_to DAT_007d3ff8 initializer; enumerate slot at +0x10 |
| U-1248 | 0x00771530 particle array | structural | Particle array layout at DAT_00771530: 4 layers × 0x1c00, stride 8 floats (32 bytes), 224 entries/layer; fields [3..5]=velocity, [6]=frame counter, [7] and position layout uncharacterized | decomp FUN_00491340 caller + FUN_004910c0 caller to trace write sites; cross-ref DAT_00771530 |

---

## New STUBS (S=1240..1259 range)

All prior camera_follow S-0540..S-0555 stubs can be CLEARED once re-classify is run, as all have been fully characterized structurally in this session:
- S-0540 (0x0046d320): not in this session's scope (parent FUN_0040b090 D-session)
- S-0541 (0x0046d360): not in this session's scope
- S-0542 (0x0040aef0): not in this session's scope
- S-0543 (0x0055dec0): not in this session's scope
- **S-0544 (0x004756e0): CLEAR** — C1 characterized
- **S-0545 (0x00475010): CLEAR** — C1 characterized (uncertain inner body, structure known)
- **S-0546 (0x004c4d20): CLEAR** — C1 characterized
- **S-0547 (0x004c3dc0): CLEAR** — C1 characterized (U-1247 filed)
- **S-0548 (0x004c39b0): CLEAR** — C1 characterized
- **S-0549 (0x0042b930): CLEAR** — C1 confirmed
- **S-0550 (0x0042f510): CLEAR** — C1 confirmed
- **S-0551 (0x00471780): CLEAR** — C1 characterized
- **S-0552 (0x00471ac0): CLEAR** — C1 characterized
- **S-0553 (0x0047ce80): CLEAR** — C1 confirmed
- **S-0554 (0x0047ce00): CLEAR** — C1 confirmed
- **S-0555 (0x0047bb10): CLEAR** — C1 characterized

No new S-1240..S-1259 stubs generated: all depth-3 callees go to DEFERRED.

---

## New DEFERRED rows for camera_follow_d2-cont1 (D=3640..D-3699 range)

| ID | RVA | Caller | Role / evidence | Bucket |
|---|---|---|---|---|
| D-3640 | 0x004752f0 FUN_004752f0 | FUN_004756e0 | computes 64-byte transform block; path mode 0 (linear?) | camera_follow_d2-cont1 |
| D-3641 | 0x004b47e0 FUN_004b47e0 | FUN_004756e0, FUN_00471780 | computes 64-byte transform block; path mode 1 (spline?) | camera_follow_d2-cont1 |
| D-3642 | 0x004e66d0 FUN_004e66d0 | FUN_00474fb0 via FUN_00475010 | exception/cleanup frame dispatcher; 3 args: object, LAB_00474f90, param_2 | camera_follow_d2-cont1 |
| D-3643 | 0x004c0e50 FUN_004c0e50 | FUN_004c1b10 | called when camera attached frame != 0; role: RW frame dirty-flag update | camera_follow_d2-cont1 |
| D-3644 | 0x0046c7b0 FUN_0046c7b0 | FUN_0040e180 | vehicle-active test; returns 1 if vehicle index active | camera_follow_d2-cont1 |
| D-3645 | 0x0046cbb0 FUN_0046cbb0 | FUN_0040e180 | fills local_38 field per vehicle; role unknown; depth-3 of FUN_0040e180 | camera_follow_d2-cont1 |
| D-3646 | 0x0046d4a0 FUN_0046d4a0 | FUN_0040e180 | gets position struct ptr for vehicle index into local_34; depth-3 | camera_follow_d2-cont1 |
| D-3647 | 0x004c3ac0 FUN_004c3ac0 | FUN_0040e180 | 3D vector magnitude; takes float[3] ptr; returns float | camera_follow_d2-cont1 |
| D-3648 | 0x004d8350 FUN_004d8350 | FUN_004c0ed0 | conditional call on frame dirty-bit; takes sub-ptr at offset +0xa0; role unknown | camera_follow_d2-cont1 |
| D-3649 | DAT_00771530 particle array | FUN_00491340, FUN_004910c0 | particle array base; layout partially known (see U-1248); not a function | camera_follow_d2-cont1 |
| D-3650 | 0x00472650 FUN_00472650 | FUN_004910c0 | random float in [param_1, param_2] range | camera_follow_d2-cont1 |
| D-3651 | 0x00426020 FUN_00426020 | FUN_00471ac0 | zero-arg; returns current scene/camera object handle | camera_follow_d2-cont1 |
| D-3652 | 0x0047d080 FUN_0047d080 | FUN_00471ac0 | sets visibility flag on camera-anim object; (id, flag) | camera_follow_d2-cont1 |
| D-3653 | 0x0047d100 FUN_0047d100 | FUN_00471ac0 | secondary visibility setter on camera-anim object; (id, flag) | camera_follow_d2-cont1 |
| D-3654 | 0x0047d150 FUN_0047d150 | FUN_00471780 | returns value from entry index; sibling of FUN_0047d130 | camera_follow_d2-cont1 |
| D-3655 | 0x004c4dc0 FUN_004c4dc0 | FUN_00471780 | extracts 4x4 matrix from path node ptr into local_40 | camera_follow_d2-cont1 |
| D-3656 | 0x004c3d60 FUN_004c3d60 | FUN_00471780 | transforms position vector by 4x4 matrix; (out, in, matrix) | camera_follow_d2-cont1 |
| D-3657 | 0x0055b650 FUN_0055b650 | FUN_00471780 | applies spring/force to physics object; (physics_obj, force_vec, anchor_ptr) | camera_follow_d2-cont1 |
| D-3658 | 0x0055c4a0 FUN_0055c4a0 | FUN_00471780 | sets spin constant on physics object; (obj, 0x3c23d70a~0.01f) | camera_follow_d2-cont1 |
| D-3659 | 0x0055ac50 FUN_0055ac50 + 0x0055ac00 FUN_0055ac00 + 0x0055ad30 FUN_0055ad30 | FUN_00471780 | three calls on audio/physics obj; roles unknown | camera_follow_d2-cont1 |
| D-3660 | 0x0047ba20 FUN_0047ba20 | FUN_0047bb10 | edge intersection test: (node, edge_idx, point_ptr, &side_flag) | camera_follow_d2-cont1 |
| D-3661 | 0x00477e60 FUN_00477e60 | FUN_0047bb10 | per-edge computation; args: (param_3_ptr, param_2_ptr, vertex0_ptr, vertex1_ptr) | camera_follow_d2-cont1 |
| D-3662 | 0x00477f00 FUN_00477f00 | FUN_0047bb10 | output centroid+offset result; args: (hit_count*0x10+param_3, node+0x138, &centroid_delta) | camera_follow_d2-cont1 |

Total: 23 new DEFERRED rows (D-3640..D-3662), all pushed to camera_follow_d2-cont1 bucket.  
Cap count rule: FUN_0040e180 alone generated 4 rows; FUN_00471780 generated 6 rows; FUN_00471ac0 generated 3 rows; FUN_0047bb10 generated 3 rows. All over-cap functions have their callees pushed to cont1.

---

## Unresolved Questions

1. FUN_00475010 inner body (LAB_00474f50..LAB_00474f90): exception frame vs custom cleanup? — need listing_disassemble_range.
2. DAT_0067ecb0 semantic: state integer returned by FUN_0042b930 — what values can it take beyond 3? — need reference_to + caller trace.
3. DAT_006146b4 drag constant (0x006146b4): what sets this value? — need reference_to.
4. DAT_007f100c follow time constant (0x007f100c): set by which config path? — need reference_to.
5. Particle array struct full layout (U-1248).
6. FUN_004c3dc0 vtable slot (U-1247).

---

## Pool Slot Release

No lock was created by this session. Pool slot Mashed_pool6 remains available (no .lock file set).

---

## Notes on Re-Classify

The re-classify skill must be run by the parent session for:
- S-0544..S-0555 (clear stubs — all depth-2 functions fully characterized at C1)
- D-1540..D-1560 (mark as analyzed at C1)
- Add U-1247, U-1248
- Add D-3640..D-3662 to DEFERRED with bucket camera_follow_d2-cont1
All tracker mutations must go through re-classify per project rules. This session does NOT mutate hooks.csv / STUBS.md / UNCERTAINTIES.md / DEFERRED.md directly.
