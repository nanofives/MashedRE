// Mashed RE — B5e: RwpSolver-island CLUSTER 10 clean-room port (the per-contact constraint-row
// generator + its two body-relative-velocity/basis helpers).
//
// Anchored to MASHED.exe BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
// (Ghidra pool14, read_only, 2026-07-17). VERBATIM transcription of the 3 K10 functions from
// re/analysis/b5e/decomp/FUN_00xxxxxx.c, every body re-verified against live disasm before
// porting. Style/idiom follows RwpSolverCore7/8/9.cpp.
//
// Members (rva:size): 0056f350:1918 0056fb90:770 0056fea0:483 — 3 fns / 3171 B. Deps K2/K3/K9:
//   f350 -> FUN_0055b750(K3), FUN_0056fad0/f1f0/f0a0(K9); fb90 -> FUN_00546b10(K2). fea0 leaf.
//
// NO-GUESSING verifications against live pool14 disasm (2026-07-17):
//  1. Calling convention: all 3 end in a plain RET (0xC3) => __cdecl. RET bytes: 0x0056facd (f350),
//     0x0056fe91 (fb90), 0x00570082 (fea0).
//  2. FUN_0056f350's 9 CALLs are ALL DIRECT (E8 rel: 0055b750 x2 @0x0056f531/f54c, fad0 x3
//     @f5b1/fa10/fa7b, f1f0 x3 @f6bb/fa25/fa89, f0a0 @fabb) — NO indirect/vtable CALL despite the
//     census's indirect flag (that flag does not correspond to any `CALL reg/mem` here). No fn-ptr
//     to bind. The data-table lookups (&DAT_005e57a4 identity default, &DAT_005e57b0 indexed by
//     param_2[0x2b], &DAT_005e57c4/&DAT_005e57d0 axis basis pair) are bound by absolute address
//     (standalone rebind = KV/lane-end, like K9's DAT_005e5738).
//  3. FUN_0056f350's `local_78` is ONE contiguous stack buffer (undefined1[64] + the local_38..
//     local_18 tail): FUN_0056fad0 fills [0..0xe] (the jacobian rows), f350 writes the constraint
//     limits at [0x10..0x18], FUN_0056f1f0 consumes the whole row. Ported as `undefined4 local_78
//     [27]` with typed writes (float via *(float*)&, raw undefined4 direct) so the mixed float/int
//     layout matches; [0x19]/[0x1a] are left as the original leaves them (uninitialised stack —
//     the decomp writes no name there; f1f0 copies [0x1a] raw, a downstream-masked slot).
//  4. Float constants (memory_read): _DAT_005cc320=1.0, _DAT_005cc32c=0.5, _DAT_005cd03c=1.0e-4,
//     DAT_005d757c=0.0, _DAT_005e456c=-1.0e-3 (0xba83126f), _DAT_005cc9b4=0.99 (0x3f7d70a4),
//     _DAT_005e520c=0.70710678 (0x3f3504f3, = 1/sqrt2). Literals kept verbatim: 0x3f4ccccd (0.8f
//     default), 0x7f7fffff (FLT_MAX bits), 3.4028235e+38 (FLT_MAX), 4.2039e-45 (= int 3 tag).
//     SQRT() = x87 FSQRT -> 1.0f/sqrtf; ABS() = FABS -> fabsf.
//  5. FUN_0056fb90 / FUN_0056fea0 are pure float (cross-product + quaternion-to-matrix / quaternion
//     product). Data-flow transcribed verbatim; the multi-term sums keep the decomp order (Ghidra
//     printed the mixed-sign ones already parenthesized) under the <=1-ULP x87 floor -> U-9020.
//     fb90 reads a config body via pointer chains + calls FUN_00546b10 (K2) on the null-shape path.
#include "../Core/HookSystem.h"
#include <cmath>                    // sqrtf, fabsf — x87 FSQRT/FABS floor

namespace mashed_re {
namespace Collision {

typedef unsigned char  byte;
typedef unsigned short ushort;
typedef unsigned int   uint;
typedef unsigned int   undefined4;

#define _DAT_005cc320  (*(const float*)0x005cc320u)   // 1.0f
#define _DAT_005cc32c  (*(const float*)0x005cc32cu)   // 0.5f
#define _DAT_005cd03c  (*(const float*)0x005cd03cu)   // 1.0e-4f
#define DAT_005d757c   (*(const float*)0x005d757cu)   // 0.0f
#define _DAT_005e456c  (*(const float*)0x005e456cu)   // -1.0e-3f
#define _DAT_005cc9b4  (*(const float*)0x005cc9b4u)   // 0.99f
#define _DAT_005e520c  (*(const float*)0x005e520cu)   // 0.70710678f (1/sqrt2)
// data tables (absolute-address binds; note 2)
#define DAT_005e57a4   0x005e57a4u
#define DAT_005e57b0   0x005e57b0u
#define DAT_005e57c4   0x005e57c4u
#define DAT_005e57d0   0x005e57d0u

// --- extern callees (earlier clusters). ---
extern "C" void __cdecl FUN_0055b750(int *param_1,float *param_2,float *param_3);   // K3
extern "C" void __cdecl FUN_0056fad0(float *param_1,float *param_2,float *param_3,float *param_4,
                                     int param_5,int param_6);                       // K9
extern "C" void __cdecl FUN_0056f1f0(int *param_1,undefined4 *param_2);             // K9
extern "C" void __cdecl FUN_0056f0a0(int param_1);                                  // K9
extern "C" void __cdecl FUN_00546b10(float *param_1,float *param_2);               // K2

// ---------------------------------------------------------------------------
// 0x0056fb90  Relative-motion basis for a contact half: resolves the constraint body (or the null
//             shape via FUN_00546b10), builds the contact-frame 3x3 (from the two contact tangents)
//             + its double-cover matrix in param_5, and the relative angular offset. Header note 5.
// ---------------------------------------------------------------------------
extern "C" int * __cdecl FUN_0056fb90(uint *param_1,float *param_2,float *param_3,int *param_4,
                                      float *param_5)
{
  float fVar1,fVar2,fVar3,fVar4,fVar6,fVar7;
  ushort uVar5;
  float *pfVar8,*pfVar9;
  int *piVar10;
  float local_1c,local_18,local_14,local_10,local_c,local_8,local_4;

  uVar5 = (ushort)param_1[1];
  if (uVar5 == 0xffff) {
    piVar10 = (int *)0x0;
  }
  else {
    piVar10 = *(int **)(*(int *)(*param_1 + 0x10) + 4 + (uint)uVar5 * 0xc);
    if (piVar10 != (int *)0x0) {
      pfVar9 = (float *)(piVar10[1] * 0x40 + **(int **)(*piVar10 + 0x10));
      // (float)piVar10[N] are raw bit copies (MOV @0x0056fbd3), i.e. float reinterprets not converts.
      local_1c = *(float *)(piVar10 + 2);
      local_18 = *(float *)(piVar10 + 3);
      local_14 = *(float *)(piVar10 + 4);
      pfVar8 = (float *)(piVar10[1] * 0x10 + (*(int **)(*piVar10 + 0x10))[4]);
      local_10 = *pfVar8;
      local_c = pfVar8[1];
      local_8 = pfVar8[2];
      local_4 = pfVar8[3];
      goto LAB_0056fc5f;
    }
  }
  if (uVar5 == 0xffff) {
    pfVar9 = (float *)(~(0u - (uint)((*(byte *)(*(int *)(*param_1 + 0x5c) + 0x40) & 2) != 0)) & *param_1);
  }
  else {
    pfVar9 = *(float **)(*(int *)(*param_1 + 0x10) + 8 + (uint)uVar5 * 0xc);
  }
  local_1c = 0.0;
  local_18 = 0.0;
  local_14 = 0.0;
  FUN_00546b10(&local_10,pfVar9);
LAB_0056fc5f:
  *param_4 = (int)(pfVar9 + 0xc);
  param_3[3] = local_4 * param_2[3] -
               (param_2[2] * local_8 + local_10 * *param_2 + param_2[1] * local_c);
  *param_3 = param_2[2] * local_c - param_2[1] * local_8;
  param_3[1] = local_8 * *param_2 - param_2[2] * local_10;
  param_3[2] = param_2[1] * local_10 - local_c * *param_2;
  *param_3 = local_4 * *param_2 + *param_3;
  param_3[1] = param_2[1] * local_4 + param_3[1];
  param_3[2] = param_2[2] * local_4 + param_3[2];
  *param_3 = param_2[3] * local_10 + *param_3;
  param_3[1] = param_2[3] * local_c + param_3[1];
  fVar6 = param_3[1];
  fVar2 = param_2[3] * local_8 + param_3[2];
  param_3[2] = fVar2;
  fVar1 = *param_3;
  fVar7 = param_3[3];
  fVar3 = fVar2 * fVar2 + fVar6 * fVar6;
  *param_5 = _DAT_005cc320 - (fVar3 + fVar3);
  fVar3 = fVar6 * fVar1 + fVar7 * fVar2;
  param_5[1] = fVar3 + fVar3;
  fVar3 = fVar2 * fVar1 - fVar7 * fVar6;
  param_5[2] = fVar3 + fVar3;
  fVar3 = fVar6 * fVar1 - fVar7 * fVar2;
  param_5[4] = fVar3 + fVar3;
  fVar3 = fVar2 * fVar2 + fVar1 * fVar1;
  param_5[5] = _DAT_005cc320 - (fVar3 + fVar3);
  fVar3 = fVar1 * fVar7 + fVar2 * fVar6;
  param_5[0xc] = 0.0;
  param_5[0xd] = 0.0;
  param_5[0xe] = 0.0;
  *(undefined4 *)(param_5 + 3) = 3;              // was 4.2039e-45 (raw int 3 tag; K6-note-8 class)
  param_5[6] = fVar3 + fVar3;
  fVar3 = fVar7 * fVar6 + fVar2 * fVar1;
  param_5[8] = fVar3 + fVar3;
  fVar7 = fVar2 * fVar6 - fVar1 * fVar7;
  param_5[9] = fVar7 + fVar7;
  fVar1 = fVar6 * fVar6 + fVar1 * fVar1;
  param_5[10] = _DAT_005cc320 - (fVar1 + fVar1);
  local_1c = param_2[4] - local_1c;
  local_18 = param_2[5] - local_18;
  local_14 = param_2[6] - local_14;
  fVar1 = pfVar9[9];
  fVar6 = pfVar9[5];
  fVar7 = pfVar9[1];
  fVar2 = pfVar9[10];
  fVar3 = pfVar9[6];
  fVar4 = pfVar9[2];
  param_5[0xc] = pfVar9[0xc] + local_1c * *pfVar9 + local_18 * pfVar9[4] + local_14 * pfVar9[8];
  param_5[0xd] = pfVar9[0xd] + local_1c * fVar7 + local_18 * fVar6 + local_14 * fVar1;
  param_5[0xe] = pfVar9[0xe] + local_1c * fVar4 + local_18 * fVar3 + local_14 * fVar2;
  return piVar10;
}

// ---------------------------------------------------------------------------
// 0x0056fea0  Quaternion product param_2 (x) param_1 -> the double-cover 3x3 (param_3) and the
//             half-scaled 4x3 derivative block (param_4, each row * 0.5). Header note 5. Verbatim.
// ---------------------------------------------------------------------------
extern "C" void __cdecl FUN_0056fea0(float *param_1,float *param_2,float *param_3,float *param_4)
{
  float fVar1,fVar2,fVar3,fVar4,fVar5,fVar6,fVar7,fVar8,fVar9,fVar10,fVar11,fVar12,fVar13,fVar14,
        fVar15,fVar16;

  fVar9 = param_2[3] * param_1[3];
  fVar11 = *param_2 * param_1[3];
  fVar12 = param_2[1] * param_1[3];
  fVar14 = param_2[2] * param_1[3];
  fVar1 = *param_1 * param_2[3];
  fVar2 = *param_1 * *param_2;
  fVar16 = param_2[1] * *param_1;
  fVar3 = param_2[2] * *param_1;
  fVar13 = param_2[3] * param_1[1];
  fVar15 = *param_2 * param_1[1];
  fVar4 = param_2[1] * param_1[1];
  fVar5 = param_2[2] * param_1[1];
  fVar6 = param_1[2] * param_2[3];
  fVar7 = param_1[2] * *param_2;
  fVar8 = param_1[2] * param_2[1];
  fVar10 = param_1[2] * param_2[2];
  param_3[3] = fVar10 + fVar4 + fVar2 + fVar9;
  *param_3 = ((fVar8 - fVar1) + fVar11) - fVar5;
  param_3[1] = (fVar3 - (fVar7 + fVar13)) + fVar12;
  param_3[2] = ((fVar15 - fVar6) - fVar16) + fVar14;
  *param_4 = (((fVar1 - fVar11) - fVar5) + fVar8) * _DAT_005cc32c;
  param_4[1] = (((fVar13 - fVar12) + fVar3) - fVar7) * _DAT_005cc32c;
  param_4[2] = ((fVar6 - (fVar16 + fVar14)) + fVar15) * _DAT_005cc32c;
  param_4[3] = (((fVar2 + fVar9) - fVar10) - fVar4) * _DAT_005cc32c;
  param_4[4] = (fVar6 + fVar15 + fVar16 + fVar14) * _DAT_005cc32c;
  param_4[5] = (((fVar3 - fVar12) + fVar7) - fVar13) * _DAT_005cc32c;
  param_4[6] = (((fVar15 - fVar6) - fVar14) + fVar16) * _DAT_005cc32c;
  param_4[7] = (((fVar4 - fVar10) + fVar9) - fVar2) * _DAT_005cc32c;
  param_4[8] = (fVar8 + fVar5 + fVar1 + fVar11) * _DAT_005cc32c;
  param_4[9] = (fVar7 + fVar13 + fVar3 + fVar12) * _DAT_005cc32c;
  param_4[10] = (((fVar8 + fVar5) - fVar1) - fVar11) * _DAT_005cc32c;
  param_4[0xb] = (((fVar10 - fVar4) - fVar2) + fVar9) * _DAT_005cc32c;
  return;
}

// ---------------------------------------------------------------------------
// 0x0056f350  Per-manifold constraint-row generator: for each of param_2[0x2b] contact points,
//             resolves the two bodies (local_e4/e0), builds the relative contact vectors, calls
//             FUN_0055b750 (per-body point velocity, K3) and FUN_0056fad0 (jacobian basis, K9),
//             sets the friction/penetration limits into the local_78 row, emits it via
//             FUN_0056f1f0 (K9) up to 3x (normal + 2 friction dirs), then advances the batch via
//             FUN_0056f0a0. Header notes 2/3/4. Verbatim transcription.
// ---------------------------------------------------------------------------
extern "C" void __cdecl FUN_0056f350(int param_1,float *param_2,float param_3)
{
  float *pfVar1;
  float fVar2,fVar4,fVar5;
  undefined4 *puVar3,*puVar6,*puVar7;
  float *pfVar8,*pfVar9;
  int iVar10;
  float local_f0,local_ec;
  int *local_e4,*local_e0;
  float local_dc,local_d8,local_d4;
  float *local_d0;
  float local_cc,local_c8,local_c4,local_c0,local_bc,local_b8;
  float *local_b4;
  float local_b0,local_ac,local_a8,local_a4,local_a0,local_9c,local_98,local_94,local_90,local_8c,
        local_88,local_84,local_80,local_7c;
  // note 3: local_78 is one contiguous row buffer (fad0 fills [0..0xe], f350 sets limits at
  // [0x10..0x18], f1f0 consumes). Mixed float/undefined4 by slot.
  undefined4 local_78 [27];
  // tail-limit named locals (copied into local_78[0x10..0x18] before each f1f0 call).
  float local_38,local_34,local_20,local_1c;
  undefined4 local_30,local_2c,local_28,local_24,local_18;
  float local_4;

  local_b4 = (float *)DAT_005e57a4;
  local_d0 = (float *)DAT_005e57a4;
  // NB: param_2[0x2b]/[0x2d]/[0x2f] are RAW INT fields (Ghidra typed param_2 as float* so it
  // mis-rendered them as float; verified MOV loads @0x0056f366/f39d/f3c9). Only param_2[0x34]
  // (FMUL @f37b) is genuinely float.
  local_7c = *(float *)(DAT_005e57b0 + *(int *)(param_2 + 0x2b) * 4) * param_2[0x34];
  if (*(ushort *)(param_2 + 0x2e) == 0xffff) {
    local_e4 = (int *)0x0;
  }
  else {
    local_e4 = *(int **)(*(int *)(*(int *)(param_2 + 0x2d) + 0x10) + 4 +
                        (uint)*(ushort *)(param_2 + 0x2e) * 0xc);
  }
  if (*(ushort *)(param_2 + 0x30) == 0xffff) {
    local_e0 = (int *)0x0;
  }
  else {
    local_e0 = *(int **)(*(int *)(*(int *)(param_2 + 0x2f) + 0x10) + 4 +
                        (uint)*(ushort *)(param_2 + 0x30) * 0xc);
  }
  if (local_e4 != (int *)0x0) {
    local_b4 = (float *)(local_e4[1] * 0x40 + 0x30 + **(int **)(*local_e4 + 0x10));
  }
  if (local_e0 != (int *)0x0) {
    local_d0 = (float *)(**(int **)(*local_e0 + 0x10) + 0x30 + local_e0[1] * 0x40);
  }
  *(undefined4 *)(*(int *)(param_1 + 0xb8) + *(int *)(param_1 + 0xf8) * 4) = 0;
  *(int *)(param_1 + 0xbc) = *(int *)(param_1 + 0xbc) + 1;
  if (local_e4 == (int *)0x0) {
    iVar10 = -1;
  }
  else {
    iVar10 = (int)(short)local_e4[8];
  }
  *(int *)(*(int *)(param_1 + 0xac) + *(int *)(param_1 + 0xf8) * 8) = iVar10;
  if (local_e0 == (int *)0x0) {
    iVar10 = -1;
  }
  else {
    iVar10 = (int)(short)local_e0[8];
  }
  local_b0 = 0.0;
  *(int *)(*(int *)(param_1 + 0xac) + 4 + *(int *)(param_1 + 0xf8) * 8) = iVar10;
  if (*(int *)(param_2 + 0x2b) != 0) {          // raw int count (note above)
    pfVar9 = param_2 + 4;
    pfVar8 = local_d0;
    do {
      puVar3 = *(undefined4 **)(pfVar9 + 3);     // raw pointer at pfVar9+0xc (MOV @0x0056f4a2)
      local_a0 = 0.0; local_9c = 0.0; local_98 = 0.0;
      local_ac = 0.0; local_a8 = 0.0; local_a4 = 0.0;
      if ((puVar3 == (undefined4 *)0x0) || ((*(byte *)((int)puVar3 + 0x1c) & 8) == 0)) {
        pfVar1 = pfVar9 + -1;
        local_88 = pfVar9[-1] - *local_b4;
        local_84 = *pfVar9 - local_b4[1];
        local_80 = pfVar9[1] - local_b4[2];
        local_94 = *pfVar1 - *pfVar8;
        local_90 = *pfVar9 - pfVar8[1];
        local_8c = pfVar9[1] - pfVar8[2];
        if (local_e4 != (int *)0x0) {
          FUN_0055b750(local_e4,pfVar1,&local_a0);
        }
        if (local_e0 != (int *)0x0) {
          FUN_0055b750(local_e0,pfVar1,&local_ac);
        }
        local_cc = pfVar9[4] + (local_a0 - local_ac);
        local_c8 = pfVar9[5] + (local_9c - local_a8);
        local_c4 = pfVar9[6] + (local_98 - local_a4);
        FUN_0056fad0((float *)local_78,param_2,&local_88,&local_94,(int)local_e4,(int)local_e0);
        if (pfVar9[2] < DAT_005d757c == (pfVar9[2] == DAT_005d757c)) {
          local_38 = 0.0;
        }
        else {
          local_38 = pfVar9[2];
        }
        if ((puVar3 == (undefined4 *)0x0) || ((*(byte *)((int)puVar3 + 0x1c) & 4) == 0)) {
          fVar2 = param_2[0x32];
          local_30 = 0;
          local_2c = 0;
          local_18 = 0x3f4ccccd;          // 0.8f
        }
        else {
          local_30 = *puVar3;
          local_18 = puVar3[3];
          local_2c = puVar3[1];
          fVar2 = *(float *)(puVar3 + 2);
        }
        local_1c = 3.4028235e+38f;
        local_20 = 0.0;
        local_24 = 0x7f7fffff;
        local_28 = 0;
        fVar4 = param_2[2] * local_c4 + local_cc * *param_2 + param_2[1] * local_c8;
        if (param_3 * _DAT_005e456c <= fVar4) {
          local_34 = 0.0;
        }
        else {
          local_34 = -(fVar4 * fVar2);
        }
        // stage the tail limits, then emit the normal row.
        *(float *)&local_78[0x10] = local_38; *(float *)&local_78[0x11] = local_34;
        local_78[0x12] = local_30; local_78[0x13] = local_2c; local_78[0x14] = local_28;
        local_78[0x15] = local_24; *(float *)&local_78[0x16] = local_20;
        *(float *)&local_78[0x17] = local_1c; local_78[0x18] = local_18;
        FUN_0056f1f0((int *)param_1,local_78);
        pfVar8 = local_d0;
        if ((*(byte *)((int)param_2 + 0xce) & 1) == 0) {
          local_38 = 0.0; local_18 = 0; local_34 = 0.0; local_2c = 0; local_24 = 0; local_28 = 0;
          if ((puVar3 == (undefined4 *)0x0) || ((*(byte *)((int)puVar3 + 0x1c) & 1) == 0)) {
            local_f0 = param_2[1] * local_c4 - param_2[2] * local_c8;
            local_ec = param_2[2] * local_cc - local_c4 * *param_2;
            fVar2 = local_c8 * *param_2 - param_2[1] * local_cc;
            if (local_f0 * local_f0 + local_ec * local_ec + fVar2 * fVar2 < _DAT_005cd03c) {
              fVar2 = (*param_2 < 0.0f) ? -*param_2 : *param_2;   // ABS
              puVar7 = (undefined4 *)DAT_005e57c4;
              if (_DAT_005cc9b4 <= fVar2) puVar7 = (undefined4 *)DAT_005e57d0;
              puVar6 = (undefined4 *)DAT_005e57c4;
              if (_DAT_005cc9b4 <= fVar2) puVar6 = (undefined4 *)DAT_005e57d0;
              pfVar8 = (float *)DAT_005e57c4;
              local_f0 = *(float *)(puVar7 + 2) * param_2[1] - *(float *)(puVar6 + 1) * param_2[2];
              if (_DAT_005cc9b4 <= fVar2) pfVar8 = (float *)DAT_005e57d0;
              puVar7 = (undefined4 *)DAT_005e57c4;
              if (_DAT_005cc9b4 <= fVar2) puVar7 = (undefined4 *)DAT_005e57d0;
              puVar6 = (undefined4 *)DAT_005e57c4;
              local_ec = param_2[2] * *pfVar8 - *(float *)(puVar7 + 2) * *param_2;
              if (_DAT_005cc9b4 <= fVar2) puVar6 = (undefined4 *)DAT_005e57d0;
              pfVar8 = (float *)DAT_005e57c4;
              if (_DAT_005cc9b4 <= fVar2) pfVar8 = (float *)DAT_005e57d0;
              fVar2 = *(float *)(puVar6 + 1) * *param_2 - param_2[1] * *pfVar8;
            }
            fVar4 = fVar2 * fVar2;
            local_f0 = (_DAT_005cc320 / sqrtf(local_f0 * local_f0 + local_ec * local_ec + fVar4)) *
                       local_f0;
            local_ec = (_DAT_005cc320 / sqrtf(local_f0 * local_f0 + local_ec * local_ec + fVar4)) *
                       local_ec;
            fVar2 = (_DAT_005cc320 / sqrtf(local_ec * local_ec + local_f0 * local_f0 + fVar4)) *
                    fVar2;
            fVar5 = param_2[2] * local_ec - param_2[1] * fVar2;
            fVar4 = fVar2 * *param_2 - param_2[2] * local_f0;
            local_4 = param_2[1] * local_f0 - local_ec * *param_2;
            local_dc = (fVar5 + local_f0) * _DAT_005e520c;
            local_d8 = (fVar4 + local_ec) * _DAT_005e520c;
            local_d4 = (local_4 + fVar2) * _DAT_005e520c;
            local_c0 = (local_f0 - fVar5) * _DAT_005e520c;
            local_bc = (local_ec - fVar4) * _DAT_005e520c;
            local_b8 = (fVar2 - local_4) * _DAT_005e520c;
          }
          else {
            local_dc = *(float *)(puVar3 + 4);
            local_d8 = *(float *)(puVar3 + 5);
            local_d4 = *(float *)(puVar3 + 6);
            local_c0 = param_2[2] * local_d8 - param_2[1] * local_d4;
            local_bc = local_d4 * *param_2 - param_2[2] * local_dc;
            local_b8 = param_2[1] * local_dc - local_d8 * *param_2;
          }
          if ((puVar3 == (undefined4 *)0x0) || ((*(byte *)((int)puVar3 + 0x1c) & 2) == 0)) {
            local_30 = 0;
            local_1c = local_7c * param_2[0x31] * _DAT_005e520c;
            local_20 = local_1c;
          }
          else {
            local_30 = puVar3[9];
            local_1c = *(float *)(puVar3 + 8);
            local_20 = *(float *)(puVar3 + 8);
          }
          local_20 = -local_20;
          FUN_0056fad0((float *)local_78,&local_dc,&local_88,&local_94,(int)local_e4,(int)local_e0);
          *(float *)&local_78[0x10] = local_38; *(float *)&local_78[0x11] = local_34;
          local_78[0x12] = local_30; local_78[0x13] = local_2c; local_78[0x14] = local_28;
          local_78[0x15] = local_24; *(float *)&local_78[0x16] = local_20;
          *(float *)&local_78[0x17] = local_1c; local_78[0x18] = local_18;
          FUN_0056f1f0((int *)param_1,local_78);
          if ((puVar3 != (undefined4 *)0x0) && ((*(byte *)((int)puVar3 + 0x1c) & 2) != 0)) {
            local_30 = puVar3[0xb];
            local_1c = *(float *)(puVar3 + 10);
            local_20 = -*(float *)(puVar3 + 10);
          }
          FUN_0056fad0((float *)local_78,&local_c0,&local_88,&local_94,(int)local_e4,(int)local_e0);
          *(float *)&local_78[0x10] = local_38; *(float *)&local_78[0x11] = local_34;
          local_78[0x12] = local_30; local_78[0x13] = local_2c; local_78[0x14] = local_28;
          local_78[0x15] = local_24; *(float *)&local_78[0x16] = local_20;
          *(float *)&local_78[0x17] = local_1c; local_78[0x18] = local_18;
          FUN_0056f1f0((int *)param_1,local_78);
          pfVar8 = local_d0;
        }
      }
      local_b0 = (float)((int)local_b0 + 1);
      pfVar9 = pfVar9 + 10;
    } while ((uint)local_b0 < (uint)*(int *)(param_2 + 0x2b));   // raw int count
  }
  FUN_0056f0a0(param_1);
  return;
}

// --- gta-reversed-style hook registration — CLUSTER 10. ---
RH_ScopedInstall(FUN_0056f350, 0x0056f350);
RH_ScopedInstall(FUN_0056fb90, 0x0056fb90);
RH_ScopedInstall(FUN_0056fea0, 0x0056fea0);

}  // namespace Collision
}  // namespace mashed_re
