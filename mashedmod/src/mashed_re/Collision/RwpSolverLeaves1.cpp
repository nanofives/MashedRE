// Mashed RE — B5e: RwpSolver-island CLUSTER 1 first-draft clean-room port skeleton.
//
// Anchored to MASHED.exe BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
// (Ghidra pool, read_only, 2026-07-15). VERBATIM transcription of the 19 leaf functions of
// the solver-island static closure that have no island-internal callees (CLUSTER 1). Each
// body is copied character-for-character from re/analysis/b5e/decomp/FUN_00xxxxxx.c —
// operation order, integer widths, casts, and control flow are preserved as decompiled; no
// simplification, reordering, or expression folding. Style/idiom follows the B5c integrator
// port (Collision/RwpIntegrator.cpp): extern "C" per-function, // 0x00xxxxxx RVA comments,
// absolute-address macro bindings for game globals, RH_ScopedInstall registration block.
//
// This is a DRAFT skeleton (first-pass). Lines that translate a Ghidra construct without an
// exactly-faithful C++ form, or that carry an ambiguous decomp type, are marked
// // [VERIFY-DISASM]; x87 extended-precision (float10/double) intermediates are marked // [X87].
#include "../Core/HookSystem.h"
#include <cmath>

namespace mashed_re {
namespace Collision {

// --- Ghidra scalar types kept verbatim so each body transcribes character-for-character. ---
typedef unsigned char  byte;
typedef signed char    sbyte;
typedef unsigned short ushort;
typedef unsigned int   uint;
typedef unsigned char  undefined;
typedef unsigned short undefined2;
typedef unsigned int   undefined4;
typedef long double    float10;      // x87 80-bit extended (float10) — used by FUN_005667c0

// --- Ghidra ABS()/SQRT() intrinsics — translated helpers (no matching pattern in
//     RwpIntegrator.cpp). VERIFIED against disasm 2026-07-16 (pool0):
//     ABS  = x87 FABS (0x0056628a/91/9f... in FUN_00566200; 0x0056683d/44/4c in FUN_00566830)
//            -> fabsf: sign-bit clear, bit-exact incl. -0.0/NaN (a ternary would not be).
//     SQRT = x87 FSQRT (0x005667f7 in FUN_005667c0) on the 80-bit ST0 chain. MSVC has no
//            80-bit type (long double = 64-bit) -> accepted <=1-ULP x87 floor on this path
//            (project_phys_chain_float10_methodology). [X87]
static inline float   ABS(float v)     { return std::fabsf(v); }
static inline float10 SQRT(float10 v)  { return std::sqrtl(v); }            // [X87]

// --- CRT rand thunk (FUN_00564310) --------------------------------------------------------
// FUN_00564310 calls the static-linked CRT rand at 0x005c229b. Route the call through this
// file-local thunk that jumps to the ORIGINAL CRT rand so the RNG state stays shared with the
// running game — required for bit-identity under the diff-original .asi A/B. A standalone
// rebind (own rand state) is a later lane item.
static int (__cdecl* const s_rand_orig)(void) = (int(__cdecl*)(void))0x005c229bu;

// --- Game globals referenced by the bodies. Bound to their absolute addresses via macros
//     named exactly as the decomp prints them, so each body reads verbatim (RwpIntegrator.cpp
//     deref-macro idiom). See ## GLOBALS. ---
#define _DAT_005e5418     (*(float*)0x005e5418u)   // FUN_005641b0, FUN_00564310
#define _DAT_005cc318     (*(float*)0x005cc318u)   // FUN_005641b0, FUN_00564310
#define _DAT_005cc32c     (*(float*)0x005cc32cu)   // FUN_00566200
#define _DAT_005cc320     (*(float*)0x005cc320u)   // FUN_005667c0
#define PTR_DAT_005ceabc  (*(float*)0x005ceabcu)   // FUN_005667c0 — VERIFIED 2026-07-16: Ghidra
                                                   //   typed 0x005ceabc as a pointer, but the code
                                                   //   reads it as a FLOAT (FCOM float ptr
                                                   //   [0x005ceabc] @ 0x005667e2); bits 0x00800000
                                                   //   = 1.17549435e-38 (FLT_MIN threshold)

// ---------------------------------------------------------------------------
// 0x005601f0  Builds compressed output rows from a stride-16 source param_2 indexed by
//             param_5[0] (index array, count param_5[1]).
// ---------------------------------------------------------------------------
extern "C" void __cdecl FUN_005601f0(int *param_1,int param_2,undefined4 param_3,undefined4 param_4,int *param_5)

{
  undefined4 *puVar1;
  int iVar2;
  int iVar3;
  int iVar4;

  iVar4 = 0;
  if (param_5[1] != 0) {
    iVar2 = 0;
    do {
      iVar3 = iVar2 + 0x10;
      puVar1 = (undefined4 *)((*(uint *)(*param_5 + iVar4 * 4) >> 2) * 0x10 + param_2);
      iVar4 = iVar4 + 1;
      *(undefined4 *)(iVar2 + *param_1) = *puVar1;
      *(undefined4 *)(*param_1 + -0xc + iVar3) = puVar1[3];
      *(undefined4 *)(*param_1 + -8 + iVar3) = puVar1[6];
      *(undefined4 *)(*param_1 + -4 + iVar3) = puVar1[9];
      iVar2 = iVar3;
    } while (iVar4 != param_5[1]);
    param_1[1] = param_5[1];
    return;
  }
  param_1[1] = param_5[1];
  return;
}

// ---------------------------------------------------------------------------
// 0x00563e70  Inserts pair (param_2, param_3) into a hash-bucketed doubly-linked-list.
// ---------------------------------------------------------------------------
extern "C" void __cdecl FUN_00563e70(int param_1,ushort param_2,ushort param_3)

{
  ushort uVar1;
  ushort uVar2;
  ushort uVar3;
  int iVar4;
  int iVar5;
  int iVar6;

  uVar1 = *(ushort *)(param_1 + 0xc);
  if (uVar1 != 0xffff) {
    iVar4 = *(int *)(param_1 + 8);
    iVar5 = (uint)uVar1 * 8;
    uVar2 = *(ushort *)(iVar4 + 2 + iVar5);
    *(ushort *)(param_1 + 0xc) = uVar2;
    iVar6 = (uint)uVar2 * 8;
    *(undefined2 *)(param_1 + 0xc) = *(undefined2 *)(iVar6 + 2 + iVar4);
    *(ushort *)(iVar4 + 4 + iVar5) = param_3;
    *(ushort *)(*(int *)(param_1 + 8) + 6 + iVar5) = uVar2;
    *(ushort *)(iVar6 + 4 + *(int *)(param_1 + 8)) = param_2;
    *(ushort *)(iVar6 + 6 + *(int *)(param_1 + 8)) = uVar1;
    uVar3 = *(ushort *)((uint)param_2 * 8 + 2 + *(int *)(param_1 + 8));
    *(ushort *)(*(int *)(param_1 + 8) + 2 + iVar5) = uVar3;
    *(ushort *)(iVar5 + *(int *)(param_1 + 8)) = param_2;
    *(ushort *)(*(int *)(param_1 + 8) + (uint)uVar3 * 8) = uVar1;
    *(ushort *)(*(int *)(param_1 + 8) + 2 + (uint)param_2 * 8) = uVar1;
    uVar1 = *(ushort *)((uint)param_3 * 8 + 2 + *(int *)(param_1 + 8));
    *(ushort *)(iVar6 + 2 + *(int *)(param_1 + 8)) = uVar1;
    *(ushort *)(iVar6 + *(int *)(param_1 + 8)) = param_3;
    *(ushort *)(*(int *)(param_1 + 8) + (uint)uVar1 * 8) = uVar2;
    *(ushort *)(*(int *)(param_1 + 8) + 2 + (uint)param_3 * 8) = uVar2;
  }
  return;
}

// ---------------------------------------------------------------------------
// 0x00563f60  Bucket-clear: removes all nodes in the linked-list chain headed by param_2's
//             bucket and returns them to the free-list.
// ---------------------------------------------------------------------------
extern "C" void __cdecl FUN_00563f60(int param_1,ushort param_2)

{
  ushort uVar1;
  ushort uVar2;
  ushort uVar3;
  ushort uVar4;
  int iVar5;
  int iVar6;
  int iVar7;

  iVar5 = *(int *)(param_1 + 8);
  uVar1 = *(ushort *)(iVar5 + 2 + (uint)param_2 * 8);
  while (uVar1 != param_2) {
    iVar6 = (uint)uVar1 * 8;
    uVar2 = *(ushort *)(iVar6 + 2 + iVar5);
    uVar3 = *(ushort *)(iVar6 + 6 + iVar5);
    uVar4 = *(ushort *)(iVar6 + iVar5);
    *(ushort *)(iVar5 + (uint)uVar2 * 8) = uVar4;
    *(ushort *)(*(int *)(param_1 + 8) + 2 + (uint)uVar4 * 8) = uVar2;
    iVar5 = *(int *)(param_1 + 8);
    iVar7 = (uint)uVar3 * 8;
    uVar2 = *(ushort *)(iVar7 + 2 + iVar5);
    uVar4 = *(ushort *)(iVar7 + iVar5);
    *(ushort *)(iVar5 + (uint)uVar2 * 8) = uVar4;
    *(ushort *)(*(int *)(param_1 + 8) + 2 + (uint)uVar4 * 8) = uVar2;
    *(undefined2 *)(*(int *)(param_1 + 8) + 2 + iVar6) = *(undefined2 *)(param_1 + 0xc);
    *(ushort *)(*(int *)(param_1 + 8) + 2 + iVar7) = uVar1;
    *(ushort *)(param_1 + 0xc) = uVar3;
    iVar5 = *(int *)(param_1 + 8);
    uVar1 = *(ushort *)(iVar5 + 2 + (uint)param_2 * 8);
  }
  return;
}

// ---------------------------------------------------------------------------
// 0x00564040  Collects all unique pair-IDs that key param_2 is hashed with, filtered by
//             bitmap param_4. Stores into output buffer param_3.
// ---------------------------------------------------------------------------
extern "C" void __cdecl FUN_00564040(int param_1,ushort param_2,int param_3,int param_4,int param_5)

{
  ushort uVar1;
  ushort uVar2;
  int iVar3;
  uint uVar4;
  uint local_4;

  iVar3 = *(int *)(param_1 + 8);
  local_4 = 0;
  for (uVar1 = *(ushort *)(iVar3 + 2 + (uint)param_2 * 8); uVar1 != param_2;
      uVar1 = *(ushort *)((uint)uVar1 * 8 + 2 + iVar3)) {
    uVar2 = *(ushort *)((uint)uVar1 * 8 + 4 + iVar3);
    if ((*(uint *)(param_4 + (uint)(uVar2 >> 5) * 4) & 1 << ((byte)uVar2 & 0x1f)) == 0) {
      *(ushort *)(param_3 + local_4 * 2) = uVar2;
      local_4 = local_4 + 1;
    }
    iVar3 = *(int *)(param_1 + 8);
  }
  if (param_5 != 0) {
    while (1 < local_4) {
      local_4 = local_4 - 1;
      uVar4 = 0;
      if (local_4 != 0) {
        do {
          uVar1 = *(ushort *)(param_3 + uVar4 * 2);
          uVar2 = *(ushort *)(param_3 + 2 + uVar4 * 2);
          if (uVar2 < uVar1) {
            *(ushort *)(param_3 + uVar4 * 2) = uVar2;
            *(ushort *)(param_3 + 2 + uVar4 * 2) = uVar1;
          }
          uVar4 = uVar4 + 1;
        } while (uVar4 < local_4);
      }
    }
  }
  return;
}

// ---------------------------------------------------------------------------
// 0x005641b0  AABB-containment test with hysteresis. param_1/param_2 are 8-float AABBs
//             (min at +0..+8, max at +0x10..+0x18).
// ---------------------------------------------------------------------------
extern "C" bool __cdecl FUN_005641b0(float *param_1,float *param_2)

{
  return (byte)((*param_1 < param_2[4] * _DAT_005e5418 + *param_2 * _DAT_005cc318) +
                ((param_1[1] < param_2[5] * _DAT_005e5418 + param_2[1] * _DAT_005cc318) +
                (param_1[2] < param_2[6] * _DAT_005e5418 + param_2[2] * _DAT_005cc318) * '\x02') *
                '\x02' | (*param_2 * _DAT_005e5418 + param_2[4] * _DAT_005cc318 < param_1[4]) +
                         ((param_2[1] * _DAT_005e5418 + param_2[5] * _DAT_005cc318 < param_1[5]) +
                         (param_2[2] * _DAT_005e5418 + param_2[6] * _DAT_005cc318 < param_1[6]) *
                         '\x02') * '\x02') == 7;
}

// ---------------------------------------------------------------------------
// 0x00564310  Selects an octree octant (3-bit code in [0..7]) where param_2 (loose AABB)
//             overlaps param_3 (parent AABB). Uses CRT rand (via s_rand_orig thunk).
// ---------------------------------------------------------------------------
extern "C" uint __cdecl FUN_00564310(float *param_1,float *param_2,float *param_3)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  uint uVar7;
  uint uVar8;
  uint uVar9;
  uint uVar10;

  uVar7 = s_rand_orig();   // decomp: uVar7 = _rand();  (0x005c229b static-linked CRT rand)
  fVar1 = *param_3 * _DAT_005e5418 + param_3[4] * _DAT_005cc318;
  fVar2 = param_3[1] * _DAT_005e5418 + param_3[5] * _DAT_005cc318;
  fVar3 = param_3[2] * _DAT_005e5418 + param_3[6] * _DAT_005cc318;
  fVar4 = param_3[4] * _DAT_005e5418 + *param_3 * _DAT_005cc318;
  fVar5 = param_3[5] * _DAT_005e5418 + param_3[1] * _DAT_005cc318;
  fVar6 = param_3[6] * _DAT_005e5418 + param_3[2] * _DAT_005cc318;
  uVar8 = (uint)(fVar1 < param_2[4]) +
          ((uint)(fVar2 < param_2[5]) + (uint)(fVar3 < param_2[6]) * 2) * 2;
  uVar10 = (uint)(*param_2 < fVar4) +
           ((uint)(param_2[1] < fVar5) + (uint)(param_2[2] < fVar6) * 2) * 2;
  uVar9 = uVar10 & uVar8;
  if ((ushort)((ushort)uVar10 | (ushort)uVar8) != 7) {
    return 0xffff;
  }
  uVar8 = ~uVar9 & uVar8 | uVar7 & uVar9;
  param_1[4] = (float)(uVar8 & 1) * fVar1 + (float)(int)(1 - (uVar8 & 1)) * param_3[4];
  param_1[5] = (float)(uVar8 >> 1 & 1) * fVar2 + (float)(int)(1 - (uVar8 >> 1 & 1)) * param_3[5];
  uVar10 = -uVar8 - 1;
  uVar7 = (int)uVar10 >> 1 & 1;
  param_1[6] = (float)(uVar8 >> 2 & 1) * fVar3 + (float)(int)(1 - (uVar8 >> 2 & 1)) * param_3[6];
  *param_1 = (float)(uVar10 & 1) * fVar4 + (float)(int)(1 - (uVar10 & 1)) * *param_3;
  param_1[1] = (float)uVar7 * fVar5 + (float)(int)(1 - uVar7) * param_3[1];
  uVar10 = (int)uVar10 >> 2 & 1;
  param_1[2] = (float)uVar10 * fVar6 + (float)(int)(1 - uVar10) * param_3[2];
  return uVar8;
}

// ---------------------------------------------------------------------------
// 0x00565120  Writes the low 10 bits of an octant-table entry at
//             this + 0x9820 + (param_3 + param_2*10)*2.
// ---------------------------------------------------------------------------
extern "C" void __cdecl FUN_00565120(int param_1,uint param_2,uint param_3,ushort param_4)

{
  int iVar1;

  iVar1 = (param_3 & 0xffff) + (param_2 & 0xffff) * 10;
  *(ushort *)(param_1 + 0x9820 + iVar1 * 2) =
       *(ushort *)(param_1 + 0x9820 + iVar1 * 2) & 0xfc00 | param_4;
  return;
}

// ---------------------------------------------------------------------------
// 0x00565160  Octant-relocate helper (one of three, used by FUN_00564c80 octree-split).
// ---------------------------------------------------------------------------
extern "C" void __cdecl FUN_00565160(int param_1,uint param_2,uint param_3)

{
  uint *puVar1;
  int iVar2;

  iVar2 = (param_2 & 0xffff) + 0x79b;
  puVar1 = (uint *)(param_1 + iVar2 * 0x14);
  *(ushort *)(param_1 + 0x8820 + (param_3 & 0xffff) * 4) =
       *(ushort *)(param_1 + iVar2 * 0x14) & 0x3ff;
  *puVar1 = *puVar1 & 0xfffffc00 | param_3 & 0xffff;
  return;
}

// ---------------------------------------------------------------------------
// 0x005651b0  Octant-table primitive-count incrementer (matches FUN_00564c80 split-trigger).
// ---------------------------------------------------------------------------
extern "C" void __cdecl FUN_005651b0(int param_1,uint param_2,uint param_3)

{
  int iVar1;
  ushort uVar2;

  iVar1 = (param_3 & 0xffff) + (param_2 & 0xffff) * 10;
  uVar2 = *(ushort *)(param_1 + 0x9820 + iVar1 * 2);
  if ((uVar2 & 0x7c00) != 0x7c00) {
    *(ushort *)(param_1 + 0x9820 + iVar1 * 2) = uVar2 + 0x400;
  }
  return;
}

// ---------------------------------------------------------------------------
// 0x00565200  Reads per-primitive tight AABB at this + (param_2 & 0xffff)*0x20, expanded by
//             the scalar at this + 0xc014.
// ---------------------------------------------------------------------------
extern "C" void __cdecl FUN_00565200(int param_1,uint param_2,float *param_3)

{
  float *pfVar1;

  pfVar1 = (float *)((param_2 & 0xffff) * 0x20 + param_1);
  param_3[4] = pfVar1[4] + *(float *)(param_1 + 0xc014);
  param_3[5] = pfVar1[5] + *(float *)(param_1 + 0xc014);
  param_3[6] = pfVar1[6] + *(float *)(param_1 + 0xc014);
  *param_3 = *pfVar1 - *(float *)(param_1 + 0xc014);
  param_3[1] = pfVar1[1] - *(float *)(param_1 + 0xc014);
  param_3[2] = pfVar1[2] - *(float *)(param_1 + 0xc014);
  return;
}

// ---------------------------------------------------------------------------
// 0x00565550  Trivial accessor: return *(ushort*)(this + 0xb810 + (param_2 & 0xffff)*2).
// ---------------------------------------------------------------------------
extern "C" undefined2 __cdecl FUN_00565550(int param_1,uint param_2)

{
  return *(undefined2 *)(param_1 + 0xb810 + (param_2 & 0xffff) * 2);
}

// ---------------------------------------------------------------------------
// 0x00565ef0  Per-axis min/max merge of two 8-float AABBs (min +0, max +4) into param_1.
// ---------------------------------------------------------------------------
extern "C" void __cdecl FUN_00565ef0(float *param_1,float *param_2,float *param_3)

{
  float fVar1;

  if (param_2[4] < param_3[4] == (param_2[4] == param_3[4])) {
    fVar1 = param_3[4];
  }
  else {
    fVar1 = param_2[4];
  }
  param_1[4] = fVar1;
  if (param_3[5] < param_2[5]) {
    fVar1 = param_3[5];
  }
  else {
    fVar1 = param_2[5];
  }
  param_1[5] = fVar1;
  if (param_3[6] < param_2[6]) {
    fVar1 = param_3[6];
  }
  else {
    fVar1 = param_2[6];
  }
  param_1[6] = fVar1;
  if (*param_2 < *param_3) {
    fVar1 = *param_3;
  }
  else {
    fVar1 = *param_2;
  }
  *param_1 = fVar1;
  if (param_3[1] < param_2[1] == (param_3[1] == param_2[1])) {
    fVar1 = param_3[1];
  }
  else {
    fVar1 = param_2[1];
  }
  param_1[1] = fVar1;
  if (param_3[2] < param_2[2] != (param_3[2] == param_2[2])) {
    param_1[2] = param_2[2];
    return;
  }
  param_1[2] = param_3[2];
  return;
}

// ---------------------------------------------------------------------------
// 0x00565fa0  Builds an AABB spanning param_2 and param_3, inflated by param_4.
// ---------------------------------------------------------------------------
extern "C" void __cdecl FUN_00565fa0(float *param_1,float *param_2,float *param_3,float param_4)

{
  float fVar1;

  if (*param_3 <= *param_2) {
    param_1[4] = *param_3 - param_4;
    fVar1 = *param_2;
  }
  else {
    param_1[4] = *param_2 - param_4;
    fVar1 = *param_3;
  }
  *param_1 = param_4 + fVar1;
  if (param_3[1] <= param_2[1]) {
    param_1[5] = param_3[1] - param_4;
    fVar1 = param_2[1];
  }
  else {
    param_1[5] = param_2[1] - param_4;
    fVar1 = param_3[1];
  }
  param_1[1] = param_4 + fVar1;
  if (param_2[2] < param_3[2]) {
    param_1[6] = param_2[2] - param_4;
    param_1[2] = param_4 + param_3[2];
    return;
  }
  param_1[6] = param_3[2] - param_4;
  param_1[2] = param_4 + param_2[2];
  return;
}

// ---------------------------------------------------------------------------
// 0x00566200  Transforms an AABB (param_2) by a 4x3 matrix (param_3) into a re-fit AABB
//             (param_1); center/extent form scaled by _DAT_005cc32c.
// ---------------------------------------------------------------------------
extern "C" void __cdecl FUN_00566200(float *param_1,float *param_2,float *param_3)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float fVar8;

  fVar1 = (*param_2 - param_2[4]) * _DAT_005cc32c;
  fVar2 = (param_2[1] - param_2[5]) * _DAT_005cc32c;
  fVar3 = (param_2[2] - param_2[6]) * _DAT_005cc32c;
  fVar8 = ABS(*param_3) * fVar1 + ABS(param_3[2]) * fVar3 + ABS(param_3[1]) * fVar2;
  fVar7 = ABS(param_3[4]) * fVar1 + ABS(param_3[6]) * fVar3 + ABS(param_3[5]) * fVar2;
  fVar1 = ABS(param_3[8]) * fVar1 + ABS(param_3[10]) * fVar3 + ABS(param_3[9]) * fVar2;
  fVar6 = (param_2[4] + *param_2) * _DAT_005cc32c - param_3[0xc];
  fVar2 = (param_2[5] + param_2[1]) * _DAT_005cc32c - param_3[0xd];
  fVar4 = (param_2[6] + param_2[2]) * _DAT_005cc32c - param_3[0xe];
  fVar5 = param_3[2] * fVar4 + fVar6 * *param_3 + param_3[1] * fVar2;
  fVar3 = param_3[6] * fVar4 + param_3[4] * fVar6 + param_3[5] * fVar2;
  fVar2 = param_3[10] * fVar4 + param_3[8] * fVar6 + param_3[9] * fVar2;
  param_1[4] = fVar5 - fVar8;
  param_1[5] = fVar3 - fVar7;
  param_1[6] = fVar2 - fVar1;
  *param_1 = fVar8 + fVar5;
  param_1[1] = fVar7 + fVar3;
  param_1[2] = fVar1 + fVar2;
  return;
}

// ---------------------------------------------------------------------------
// 0x005667c0  Normalizes vec3 param_2 into param_1 (guarded by threshold PTR_DAT_005ceabc);
//             returns the x87 float10 scaled magnitude.
// ---------------------------------------------------------------------------
extern "C" float10 __cdecl FUN_005667c0(float *param_1,float *param_2)

{
  float10 fVar1;
  float10 fVar2;

  fVar2 = (float10)param_2[2] * (float10)param_2[2] +                                        // [X87]
          (float10)param_2[1] * (float10)param_2[1] + (float10)*param_2 * (float10)*param_2; // [X87]
  if ((float10)(float)PTR_DAT_005ceabc < fVar2) {              // [X87] [VERIFY-DISASM] PTR_ typed symbol
    fVar1 = (float10)_DAT_005cc320 / SQRT(fVar2);                                            // [X87]
    *param_1 = (float)(fVar1 * (float10)*param_2);                                           // [X87]
    param_1[1] = (float)((float10)param_2[1] * fVar1);                                       // [X87]
    param_1[2] = (float)((float10)param_2[2] * fVar1);                                       // [X87]
    return fVar1 * fVar2;                                                                    // [X87]
  }
  *param_1 = 0.0;
  param_1[1] = 0.0;
  param_1[2] = 1.0;
  return fVar2;
}

// ---------------------------------------------------------------------------
// 0x00566830  Builds a perpendicular vector to param_2 into param_1 (zeroes the
//             largest-magnitude axis, swaps/negates the other two).
// ---------------------------------------------------------------------------
extern "C" void __cdecl FUN_00566830(float *param_1,float *param_2)

{
  float fVar1;
  uint uVar2;
  uint uVar3;

  *param_1 = ABS(*param_2);
  param_1[1] = ABS(param_2[1]);
  fVar1 = ABS(param_2[2]);
  param_1[2] = fVar1;
  uVar2 = 0x21312300 >>
          ((((param_1[1] < *param_1) * '\x02' | fVar1 < *param_1) * '\x02' | fVar1 < param_1[1]) <<
          2) & 3;
  param_1[uVar2] = 0.0;
  uVar2 = 1 << (sbyte)uVar2 & 3;
  uVar3 = 1 << (sbyte)uVar2 & 3;
  param_1[uVar2] = param_2[uVar3];
  param_1[uVar3] = -param_2[uVar2];
  return;
}

// ---------------------------------------------------------------------------
// 0x005675d0  Zero-inits a solver-batch record at param_2 and seeds counts from param_3/4.
// ---------------------------------------------------------------------------
extern "C" void __cdecl FUN_005675d0(int param_1,int param_2,int param_3,undefined4 param_4)

{
  int iVar1;

  *(undefined4 *)(param_2 + 0x20) = 0;
  *(undefined4 *)(param_2 + 0x2c) = 0;
  *(undefined4 *)(param_2 + 0x38) = 0;
  *(undefined4 *)(param_2 + 0x14) = 0;
  *(undefined4 *)(param_2 + 0x44) = 0;
  *(undefined4 *)(param_2 + 8) = 0;
  *(undefined4 *)(param_2 + 0x50) = 0;
  *(undefined4 *)(param_2 + 0x74) = 0;
  *(undefined4 *)(param_2 + 0x68) = 0;
  iVar1 = (int)(param_3 + (param_3 >> 0x1f & 3U)) >> 2;
  *(undefined4 *)(param_2 + 0x7c) = param_4;
  *(int *)(param_2 + 0x8c) = iVar1;
  *(int *)(param_2 + 0x98) = iVar1;
  *(int *)(param_2 + 0x80) = iVar1 * 2;
  *(int *)(param_2 + 0x84) = iVar1;
  *(int *)(param_1 + 4) = iVar1;
  *(int *)(param_1 + 0x10) = iVar1;
  return;
}

// ---------------------------------------------------------------------------
// 0x005684c0  Appends a stride-0x28 node into the array at param_1+8, wires the
//             doubly-linked list through param_2/param_3 and two sub-lists (param_4/param_5).
// ---------------------------------------------------------------------------
extern "C" void __cdecl FUN_005684c0(int param_1,int *param_2,int *param_3,undefined4 *param_4,undefined4 *param_5)

{
  undefined4 *puVar1;
  int iVar2;

  iVar2 = *(int *)(param_1 + 4);
  *(int *)(param_1 + 4) = iVar2 + 1;
  puVar1 = (undefined4 *)(param_1 + 8 + iVar2 * 0x28);
  *puVar1 = 1;
  if ((param_3 == (int *)0x0) || (param_2 == param_3)) {
    *param_2 = (int)param_2;
    param_2[1] = (int)puVar1;
    param_2[2] = 0;
    puVar1[2] = (undefined4)param_2;   // 32-bit MOV store of ECX (ptr) — disasm 0x005684fb/0x00568512
    puVar1[1] = 1;
  }
  else {
    *param_2 = (int)param_2;
    param_2[1] = (int)puVar1;
    param_2[2] = (int)param_3;
    *param_3 = (int)param_2;
    param_3[1] = 0;
    param_3[2] = 0;
    puVar1[2] = (undefined4)param_2;   // 32-bit MOV store — disasm 0x005684fb
    param_2 = (int *)param_2[2];
    puVar1[1] = 2;
  }
  puVar1[3] = (undefined4)param_2;     // 32-bit MOV store — disasm 0x0056851c
  if (param_4 != (undefined4 *)0x0) {
    *param_4 = 0;
    puVar1[6] = (undefined4)param_4;   // 32-bit MOV stores — disasm 0x00568529/2c
    puVar1[5] = (undefined4)param_4;
    puVar1[4] = 1;
    puVar1[7] = 0;
  }
  if (param_5 != (undefined4 *)0x0) {
    *param_5 = 0;
    puVar1[4] = 0;
    puVar1[9] = (undefined4)param_5;   // 32-bit MOV stores — disasm 0x00568546/49
    puVar1[8] = (undefined4)param_5;
    puVar1[7] = 1;
  }
  return;
}

// ---------------------------------------------------------------------------
// 0x00568560  Appends param_2 as a singly-linked tail node into the container at param_1 and
//             enqueues optional sub-list heads (param_4/param_5).
// ---------------------------------------------------------------------------
extern "C" void __cdecl FUN_00568560(int *param_1,int param_2,int *param_3,undefined4 *param_4,undefined4 *param_5)

{
  if (param_2 != 0) {
    *param_1 = *param_1 + 1;
    *param_3 = param_2;
    param_3[1] = 0;
    param_3[2] = 0;
    *(int **)(param_1[3] + 8) = param_3;
    param_1[3] = (int)param_3;
    param_1[1] = param_1[1] + 1;
  }
  if (param_4 != (undefined4 *)0x0) {
    *param_4 = 0;
    if (param_1[4] == 0) {
      param_1[6] = (int)param_4;
      param_1[5] = (int)param_4;
      param_1[4] = 1;
    }
    else {
      *(undefined4 **)param_1[6] = param_4;
      param_1[6] = (int)param_4;
      param_1[4] = param_1[4] + 1;
    }
  }
  if (param_5 != (undefined4 *)0x0) {
    *param_5 = 0;
    if (param_1[7] == 0) {
      param_1[9] = (int)param_5;
      param_1[8] = (int)param_5;
      param_1[7] = 1;
      return;
    }
    *(undefined4 **)param_1[9] = param_5;
    param_1[9] = (int)param_5;
    param_1[7] = param_1[7] + 1;
  }
  return;
}

// --- gta-reversed-style hook registration (inert on the exe via HookSystemNoOp; installs the
//     inline-JMP under the .asi for the diff-original A/B acceptance) — CLUSTER 1, all 19. ---
RH_ScopedInstall(FUN_005601f0, 0x005601f0);
RH_ScopedInstall(FUN_00563e70, 0x00563e70);
RH_ScopedInstall(FUN_00563f60, 0x00563f60);
RH_ScopedInstall(FUN_00564040, 0x00564040);
RH_ScopedInstall(FUN_005641b0, 0x005641b0);
RH_ScopedInstall(FUN_00564310, 0x00564310);
RH_ScopedInstall(FUN_00565120, 0x00565120);
RH_ScopedInstall(FUN_00565160, 0x00565160);
RH_ScopedInstall(FUN_005651b0, 0x005651b0);
RH_ScopedInstall(FUN_00565200, 0x00565200);
RH_ScopedInstall(FUN_00565550, 0x00565550);
RH_ScopedInstall(FUN_00565ef0, 0x00565ef0);
RH_ScopedInstall(FUN_00565fa0, 0x00565fa0);
RH_ScopedInstall(FUN_00566200, 0x00566200);
RH_ScopedInstall(FUN_005667c0, 0x005667c0);
RH_ScopedInstall(FUN_00566830, 0x00566830);
RH_ScopedInstall(FUN_005675d0, 0x005675d0);
RH_ScopedInstall(FUN_005684c0, 0x005684c0);
RH_ScopedInstall(FUN_00568560, 0x00568560);

}  // namespace Collision
}  // namespace mashed_re
