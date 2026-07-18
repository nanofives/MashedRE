// ============================================================================
//  RwpSolverCore16.cpp  —  B5e solver-island cluster K16
//  10 verbatim RWP-3.7 helpers (contact-manifold / GJK-simplex / broadphase):
//    FUN_005735f0 (0x005735f0, 125 B)  — plane-offset a point by an edge cross-product
//    FUN_00575120 (0x00575120, 207 B)  — contact-point insert with L1 dedup (front+back)
//    FUN_005751f0 (0x005751f0, 180 B)  — squared perpendicular-distance ratio (float10)
//    FUN_00576640 (0x00576640, 562 B)  — broadphase pair generation w/ filter matrices
//    FUN_00579b50 (0x00579b50, 164 B)  — manifold coplanarity test vs tolerance
//    FUN_00579c00 (0x00579c00, 327 B)  — weighted centroid of selected simplex features
//    FUN_00579d50 (0x00579d50, 242 B)  — support-map delta + dot (float10); calls FUN_0055c000
//    FUN_00579e50 (0x00579e50, 133 B)  — per-vertex incremental dot update
//    FUN_00579ee0 (0x00579ee0, 865 B)  — incremental Gram (cross-product) matrix update
//    FUN_0057ae20 (0x0057ae20,   7 B)  — offset accessor: return p + 0x30
//  Sole external dep: FUN_0055c000 (0x0055c000 GJK support map — un-ported B5c extern,
//  already resolved by CollisionBodyCreate.cpp). Version anchor: MASHED.exe
//  2,846,720 B / SHA-256 BDCAE093…3C0E (verify before hook authoring).
//
//  All float math is x87 (FLD/FMUL/FSTP/FDIV confirmed) — plain C float compiles to x87
//  under the project's /arch:IA32 build, so this is bit-faithful. Ghidra's float-compare
//  idioms `(a<b)==(a==b)` (= a>=b) and `(a<b)!=(a==b)` (= a<=b) are kept VERBATIM (C
//  evaluates them identically and this preserves any NaN-ordering behavior).
//
//  NOTE (reinterpret, not conversion): FUN_00576640's `(float)param_1[0x1c]` is a plain
//    dword load (MOV [EAX+0x70]; disasm @0x0057664a) of a float bit-pattern in an int
//    slot — ported as `*(float*)&param_1[0x1c]`, NOT an int→float cast.
//  NOTE (contiguous-buffer, K8/K10 class): FUN_00579d50 passes `&local_18` and `&local_c`
//    as vec3 out-params to FUN_0055c000 — the three consecutive locals at each address are
//    ONE array. Ported as `neg[3]` (=local_18/14/10) and `outc[3]` (=local_c/8/4).
// ============================================================================
#include "../Core/HookSystem.h"
#include <cmath>       // fabsf (ABS)

namespace mashed_re {
namespace Collision {

typedef unsigned int   uint;
typedef unsigned int   undefined4;
typedef unsigned short ushort;
typedef unsigned char  byte;
typedef long double    float10;      // x87 80-bit return (ST0) of FUN_005751f0 / FUN_00579d50

#define DAT_005d757c   (*(const float*)0x005d757cu)   // 0.0f
#define _DAT_005cc320  (*(const float*)0x005cc320u)   // 1.0f              (0x3f800000)
#define _DAT_005cc56c  (*(const float*)0x005cc56cu)   // 0.100000001f      (0x3dcccccd)
#define _DAT_005cc32c  (*(const float*)0x005cc32cu)   // 0.5f              (0x3f000000)
#define _DAT_005cea1c  (*(const float*)0x005cea1cu)   // -9.99999975e-06f  (0xb727c5ac)

// --- extern callee (un-ported B5c GJK support map) --------------------------
extern "C" void* __cdecl FUN_0055c000(int shape,void* mtx,float* dir,float* out); // 0x0055c000

// ---------------------------------------------------------------------------
// 0x005735f0  out = p1 + cross(p1[4..6], p3 - p2)   (zeroes out on null p1)
// ---------------------------------------------------------------------------
extern "C" void __cdecl
FUN_005735f0(float *param_1,float *param_2,float *param_3,float *param_4)
{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;

  if (param_1 == (float *)0x0) {
    param_4[2] = 0.0f;
    param_4[1] = 0.0f;
    *param_4 = 0.0f;
    return;
  }
  fVar1 = *param_3;
  fVar2 = *param_2;
  fVar3 = param_3[1];
  fVar4 = param_2[1];
  fVar5 = param_3[2];
  fVar6 = param_2[2];
  *param_4 = param_1[5] * (fVar5 - fVar6) - param_1[6] * (fVar3 - fVar4);
  param_4[1] = param_1[6] * (fVar1 - fVar2) - param_1[4] * (fVar5 - fVar6);
  param_4[2] = param_1[4] * (fVar3 - fVar4) - param_1[5] * (fVar1 - fVar2);
  *param_4 = *param_1 + *param_4;
  param_4[1] = param_1[1] + param_4[1];
  param_4[2] = param_1[2] + param_4[2];
  return;
}

// ---------------------------------------------------------------------------
// 0x00575120  contact insert: reject if within L1(param_4) of the last/first
//             existing contact; else append (xyz, id, clear +0x1c) and bump count.
// ---------------------------------------------------------------------------
extern "C" undefined4 __cdecl
FUN_00575120(float *param_1,undefined4 param_2,int param_3,float param_4)
{
  int iVar1;
  uint uVar2;
  float fVar3;

  uVar2 = *(uint *)(param_3 + 0xac);
  if ((uVar2 != 0) && (DAT_005d757c <= param_4)) {
    fVar3 = fabsf(*(float *)(param_3 + 0xc) - *param_1) +
            fabsf(*(float *)(param_3 + 0x14) - param_1[2]) +
            fabsf(*(float *)(param_3 + 0x10) - param_1[1]);
    if ((fVar3 < param_4) != (fVar3 == param_4)) {
      return 0;
    }
    if ((1 < uVar2) &&
       (iVar1 = param_3 + uVar2 * 0x28,
       fVar3 = fabsf(*(float *)(iVar1 + -0x14) - param_1[2]) +
               fabsf(*(float *)(param_3 + -0x1c + uVar2 * 0x28) - *param_1) +
               fabsf(*(float *)(iVar1 + -0x18) - param_1[1]),
       (fVar3 < param_4) != (fVar3 == param_4)))
    {
      return 0;
    }
  }
  iVar1 = param_3 + uVar2 * 0x28;
  *(float *)(iVar1 + 0xc) = *param_1;
  *(float *)(iVar1 + 0x10) = param_1[1];
  fVar3 = param_1[2];
  *(undefined4 *)(iVar1 + 0x1c) = 0;
  *(float *)(iVar1 + 0x14) = fVar3;
  *(undefined4 *)(iVar1 + 0x18) = param_2;
  *(int *)(param_3 + 0xac) = *(int *)(param_3 + 0xac) + 1;
  return 1;
}

// ---------------------------------------------------------------------------
// 0x005751f0  |cross(p3-p1, p2-p1)|² / (|p3-p1|²·|p3-p1|² + 1)   — float10 return
// ---------------------------------------------------------------------------
extern "C" float10 __cdecl
FUN_005751f0(float *param_1,float *param_2,float *param_3)
{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float10 fVar6;
  float10 fVar7;

  fVar4 = *param_3 - *param_1;
  fVar5 = param_3[1] - param_1[1];
  fVar1 = (float)((float10)param_3[2] - (float10)param_1[2]);
  fVar2 = (float)(((float10)param_3[2] - (float10)param_1[2]) *
                  ((float10)param_2[1] - (float10)param_1[1]) -
                 (float10)fVar5 * ((float10)param_2[2] - (float10)param_1[2]));
  fVar3 = (float)((float10)fVar4 * ((float10)param_2[2] - (float10)param_1[2]) -
                 (float10)fVar1 * ((float10)*param_2 - (float10)*param_1));
  fVar7 = (float10)fVar5 * ((float10)*param_2 - (float10)*param_1) -
          (float10)fVar4 * ((float10)param_2[1] - (float10)param_1[1]);
  fVar6 = (float10)fVar1 * (float10)fVar1 +
          (float10)fVar4 * (float10)fVar4 + (float10)fVar5 * (float10)fVar5;
  return (fVar7 * fVar7 + (float10)fVar2 * (float10)fVar2 + (float10)fVar3 * (float10)fVar3) /
         (fVar6 * fVar6 + (float10)_DAT_005cc320);
}

// ---------------------------------------------------------------------------
// 0x00576640  broadphase pair generation: emit (bodyA,bodyB) index pairs whose
//             AABBs overlap within param_1[0x1c], honoring the two filter matrices.
// ---------------------------------------------------------------------------
extern "C" uint __cdecl
FUN_00576640(int *param_1,uint param_2,uint param_3,int param_4,int param_5)
{
  float fVar1;
  int iVar2;
  int *piVar3;
  float *pfVar4;
  uint uVar5;
  uint uVar6;
  ushort *puVar7;
  uint uVar8;
  int *piVar9;
  float *pfVar10;
  float *local_24;
  uint local_1c;
  uint local_14;
  uint local_10;

  fVar1 = *(float *)&param_1[0x1c];
  iVar2 = *param_1;
  piVar3 = (int *)param_1[3];
  uVar8 = 0;
  local_1c = 0;
  pfVar4 = (float *)param_1[9];
  uVar5 = param_1[2];
  local_10 = 0;
  piVar9 = piVar3;
  local_24 = pfVar4;
  if (param_2 != 0) {
    while (uVar8 < uVar5) {
      if ((piVar9[2] != 1) && (local_14 = 0, param_3 != 0)) {
        pfVar10 = pfVar4 + param_2 * 8 + 1;
        puVar7 = (ushort *)(piVar3 + param_2 * 5 + 1);
        do {
          if (((((*(int *)(puVar7 + 2) != 1) &&
                ((*piVar9 != *(int *)(puVar7 + -2) || (*(ushort *)(piVar9 + 1) != *puVar7)))) &&
               ((*(ushort *)(piVar9 + 1) != 0xffff || (*puVar7 != 0xffff)))) &&
              (((param_4 == 0 ||
                (uVar6 = (uint)*puVar7 + *(int *)(param_4 + 4) * (uint)*(ushort *)(piVar9 + 1),
                uVar8 = local_1c,
                (*(uint *)(param_4 + 0xc + (uVar6 >> 5) * 4) & 1 << ((byte)uVar6 & 0x1f)) == 0)) &&
               ((param_5 == 0 ||
                (uVar6 = (uint)*(ushort *)(*(int *)(puVar7 + 2) + 0x5a) +
                         (uint)*(ushort *)(piVar9[2] + 0x5a) * *(int *)(param_5 + 4),
                (*(uint *)(param_5 + 0xc + (uVar6 >> 5) * 4) & 1 << ((byte)uVar6 & 0x1f)) == 0))))))
             && ((local_24 == pfVar10 + -1 ||
                 (((((local_24[4] - pfVar10[-1] < fVar1 != (local_24[4] - pfVar10[-1] == fVar1) &&
                     (pfVar10[3] - *local_24 < fVar1 != (pfVar10[3] - *local_24 == fVar1))) &&
                    (local_24[5] - *pfVar10 < fVar1 != (local_24[5] - *pfVar10 == fVar1))) &&
                   ((pfVar10[4] - local_24[1] < fVar1 != (pfVar10[4] - local_24[1] == fVar1) &&
                    (local_24[6] - pfVar10[1] < fVar1 != (local_24[6] - pfVar10[1] == fVar1))))) &&
                  (pfVar10[5] - local_24[2] < fVar1 != (pfVar10[5] - local_24[2] == fVar1))))))) {
            *(int **)(iVar2 + uVar8 * 8) = piVar9;
            *(ushort **)(iVar2 + 4 + uVar8 * 8) = puVar7 + -2;
            uVar8 = uVar8 + 1;
            local_1c = uVar8;
            if (uVar5 <= uVar8) {
              param_1[0x1e] = 1;
              break;
            }
          }
          local_14 = local_14 + 1;
          pfVar10 = pfVar10 + 8;
          puVar7 = puVar7 + 10;
        } while (local_14 < param_3);
      }
      local_10 = local_10 + 1;
      local_24 = local_24 + 8;
      piVar9 = piVar9 + 5;
      if (param_2 <= local_10) {
        param_1[1] = uVar8;
        return uVar8;
      }
    }
  }
  param_1[1] = uVar8;
  return uVar8;
}

// ---------------------------------------------------------------------------
// 0x00579b50  coplanarity test: |plane_normal·(p1[3..5] - manifold[0xc..0x14])| <
//             tol·0.1 + max(|manifold+0x18|,|p1[6]|)·0.5  ?  1 : 0
// ---------------------------------------------------------------------------
extern "C" undefined4 __cdecl
FUN_00579b50(float *param_1,int param_2,float param_3)
{
  float fVar1;

  if (*(int *)(param_2 + 0xac) == 0) {
    return 1;
  }
  fVar1 = fabsf(*(float *)(param_2 + 0x18));
  if (fabsf(*(float *)(param_2 + 0x18)) <= fabsf(param_1[6])) {
    fVar1 = fabsf(param_1[6]);
  }
  if (fabsf(param_1[2] * (param_1[5] - *(float *)(param_2 + 0x14)) +
            (param_1[3] - *(float *)(param_2 + 0xc)) * *param_1 +
            param_1[1] * (param_1[4] - *(float *)(param_2 + 0x10))) <
      param_3 * _DAT_005cc56c + fVar1 * _DAT_005cc32c) {
    return 1;
  }
  return 0;
}

// ---------------------------------------------------------------------------
// 0x00579c00  weighted centroid of the selected simplex features; returns 0 if
//             the min barycentric weight is below -eps of the max, else 1.
// ---------------------------------------------------------------------------
extern "C" undefined4 __cdecl
FUN_00579c00(float *param_1,uint param_2,int param_3,float *param_4)
{
  float fVar1;
  float fVar2;
  float *pfVar3;
  float *pfVar4;
  uint uVar5;
  float local_c;
  int local_8;
  float local_4;

  fVar2 = DAT_005d757c;
  uVar5 = 1;
  local_c = 0.0f;
  local_4 = -1.0f;
  param_1[2] = 0.0f;
  param_1[1] = 0.0f;
  *param_1 = 0.0f;
  local_8 = 0;
  pfVar4 = param_4;
  if (0 < (int)param_2) {
    do {
      if ((param_2 & uVar5) != 0) {
        fVar1 = param_4[local_8 + 0x28 + param_2 * 4];
        if (fVar1 < fVar2) {
          fVar2 = fVar1;
        }
        if (local_4 < fVar1) {
          local_4 = fVar1;
        }
        local_c = fVar1 + local_c;
        pfVar3 = pfVar4 + 3;
        if (param_3 == 0) {
          pfVar3 = pfVar4;
        }
        *param_1 = fVar1 * *pfVar3 + *param_1;
        pfVar3 = pfVar4 + 3;
        if (param_3 == 0) {
          pfVar3 = pfVar4;
        }
        param_1[1] = pfVar3[1] * fVar1 + param_1[1];
        pfVar3 = pfVar4 + 3;
        if (param_3 == 0) {
          pfVar3 = pfVar4;
        }
        param_1[2] = pfVar3[2] * fVar1 + param_1[2];
      }
      uVar5 = uVar5 * 2;
      local_8 = local_8 + 1;
      pfVar4 = pfVar4 + 6;
    } while ((int)uVar5 <= (int)param_2);
    if (local_c != DAT_005d757c) {
      local_c = _DAT_005cc320 / local_c;
      *param_1 = local_c * *param_1;
      param_1[1] = param_1[1] * local_c;
      param_1[2] = param_1[2] * local_c;
    }
  }
  if (fVar2 <= local_4 * _DAT_005cea1c) {
    return 0;
  }
  return 1;
}

// ---------------------------------------------------------------------------
// 0x00579d50  support-map delta between two shapes along ±dir, minus base, plus
//             optional offset; returns dot(dir, result) as float10.
//             Contiguous vec3 out-params: neg[3]=local_18/14/10, outc[3]=local_c/8/4.
// ---------------------------------------------------------------------------
extern "C" float10 __cdecl
FUN_00579d50(float *param_1,float *param_2,int param_3,int *param_4,float *param_5)
{
  int iVar1;
  void *uVar2;
  void *uVar3;
  float *pfVar4;
  float neg[3];    // local_18, local_14, local_10
  float outc[3];   // local_c,  local_8,  local_4

  neg[0] = -*param_2;
  neg[1] = -param_2[1];
  neg[2] = -param_2[2];
  iVar1 = *param_4;
  if (param_3 < 1) {
    FUN_0055c000(*(int *)(iVar1 + 8),*(void **)(iVar1 + 0x10),neg,outc);
    uVar3 = *(void **)(param_4[1] + 0x10);
    uVar2 = *(void **)(param_4[1] + 8);
    pfVar4 = param_2;
  }
  else {
    FUN_0055c000(*(int *)(iVar1 + 8),*(void **)(iVar1 + 0x10),param_2,outc);
    uVar3 = *(void **)(param_4[1] + 0x10);
    uVar2 = *(void **)(param_4[1] + 8);
    pfVar4 = neg;
  }
  FUN_0055c000((int)uVar2,uVar3,pfVar4,param_1 + 3);
  outc[0] = outc[0] - param_1[3];
  *param_1 = outc[0];
  param_1[1] = outc[1] - param_1[4];
  param_1[2] = outc[2] - param_1[5];
  if (param_5 != (float *)0x0) {
    *param_1 = outc[0] + *param_5;
    param_1[1] = param_5[1] + (outc[1] - param_1[4]);
    param_1[2] = param_5[2] + (outc[2] - param_1[5]);
  }
  return (float10)param_2[2] * (float10)param_1[2] +
         (float10)*param_1 * (float10)*param_2 + (float10)param_2[1] * (float10)param_1[1];
}

// ---------------------------------------------------------------------------
// 0x00579e50  for each set simplex bit, write dot(vertex, base-vertex) into the
//             gram row and copy the paired scalar slot.
// ---------------------------------------------------------------------------
extern "C" void __cdecl
FUN_00579e50(int param_1)
{
  float *pfVar1;
  int iVar2;
  uint uVar3;
  float *pfVar4;
  uint uVar5;
  int iVar6;
  int iVar7;

  uVar5 = 1;
  uVar3 = *(uint *)(param_1 + 0x1a4) | *(uint *)(param_1 + 0x1ac);
  iVar6 = 0;
  if (0 < (int)uVar3) {
    iVar7 = 0x18;
    pfVar4 = (float *)(param_1 + 8);
    do {
      if ((uVar5 & uVar3) != 0) {
        iVar2 = *(int *)(param_1 + 0x1a8);
        pfVar1 = (float *)(param_1 + iVar2 * 0x18);
        *(float *)(param_1 + (iVar2 + iVar7) * 4) =
             pfVar1[2] * *pfVar4 +
             pfVar4[-2] * *pfVar1 + pfVar4[-1] * *(float *)(param_1 + 4 + iVar2 * 0x18);
        *(undefined4 *)(param_1 + (iVar6 + 0x18 + *(int *)(param_1 + 0x1a8) * 4) * 4) =
             *(undefined4 *)(param_1 + (iVar7 + *(int *)(param_1 + 0x1a8)) * 4);
      }
      uVar5 = uVar5 * 2;
      uVar3 = *(uint *)(param_1 + 0x1a4) | *(uint *)(param_1 + 0x1ac);
      iVar6 = iVar6 + 1;
      iVar7 = iVar7 + 4;
      pfVar4 = pfVar4 + 6;
    } while ((int)uVar5 <= (int)uVar3);
  }
  return;
}

// ---------------------------------------------------------------------------
// 0x00579ee0  incremental Gram (edge cross-product) matrix update over the active
//             simplex; fills the final 4-tet barycentric row when the set is full.
// ---------------------------------------------------------------------------
extern "C" void __cdecl
FUN_00579ee0(float *param_1)
{
  float *pfVar1;
  float *pfVar2;
  float *pfVar3;
  float *pfVar4;
  int iVar5;
  uint uVar6;
  int iVar7;
  int iVar8;
  float *pfVar9;
  uint uVar10;
  int iVar11;
  int iVar12;
  uint uVar13;
  float *local_40;
  int local_38;
  float *local_34;
  float *local_30;
  float *local_28;
  float *local_24;
  float *local_20;
  float *local_1c;
  uint local_14;

  iVar7 = (int)param_1;
  iVar5 = *(int *)((int)param_1 + 0x1a8);
  uVar6 = *(uint *)((int)param_1 + 0x1ac);
  local_34 = (float *)((int)param_1 + 0x60);
  *(undefined4 *)((int)param_1 + 0xa0 + (iVar5 + uVar6 * 4) * 4) = 0x3f800000;
  uVar10 = *(uint *)((int)param_1 + 0x1a4);
  local_38 = 0;
  local_14 = 1;
  if (0 < (int)uVar10) {
    pfVar1 = local_34 + iVar5;
    local_30 = pfVar1;
    local_20 = local_34;
    local_1c = local_34;
    do {
      if ((local_14 & uVar10) != 0) {
        iVar8 = (local_14 | uVar6) * 4;
        pfVar3 = (float *)(iVar7 + 0xa0 + (iVar8 + iVar5) * 4);
        param_1 = (float *)(iVar7 + 0x60);
        pfVar4 = (float *)(iVar7 + 0xa0 + (iVar8 + local_38) * 4);
        *pfVar3 = *local_34 - *local_30;
        iVar8 = local_38 + iVar5 * 4;
        pfVar2 = param_1 + iVar8;
        uVar13 = 1;
        iVar12 = 0;
        *pfVar4 = param_1[iVar5 * 5] - param_1[iVar8];
        uVar10 = *(uint *)(iVar7 + 0x1a4);
        if (0 < (int)uVar10) {
          local_40 = local_20;
          pfVar9 = param_1 + iVar5 * 4;
          local_24 = local_1c;
          local_28 = pfVar1;
          do {
            if ((uVar13 & uVar10) != 0) {
              if ((int)local_14 <= (int)uVar13) break;
              uVar10 = uVar13 | local_14;
              iVar8 = (uVar10 | uVar6) * 4;
              *(float *)(iVar7 + 0xa0 + (iVar5 + iVar8) * 4) =
                   (*local_40 - *local_30) * *(float *)(iVar7 + 0xa0 + (local_38 + uVar10 * 4) * 4)
                   + (*param_1 - *local_28) * *(float *)(iVar7 + 0xa0 + (iVar12 + uVar10 * 4) * 4);
              iVar11 = (uVar13 | uVar6) * 4;
              *(float *)(iVar7 + 0xa0 + (local_38 + iVar8) * 4) =
                   (*pfVar9 - *pfVar2) * *(float *)(iVar7 + 0xa0 + (iVar11 + iVar5) * 4) +
                   (*param_1 - *local_24) * *(float *)(iVar7 + 0xa0 + (iVar11 + iVar12) * 4);
              *(float *)(iVar7 + 0xa0 + (iVar8 + iVar12) * 4) =
                   (*local_34 - *local_40) * *pfVar4 + (*pfVar2 - *pfVar9) * *pfVar3;
            }
            param_1 = param_1 + 5;
            uVar13 = uVar13 * 2;
            local_28 = local_28 + 4;
            iVar12 = iVar12 + 1;
            local_24 = local_24 + 4;
            local_40 = local_40 + 1;
            pfVar9 = pfVar9 + 1;
            uVar10 = *(uint *)(iVar7 + 0x1a4);
          } while ((int)uVar13 <= (int)uVar10);
        }
      }
      local_34 = local_34 + 5;
      local_14 = local_14 * 2;
      local_20 = local_20 + 4;
      local_30 = local_30 + 4;
      uVar10 = *(uint *)(iVar7 + 0x1a4);
      local_38 = local_38 + 1;
      local_1c = local_1c + 1;
    } while ((int)local_14 <= (int)uVar10);
  }
  if ((*(uint *)(iVar7 + 0x1a4) | uVar6) == 0xf) {
    *(float *)(iVar7 + 400) =
         (*(float *)(iVar7 + 0x84) - *(float *)(iVar7 + 0x80)) * *(float *)(iVar7 + 0x188) +
         (*(float *)(iVar7 + 0x94) - *(float *)(iVar7 + 0x90)) * *(float *)(iVar7 + 0x18c) +
         (*(float *)(iVar7 + 0x74) - *(float *)(iVar7 + 0x70)) * *(float *)(iVar7 + 0x184);
    *(float *)(iVar7 + 0x194) =
         (*(float *)(iVar7 + 0x80) - *(float *)(iVar7 + 0x84)) * *(float *)(iVar7 + 0x178) +
         (*(float *)(iVar7 + 0x90) - *(float *)(iVar7 + 0x94)) * *(float *)(iVar7 + 0x17c) +
         (*(float *)(iVar7 + 0x60) - *(float *)(iVar7 + 100)) * *(float *)(iVar7 + 0x170);
    *(float *)(iVar7 + 0x198) =
         (*(float *)(iVar7 + 0x60) - *(float *)(iVar7 + 0x68)) * *(float *)(iVar7 + 0x150) +
         (*(float *)(iVar7 + 0x90) - *(float *)(iVar7 + 0x98)) * *(float *)(iVar7 + 0x15c) +
         (*(float *)(iVar7 + 0x70) - *(float *)(iVar7 + 0x78)) * *(float *)(iVar7 + 0x154);
    *(float *)(iVar7 + 0x19c) =
         (*(float *)(iVar7 + 0x60) - *(float *)(iVar7 + 0x6c)) * *(float *)(iVar7 + 0x110) +
         (*(float *)(iVar7 + 0x80) - *(float *)(iVar7 + 0x8c)) * *(float *)(iVar7 + 0x118) +
         (*(float *)(iVar7 + 0x70) - *(float *)(iVar7 + 0x7c)) * *(float *)(iVar7 + 0x114);
  }
  return;
}

// ---------------------------------------------------------------------------
// 0x0057ae20  offset accessor: return p + 0x30
// ---------------------------------------------------------------------------
extern "C" int __cdecl
FUN_0057ae20(int param_1)
{
  return param_1 + 0x30;
}

// --- gta-reversed-style hook registration — CLUSTER 16. ---
RH_ScopedInstall(FUN_005735f0, 0x005735f0);
RH_ScopedInstall(FUN_00575120, 0x00575120);
RH_ScopedInstall(FUN_005751f0, 0x005751f0);
RH_ScopedInstall(FUN_00576640, 0x00576640);
RH_ScopedInstall(FUN_00579b50, 0x00579b50);
RH_ScopedInstall(FUN_00579c00, 0x00579c00);
RH_ScopedInstall(FUN_00579d50, 0x00579d50);
RH_ScopedInstall(FUN_00579e50, 0x00579e50);
RH_ScopedInstall(FUN_00579ee0, 0x00579ee0);
RH_ScopedInstall(FUN_0057ae20, 0x0057ae20);

}  // namespace Collision
}  // namespace mashed_re
