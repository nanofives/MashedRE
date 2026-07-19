// ============================================================================
//  RwpSolverCore17.cpp  —  B5e solver-island cluster K17
//  9 verbatim RWP-3.7 helpers (GJK support/distance + manifold BVH + merge):
//    FUN_00575b60 (0x00575b60, 217 B)  — face-normal orient vs support extremes
//    FUN_00575fe0 (0x00575fe0,1631 B)  — recursive BVH contact-node expansion
//    FUN_00578b20 (0x00578b20, 175 B)  — 2-shape support delta (float10)
//    FUN_00578bd0 (0x00578bd0, 212 B)  — face-A support delta (float10)
//    FUN_00578cb0 (0x00578cb0, 211 B)  — face-B support delta (float10)
//    FUN_00578d90 (0x00578d90, 184 B)  — edge-edge axis build + normalize
//    FUN_00578ff0 (0x00578ff0,2386 B)  — manifold coplanar-merge + tetra reduce
//    FUN_0057a250 (0x0057a250,1037 B)  — GJK/EPA penetration iteration
//    FUN_0057a660 (0x0057a660, 823 B)  — GJK initial-axis selection (float10)
//  Deps: K1/K2/K3 (FUN_005667c0 normalize, FUN_00566200/FUN_00566830 vec ops,
//  FUN_004c4600, FUN_0055bd70 support tail-JMP, FUN_0055c2d0 support+bias) and
//  K16 (FUN_00579c00/579d50/579e50/579ee0/579b50/57ae20). All externed below;
//  their definitions live in the sibling K1..K3/K16 TUs in the same .asi.
//  Version anchor: MASHED.exe 2,846,720 B / SHA-256 BDCAE093…3C0E.
//
//  All float math is x87 (FLD/FMUL/FSTP/FDIV/FILD confirmed by disasm) — plain C
//  float compiles to x87 under /arch:IA32, so this is bit-faithful. Ghidra's
//  float-compare idioms `(a<b)==(a==b)` (= a>=b) and `(a<b)!=(a==b)` (= a<=b) are
//  kept VERBATIM. float10 = x87 80-bit ST0 return (must be non-void).
//
//  DISASM-RESOLVED REINTERPRETS (Ghidra mistyped these; verified against listing):
//   * FUN_00578ff0: contact counts at +0xac load via FILD (0x579642 / 0x5796e4 /
//     0x57963b) — integer→float, NOT a float field; the `fVar17`/`fVar16`
//     "float" list-walks are integer pointer chases (MOV [.+0xdc]; TEST EAX;
//     DEC EAX loops @0x579732/0x5797d2/0x5797cb); `local_b4`/`+0xa8` are int
//     counters/flags (INC EAX / TEST EAX @0x579534/0x5793e9).
//   * FUN_00575fe0: `pfVar10[0x17] = (float)&DAT_005e5db0` is a literal-address
//     store `MOV [EBX+0x5c],0x5e5db0` (0x576344); all `(**code)()` are __cdecl
//     (caller-cleans, ADD ESP after each).
//  CONTIGUOUS STACK BUFFERS (address-taken consecutive locals = one array):
//   * FUN_0057a250: local_18[4]+local_8+local_4 = simplex[6]; local_30/2c/28 and
//     local_24/20/1c are vec3 out-params to FUN_00579c00/579d50/005667c0.
//   * FUN_0057a660: local_50..local_3c = loc[6]; local_38[24] = 6-float support
//     work buffer; the FUN_0055bd70 AABB out is a 7-float buffer (gap at +0x14).
//   * FUN_00575fe0: local_20..local_8 = local_bb[7] (gap at +0x14) AABB.
//  SCRATCH-SLOT REUSE: FUN_00575b60/578b20/578bd0 reuse incoming pointer slots as
//    the two FUN_0055c2d0 out-floats (modeled as fresh locals a4/a5); FUN_0057a660
//    reuses param_2 as a float accumulator (modeled as `acc`, pfVar3 = saved out);
//    FUN_0057a250 reuses param_4 (a pointer, saved as iVar5) as a float acc (fAcc).
// ============================================================================
#include "../Core/HookSystem.h"
#include <cmath>       // fabsf / fabsl (ABS)

namespace mashed_re {
namespace Collision {

typedef unsigned int   uint;
typedef unsigned int   undefined4;
typedef unsigned short undefined2;
typedef unsigned short ushort;
typedef unsigned char  byte;
typedef long double    float10;      // x87 80-bit ST0 return

#define DAT_005d757c      (*(const float*)0x005d757cu)   // 0.0f
#define _DAT_005cc320     (*(const float*)0x005cc320u)   // 1.0f
#define _DAT_005cc32c     (*(const float*)0x005cc32cu)   // 0.5f
#define _DAT_005cc33c     (*(const float*)0x005cc33cu)   // -1.0f            (0xbf800000)
#define _DAT_005cc328     (*(const float*)0x005cc328u)   // 0.00999999978f   (0x3c23d70a)
#define _DAT_005cc558     (*(const float*)0x005cc558u)   // 0.00100000005f   (0x3a83126f)
#define _DAT_005cc9b4     (*(const float*)0x005cc9b4u)   // 0.990000009f     (0x3f7d70a4)
#define _DAT_005cd03c     (*(const float*)0x005cd03cu)   // 9.99999975e-05f  (0x38d1b717)
#define _DAT_005ccabc     (*(const float*)0x005ccabcu)   // 1.10000002f      (0x3f8ccccd)
#define _DAT_005ce1d4     (*(const float*)0x005ce1d4u)   // 1.00999999f      (0x3f8147ae)
#define _DAT_005ce54c     (*(const float*)0x005ce54cu)   // 1.00099994e-06f  (0x358637bd)
#define PTR_DAT_005ceabc  (*(const float*)0x005ceabcu)   // 1.17549435e-38f  (0x00800000)
#define PTR_DAT_00623fe8  (*(uint*)0x00623fe8u)          // &table = (uint*)0x00623fe8

// --- extern callees (K1/K2/K3 + K16, defined in sibling TUs of the same .asi) --
extern "C" void    __cdecl FUN_00566200(float *param_1,float *param_2,float *param_3);           // 0x00566200
extern "C" float10 __cdecl FUN_005667c0(float *param_1,float *param_2);                           // 0x005667c0
extern "C" void    __cdecl FUN_00566830(float *param_1,float *param_2);                           // 0x00566830
extern "C" uint *  __cdecl FUN_004c4600(uint *param_1,uint *param_2,uint *param_3);               // 0x004c4600
extern "C" void    __cdecl FUN_0055bd70(int param_1,void *param_2,int param_3,void *param_4);     // 0x0055bd70 (naked tail-JMP)
extern "C" void    __cdecl FUN_0055c2d0(int param_1,float *param_2,float *param_3,float *param_4,float *param_5); // 0x0055c2d0
extern "C" undefined4 __cdecl FUN_00579b50(float *param_1,int param_2,float param_3);             // 0x00579b50 (K16)
extern "C" undefined4 __cdecl FUN_00579c00(float *param_1,uint param_2,int param_3,float *param_4);// 0x00579c00 (K16)
extern "C" float10 __cdecl FUN_00579d50(float *param_1,float *param_2,int param_3,int *param_4,float *param_5); // 0x00579d50 (K16)
extern "C" void    __cdecl FUN_00579e50(int param_1);                                             // 0x00579e50 (K16)
extern "C" void    __cdecl FUN_00579ee0(float *param_1);                                          // 0x00579ee0 (K16)
extern "C" int     __cdecl FUN_0057ae20(int param_1);                                             // 0x0057ae20 (K16)

// __cdecl indirect-call prototypes for the shape method-table slots in FUN_00575fe0
typedef void (__cdecl *rwp_support4)(void*,void*,int,void*);          // [.+0x10]
typedef uint (__cdecl *rwp_clip4)(void*,void*,uint,void*);            // [.+0x44]
typedef int  (__cdecl *rwp_clip5)(void*,void*,void*,uint,void*);      // [.+0x48]

// ---------------------------------------------------------------------------
// 0x00575b60  Write the world-space face normal of body param_1's shape into
//   param_3 (tag word -> *param_4); if bit0 of the tag is clear, flip the normal
//   to point away from param_2's support extreme (compare vs FUN_0055c2d0 span).
// ---------------------------------------------------------------------------
extern "C" void __cdecl
FUN_00575b60(int param_1,int param_2,float *param_3,byte *param_4)
{
  int iVar2 = *(int *)(param_1 + 8);
  short sVar1 = *(short *)(iVar2 + 0x48);
  *(short *)param_4 = sVar1;
  if (sVar1 != 0xe) {
    float *pfVar3 = *(float **)(param_1 + 0x10);
    *param_3 = *pfVar3 * *(float *)(iVar2 + 0x30) +
               pfVar3[4] * *(float *)(iVar2 + 0x34) + pfVar3[8] * *(float *)(iVar2 + 0x38);
    int iVar4 = *(int *)(param_1 + 0x10);
    param_3[1] = *(float *)(iVar4 + 0x14) * *(float *)(iVar2 + 0x34) +
                 *(float *)(iVar4 + 4) * *(float *)(iVar2 + 0x30) +
                 *(float *)(iVar4 + 0x24) * *(float *)(iVar2 + 0x38);
    iVar4 = *(int *)(param_1 + 0x10);
    param_3[2] = *(float *)(iVar4 + 0x18) * *(float *)(iVar2 + 0x34) +
                 *(float *)(iVar4 + 8) * *(float *)(iVar2 + 0x30) +
                 *(float *)(iVar4 + 0x28) * *(float *)(iVar2 + 0x38);
    if ((*param_4 & 1) == 0) {
      float a4, a5;                          // &param_4, &param_1 slots reused as FUN_0055c2d0 out
      FUN_0055c2d0(*(int *)(param_2 + 8), *(float **)(param_2 + 0x10), param_3, &a4, &a5);
      int iVar4b = *(int *)(param_1 + 0x10);
      float fVar5 = *(float *)(iVar4b + 0x38) * param_3[2] +
              *(float *)(iVar4b + 0x30) * *param_3 + *(float *)(iVar4b + 0x34) * param_3[1] +
              *(float *)(iVar2 + 0x18) * *(float *)(iVar2 + 0x38) +
              *(float *)(iVar2 + 0x14) * *(float *)(iVar2 + 0x34) +
              *(float *)(iVar2 + 0x10) * *(float *)(iVar2 + 0x30);
      if (a5 - fVar5 < fVar5 - a4) {
        *param_3 = -*param_3;
        param_3[1] = -param_3[1];
        param_3[2] = -param_3[2];
      }
    }
  }
  return;
}

// ---------------------------------------------------------------------------
// 0x00578b20  Signed support-span delta of the two shapes in param_1 along axis
//   param_2; if the negative side wins, mirror the axis (*_DAT_005cc33c = -1).
// ---------------------------------------------------------------------------
extern "C" float10 __cdecl
FUN_00578b20(int *param_1,float *param_2,float *param_3)
{
  float local_8, local_4;
  float a4, a5;                              // &param_2, &param_1 slots reused
  FUN_0055c2d0(*(int *)(*param_1 + 8), *(float **)(*param_1 + 0x10), param_2, &local_8, &local_4);
  FUN_0055c2d0(*(int *)(param_1[1] + 8), *(float **)(param_1[1] + 0x10), param_2, &a4, &a5);
  if ((float10)(a4 - local_4) < (float10)local_8 - (float10)a5) {
    *param_3 = a5;
    return (float10)local_8 - (float10)a5;
  }
  *param_2 = *param_2 * _DAT_005cc33c;
  param_2[1] = param_2[1] * _DAT_005cc33c;
  param_2[2] = param_2[2] * _DAT_005cc33c;
  *param_3 = -a4;
  return (float10)(a4 - local_4);
}

// ---------------------------------------------------------------------------
// 0x00578bd0  Support-span delta with face-A plane (param_2[3]/[4] bias); on the
//   losing side, mirror param_2 into param_3 scaled by -1.
// ---------------------------------------------------------------------------
extern "C" float10 __cdecl
FUN_00578bd0(int *param_1,float *param_2,float *param_3,float *param_4)
{
  float *pfVar5 = param_2;
  int iVar3 = *(int *)(*param_1 + 0x10);
  float fVar4 = *(float *)(iVar3 + 0x38) * param_2[2] +
          *(float *)(iVar3 + 0x30) * *param_2 + *(float *)(iVar3 + 0x34) * param_2[1];
  float fVar1 = param_2[3];
  float fVar2 = param_2[4];
  float a4, a5;                              // &param_2, &param_1 slots reused
  FUN_0055c2d0(*(int *)(param_1[1] + 8), *(float **)(param_1[1] + 0x10), param_2, &a4, &a5);
  float10 fVar6 = (float10)(fVar4 + fVar1) - (float10)a5;
  float fVar1b = a4 - (fVar4 + fVar2);
  if ((float10)fVar1b < fVar6) {
    *param_4 = a5;
    *param_3 = *pfVar5;
    param_3[1] = pfVar5[1];
    param_3[2] = pfVar5[2];
    return fVar6;
  }
  *param_3 = *pfVar5 * _DAT_005cc33c;
  param_3[1] = pfVar5[1] * _DAT_005cc33c;
  param_3[2] = pfVar5[2] * _DAT_005cc33c;
  *param_4 = -a4;
  return (float10)fVar1b;
}

// ---------------------------------------------------------------------------
// 0x00578cb0  Support-span delta with face-B plane; mirror-scale on the losing
//   side. Uses local out-floats only (no slot reuse).
// ---------------------------------------------------------------------------
extern "C" float10 __cdecl
FUN_00578cb0(int *param_1,float *param_2,float *param_3,float *param_4)
{
  float local_8, local_4;
  FUN_0055c2d0(*(int *)(*param_1 + 8), *(float **)(*param_1 + 0x10), param_2, &local_8, &local_4);
  int iVar1 = *(int *)(param_1[1] + 0x10);
  float fVar2 = *(float *)(iVar1 + 0x38) * param_2[2] +
          *(float *)(iVar1 + 0x30) * *param_2 + *(float *)(iVar1 + 0x34) * param_2[1];
  float fVar3 = fVar2 + param_2[3];
  fVar2 = fVar2 + param_2[4];
  float10 fVar4 = (float10)local_8 - (float10)fVar2;
  local_4 = fVar3 - local_4;
  if ((float10)local_4 < fVar4) {
    *param_4 = fVar2;
    *param_3 = *param_2;
    param_3[1] = param_2[1];
    param_3[2] = param_2[2];
    return fVar4;
  }
  *param_3 = *param_2 * _DAT_005cc33c;
  param_3[1] = param_2[1] * _DAT_005cc33c;
  param_3[2] = param_2[2] * _DAT_005cc33c;
  *param_4 = -fVar3;
  return (float10)local_4;
}

// ---------------------------------------------------------------------------
// 0x00578d90  Edge-edge separating axis: perp component of (posA-posB) against
//   param_3, normalized; if the residual is tiny relative to the projection,
//   fall back to param_3's own direction (FUN_00566830).
// ---------------------------------------------------------------------------
extern "C" void __cdecl
FUN_00578d90(float *param_1,float *param_2,float *param_3,float *param_4)
{
  *param_4 = *param_1 - *param_2;
  param_4[1] = param_1[1] - param_2[1];
  float fVar1 = param_1[2];
  float fVar2 = param_2[2];
  param_4[2] = fVar1 - fVar2;
  fVar1 = -(param_3[2] * (fVar1 - fVar2) + *param_3 * *param_4 + param_3[1] * param_4[1]);
  *param_4 = fVar1 * *param_3 + *param_4;
  param_4[1] = param_3[1] * fVar1 + param_4[1];
  param_4[2] = param_3[2] * fVar1 + param_4[2];
  float10 fVar3 = (float10)FUN_005667c0(param_4,param_4);
  if ((fabsl((float10)fVar1) <= (float10)(float)PTR_DAT_005ceabc) ||
     (fVar3 / fabsl((float10)fVar1) <= (float10)_DAT_005cc558)) {
    FUN_00566830(param_4,param_3);
    FUN_005667c0(param_4,param_4);
  }
  return;
}

// ---------------------------------------------------------------------------
// 0x0057a660  GJK initial-axis pick: prefer an extreme axis of whichever shape is
//   a box (offset via FUN_0057ae20), else the centroid-difference of the two
//   AABBs; then refine by testing the three signed cardinal axes (FUN_00579d50).
// ---------------------------------------------------------------------------
extern "C" float10 __cdecl
FUN_0057a660(int *param_1,float *param_2)
{
  float *pfVar3 = param_2;                   // saved out ptr (param_2 not reused here)
  float loc[6];                              // [0]=l50 [1]=l4c [2]=l48 [3]=l44 [4]=l40 [5]=l3c
  float local_38[6];                         // 24-byte support work buffer
  int iVar1 = *(int *)(*param_1 + 8);
  if ((**(short **)(iVar1 + 0x5c) == 4) && ((*(byte *)(iVar1 + 0x48) & 1) != 0)) {
    float *pfVar5 = (float *)FUN_0057ae20(iVar1);
    loc[0] = *pfVar5;
    loc[1] = pfVar5[1];
    loc[3] = loc[0] * _DAT_005cc33c;
    loc[2] = pfVar5[2];
    loc[4] = loc[1] * _DAT_005cc33c;
    *param_2 = loc[3];
    loc[5] = loc[2] * _DAT_005cc33c;
    param_2[1] = loc[4];
    param_2[2] = loc[5];
  }
  else {
    int iVar2 = *(int *)(param_1[1] + 8);
    if ((**(short **)(iVar2 + 0x5c) == 4) && ((*(byte *)(iVar2 + 0x48) & 1) != 0)) {
      float *pfVar5 = (float *)FUN_0057ae20(iVar2);
      loc[3] = *pfVar5;
      loc[4] = pfVar5[1];
      loc[5] = pfVar5[2];
      *param_2 = loc[3];
      param_2[1] = loc[4];
      param_2[2] = loc[5];
    }
    else {
      float bb[7];                           // FUN_0055bd70 AABB out (gap at [3])
      FUN_0055bd70(iVar1, *(void **)(*param_1 + 0x10), 0, &bb[0]);
      loc[3] = (bb[0] - bb[4]) * _DAT_005cc32c + bb[4];
      loc[4] = (bb[1] - bb[5]) * _DAT_005cc32c + bb[5];
      loc[5] = (bb[2] - bb[6]) * _DAT_005cc32c + bb[6];
      FUN_0055bd70(*(int *)(param_1[1] + 8), *(void **)(param_1[1] + 0x10), 0, &bb[0]);
      loc[0] = (bb[0] - bb[4]) * _DAT_005cc32c + bb[4];
      loc[1] = (bb[1] - bb[5]) * _DAT_005cc32c + bb[5];
      loc[2] = (bb[2] - bb[6]) * _DAT_005cc32c + bb[6];
      loc[3] = loc[3] - loc[0];
      loc[4] = loc[4] - loc[1];
      loc[5] = loc[5] - loc[2];
      FUN_005667c0(param_2, &loc[3]);
    }
  }
  float10 fVar6 = (float10)FUN_00579d50(local_38, param_2, 0xffffffff, param_1, 0);
  loc[0] = -1.0f;
  if (DAT_005d757c <= loc[3]) loc[0] = 1.0f;
  loc[1] = 0.0f; loc[2] = 0.0f;
  float10 fVar7 = (float10)FUN_00579d50(local_38, &loc[0], 0xffffffff, param_1, 0);
  float fVar4 = (float)-fVar6;
  if (-fVar7 < (float10)(float)-fVar6) {
    *param_2 = loc[0];
    param_2[1] = loc[1];
    param_2[2] = loc[2];
    fVar4 = (float)-fVar7;
  }
  float acc = fVar4;                         // param_2 slot reused as accumulator
  loc[0] = 0.0f;
  loc[1] = -1.0f;
  if (DAT_005d757c <= loc[4]) loc[1] = 1.0f;
  loc[2] = 0.0f;
  fVar6 = (float10)FUN_00579d50(local_38, &loc[0], 0xffffffff, param_1, 0);
  if (-fVar6 < (float10)acc) {
    acc = (float)-fVar6;
    *pfVar3 = loc[0];
    pfVar3[1] = loc[1];
    pfVar3[2] = loc[2];
  }
  loc[0] = 0.0f;
  loc[1] = 0.0f;
  loc[2] = -1.0f;
  if (DAT_005d757c <= loc[5]) loc[2] = 1.0f;
  fVar6 = (float10)FUN_00579d50(local_38, &loc[0], 0xffffffff, param_1, 0);
  if ((float10)acc <= -fVar6) {
    return (float10)acc;
  }
  *pfVar3 = loc[0];
  pfVar3[1] = loc[1];
  pfVar3[2] = loc[2];
  return -fVar6;
}

// ---------------------------------------------------------------------------
// 0x0057a250  GJK/EPA penetration-depth iteration over the simplex work-struct at
//   param_4 (saved as iVar5); grows the active bitmask at +0x1a4, supports along
//   the running direction (FUN_00579d50), and updates the incremental Gram basis
//   (FUN_00579e50 / FUN_00579ee0) until the depth bound converges.
// ---------------------------------------------------------------------------
extern "C" int __cdecl
FUN_0057a250(int *param_1,float *param_2,float param_3,int param_4,float *param_5,
             float *param_6,float *param_7,float param_8)
{
  float *pfVar1;
  float fVar2, fVar3;
  uint uVar4, uVar6, uVar9;
  int iVar7, iVar8;
  float10 fVar10;
  float local_40, local_34;
  int local_38;
  float sp[3];                               // local_30, local_2c, local_28
  float cand[3];                             // local_24, local_20, local_1c
  float local_18[6];                         // local_18[0..3], local_8=[4], local_4=[5]

  int iVar5 = param_4;                       // (int)param_4 — simplex work-struct base
  param_8 = param_8 * _DAT_005cd03c;
  fVar10 = (float10)_DAT_005ccabc;
  uVar9 = 1;
  local_38 = 0;
  local_34 = 0.0f;
  *(undefined4 *)(iVar5 + 0x1a4) = 0;
  float fAcc = 0.0f;                         // param_4 = 0.0 reuse
  sp[0] = 0.5f;
  sp[1] = 0.6f;
  sp[2] = 0.6244998f;
  fVar10 = (float10)param_8 * fVar10 + (float10)param_3 + (float10)_DAT_005cc320;
  do {
    local_40 = (float)fVar10;
    if (fVar10 <= (float10)param_8) break;
    uVar6 = 1;
    iVar8 = 0;
    *(undefined4 *)(iVar5 + 0x1ac) = 1;
    *(undefined4 *)(iVar5 + 0x1a8) = 0;
    if ((*(uint *)(iVar5 + 0x1a4) & 1) != 0) {
      do {
        uVar6 = uVar6 * 2;
        iVar8 = iVar8 + 1;
      } while ((*(uint *)(iVar5 + 0x1a4) & uVar6) != 0);
      *(int *)(iVar5 + 0x1a8) = iVar8;
      *(uint *)(iVar5 + 0x1ac) = uVar6;
    }
    pfVar1 = (float *)(iVar5 + *(int *)(iVar5 + 0x1a8) * 0x18);
    fVar10 = (float10)FUN_00579d50(pfVar1, sp, 0xffffffff, param_1, param_2);
    fVar2 = (float)fVar10;
    if ((float10)param_3 < fVar10) {
      *param_5 = sp[0];
      param_5[1] = sp[1];
      param_5[2] = sp[2];
      FUN_00579c00(sp, *(uint *)(iVar5 + 0x1a4), 1, (float *)iVar5);
      *param_6 = fVar2 * _DAT_005cc32c +
                 param_5[2] * sp[2] + sp[0] * *param_5 + param_5[1] * sp[1];
      *param_7 = fVar2;
      return 2;
    }
    if (fAcc < fVar2) {
      fAcc = fVar2;
    }
    if ((local_40 < fAcc * _DAT_005ce1d4) ||
       (((DAT_005d757c < local_40 && (DAT_005d757c < fVar2)) &&
        ((local_38 != 0 || (0x28 < (int)uVar9)))))) {
      *param_5 = sp[0];
      param_5[1] = sp[1];
      param_5[2] = sp[2];
      FUN_00579c00(sp, *(uint *)(iVar5 + 0x1a4), 1, (float *)iVar5);
      *param_6 = fVar2 * _DAT_005cc32c +
                 param_5[2] * sp[2] + sp[0] * *param_5 + param_5[1] * sp[1];
      *param_7 = fVar2;
      return 1;
    }
    uVar9 = uVar9 + 1;
    if ((0x32 < (int)uVar9) ||
       ((3 < (int)uVar9 &&
        (((fabsf(pfVar1[2] - local_18[2]) + fabsf(*pfVar1 - local_18[0]) + fabsf(pfVar1[1] - local_18[1])
           < _DAT_005ce54c ||
          (fabsf(pfVar1[2] - local_18[5]) + fabsf(*pfVar1 - local_18[3]) + fabsf(pfVar1[1] - local_18[4]) <
           _DAT_005ce54c)) && (local_38 = local_38 + 1, 5 < local_38)))))) {
      return 0;
    }
    uVar6 = uVar9 & 1;
    fVar2 = pfVar1[1];
    local_18[uVar6 * 3] = *pfVar1;
    fVar3 = pfVar1[2];
    local_18[uVar6 * 3 + 1] = fVar2;
    local_18[uVar6 * 3 + 2] = fVar3;
    FUN_00579e50(iVar5);
    FUN_00579ee0((float *)iVar5);
    uVar6 = 0;
    iVar8 = 0;
    do {
      uVar4 = *(uint *)((&PTR_DAT_00623fe8)[*(int *)(iVar5 + 0x1a4)] + iVar8 * 4);
      if (((iVar8 != 0) || (fAcc <= DAT_005d757c)) || ((*(uint *)(iVar5 + 0x1ac) | uVar4) != 0xf)) {
        if (uVar4 == 0) {
          iVar7 = iVar5 + *(int *)(iVar5 + 0x1a8) * 0x18;
          cand[0] = *(float *)(iVar5 + *(int *)(iVar5 + 0x1a8) * 0x18);
          cand[1] = *(float *)(iVar7 + 4);
          cand[2] = *(float *)(iVar7 + 8);
        }
        else {
          iVar7 = (int)FUN_00579c00(cand, *(uint *)(iVar5 + 0x1ac) | uVar4, 0, (float *)iVar5);
          if (iVar7 == 0) goto LAB_0057a521;
        }
        fVar2 = cand[2] * cand[2] + cand[0] * cand[0] + cand[1] * cand[1];
        if ((uVar6 == 0) || (((fVar2 < local_34) != (fVar2 == local_34)))) {
          sp[0] = cand[0];
          sp[1] = cand[1];
          sp[2] = cand[2];
          uVar6 = uVar4;
          local_34 = fVar2;
        }
      }
LAB_0057a521:
      iVar8 = iVar8 + 1;
    } while (uVar4 != 0);
    fVar10 = (float10)FUN_005667c0(sp, sp);
    uVar6 = *(uint *)(iVar5 + 0x1ac) | uVar6;
    *(uint *)(iVar5 + 0x1a4) = uVar6;
  } while ((int)uVar6 < 0xf);
  *param_7 = -1.0f;
  return 1;
}

// ---------------------------------------------------------------------------
// 0x00575fe0  Recursive BVH contact-node expansion: descends the shape tree at
//   param_4, emitting contact records into the node arena (param_1) and recursing
//   through the child support methods; branches on the shape kind word (*+0x5c).
//   Bounds-checks the two arena counters at +0x10/+0x1c against +0x14/+0x20.
// ---------------------------------------------------------------------------
extern "C" uint __cdecl
FUN_00575fe0(int param_1,uint *param_2,undefined4 param_3,int param_4,uint *param_5,
             int param_6,uint *param_7,float *param_8,float *param_9,uint param_10)
{
  short *psVar1;
  float fVar2;
  uint uVar3, uVar5;
  uint *puVar4, *puVar9, *puVar11, *puVar12, *puVar14;
  float *pfVar6, *pfVar10, *pfVar13, *pfVar15;
  int iVar7, iVar8;
  uint local_30;
  int local_2c;
  float local_20[7];                         // AABB: [0..2] max-ish, [4..6] min-ish, gap [3]

  uVar5 = ((int)param_7 - *(int *)(param_1 + 0xc)) / 0x14;
  uVar3 = (uint)((int)param_5 - *(int *)(param_1 + 0x18)) >> 6;
  *(uint *)(param_1 + 0x10) = uVar5;
  *(uint *)(param_1 + 0x1c) = uVar3;
  if ((uVar3 < *(uint *)(param_1 + 0x20)) && (uVar5 < *(uint *)(param_1 + 0x14))) {
    *(uint *)(param_1 + 0x1c) = uVar3 + 1;
    psVar1 = *(short **)(param_4 + 0x5c);
    if ((*(byte *)(psVar1 + 0x20) & 1) == 0) {
      if (*psVar1 != 0) {
        param_7[4] = (uint)param_5;
        *param_7 = *param_2;
        param_7[1] = param_2[1];
        *(undefined2 *)(param_7 + 1) = (undefined2)param_3;
        param_7[2] = (uint)param_4;
        param_7[3] = (uint)param_6;
        *(int *)(param_1 + 0x10) = *(int *)(param_1 + 0x10) + 1;
        return 1;
      }
    }
    else {
      fVar2 = *(float *)(param_1 + 0x70);
      param_6 = param_6 * 0x10000;
      if (*psVar1 == 8) {
        local_30 = 0;
        puVar9 = (uint *)**(uint **)(param_4 + 0x40);
        uVar5 = (*(uint **)(param_4 + 0x40))[1];
        puVar11 = param_7;
        if (uVar5 != 0) {
          while( true ) {
            iVar8 = (int)puVar9[0x17];
            if ((*(uint *)(param_1 + 0x20) <= *(uint *)(param_1 + 0x1c)) ||
               (*(uint *)(param_1 + 0x14) <= *(uint *)(param_1 + 0x10))) break;
            puVar4 = (uint *)(*(uint *)(param_1 + 0x1c) * 0x40 + *(int *)(param_1 + 0x18));
            pfVar6 = (float *)(*(uint *)(param_1 + 0x10) * 0x20 + *(int *)(param_1 + 0x24));
            if ((*(byte *)(iVar8 + 0x40) & 2) == 0) {
              if (param_5 == (uint *)0x0) {
                puVar12 = puVar9;
                puVar14 = puVar4;
                for (iVar7 = 0x10; puVar11 = param_7, iVar7 != 0; iVar7 = iVar7 + -1) {
                  *puVar14 = *puVar12;
                  puVar12 = puVar12 + 1;
                  puVar14 = puVar14 + 1;
                }
              }
              else {
                FUN_004c4600(puVar4,puVar9,param_5);
              }
            }
            else {
              puVar12 = param_5;
              puVar14 = puVar4;
              for (iVar7 = 0x10; puVar11 = param_7, iVar7 != 0; iVar7 = iVar7 + -1) {
                *puVar14 = *puVar12;
                puVar12 = puVar12 + 1;
                puVar14 = puVar14 + 1;
              }
            }
            if (param_8 == (float *)0x0) {
              ((rwp_support4)(*(void **)(iVar8 + 0x10)))(puVar9,puVar4,0,pfVar6);
              if (((uVar5 < 2) || (pfVar6 == param_9)) ||
                 (((pfVar6[4] - *param_9 < fVar2) != (pfVar6[4] - *param_9 == fVar2) &&
                  (((((param_9[4] - *pfVar6 < fVar2) != (param_9[4] - *pfVar6 == fVar2) &&
                     ((pfVar6[5] - param_9[1] < fVar2) != (pfVar6[5] - param_9[1] == fVar2))) &&
                    ((param_9[5] - pfVar6[1] < fVar2) != (param_9[5] - pfVar6[1] == fVar2))) &&
                   (((pfVar6[6] - param_9[2] < fVar2) != (pfVar6[6] - param_9[2] == fVar2) &&
                    ((param_9[6] - pfVar6[2] < fVar2) != (param_9[6] - pfVar6[2] == fVar2)))))))))
              goto LAB_005761fb;
            }
            else {
              pfVar13 = param_8;
              for (iVar8 = 8; puVar11 = param_7, iVar8 != 0; iVar8 = iVar8 + -1) {
                *pfVar6 = *pfVar13;
                pfVar13 = pfVar13 + 1;
                pfVar6 = pfVar6 + 1;
              }
LAB_005761fb:
              FUN_00575fe0(param_1,param_2,param_3,(int)puVar9,puVar4,local_30 + 1 + param_6,puVar11,
                           param_8,param_9,param_10);
              puVar11 = (uint *)(*(int *)(param_1 + 0xc) + *(int *)(param_1 + 0x10) * 0x14);
              param_7 = puVar11;
            }
            local_30 = local_30 + 1;
            puVar9 = puVar9 + 0x18;
            if (uVar5 <= local_30) {
              return uVar5;
            }
          }
          *(undefined4 *)(param_1 + 0x78) = 1;
        }
        return uVar5;
      }
      local_20[0] = fVar2 + *param_9;
      local_20[1] = param_9[1] + fVar2;
      local_20[2] = param_9[2] + fVar2;
      local_20[4] = param_9[4] - fVar2;
      local_20[5] = param_9[5] - fVar2;
      local_20[6] = param_9[6] - fVar2;
      if ((*(byte *)(psVar1 + 0x20) & 4) == 0) {
        FUN_00566200(&local_20[0],&local_20[0],(float *)param_5);
        uVar5 = ((rwp_clip4)(*(void **)(param_4 + 0x44)))(
                  (void *)param_4,&local_20[0],param_10,(void *)(param_1 + 0xc));
        iVar8 = *(int *)(param_1 + 0xc);
        local_2c = (((int)param_7 - iVar8) / 0x14) * 0x20 + *(int *)(param_1 + 0x24);
        local_30 = 0;
        puVar11 = param_7;
        param_7 = (uint *)(iVar8 + *(int *)(param_1 + 0x10) * 0x14);
        if (uVar5 != 0) {
          while (*(uint *)(param_1 + 0x1c) < *(uint *)(param_1 + 0x20)) {
            puVar9 = (uint *)puVar11[2];
            puVar4 = (uint *)(*(uint *)(param_1 + 0x1c) * 0x40 + *(int *)(param_1 + 0x18));
            iVar8 = (int)puVar9[0x17];
            *puVar11 = *param_2;
            *(undefined2 *)(puVar11 + 1) = (undefined2)param_3;
            puVar11[4] = (uint)puVar4;
            puVar11[3] = puVar11[3] + param_6 + 1;
            if ((*(byte *)(iVar8 + 0x40) & 2) == 0) {
              if (param_5 == (uint *)0x0) {
                puVar12 = puVar9;
                puVar14 = puVar4;
                for (iVar7 = 0x10; iVar7 != 0; iVar7 = iVar7 + -1) {
                  *puVar14 = *puVar12;
                  puVar12 = puVar12 + 1;
                  puVar14 = puVar14 + 1;
                }
              }
              else {
                FUN_004c4600(puVar4,puVar9,param_5);
              }
            }
            else {
              puVar12 = param_5;
              puVar14 = puVar4;
              for (iVar7 = 0x10; iVar7 != 0; iVar7 = iVar7 + -1) {
                *puVar14 = *puVar12;
                puVar12 = puVar12 + 1;
                puVar14 = puVar14 + 1;
              }
            }
            ((rwp_support4)(*(void **)(iVar8 + 0x10)))(puVar9,puVar4,0,(void *)local_2c);
            if ((*(byte *)(iVar8 + 0x40) & 1) == 0) {
              *(int *)(param_1 + 0x1c) = *(int *)(param_1 + 0x1c) + 1;
            }
            else {
              FUN_00575fe0(param_1,param_2,param_3,(int)puVar9,puVar4,puVar11[3],param_7,0,param_9,
                           param_10);
              iVar8 = *(int *)(param_1 + 0x10);
              iVar7 = *(int *)(param_1 + 0xc);
              puVar11[2] = 1;
              param_7 = (uint *)(iVar7 + iVar8 * 0x14);
            }
            local_30 = local_30 + 1;
            puVar11 = puVar11 + 5;
            local_2c = local_2c + 0x20;
            if (uVar5 <= local_30) {
              return uVar5;
            }
          }
          *(undefined4 *)(param_1 + 0x78) = 1;
        }
        return uVar5;
      }
      iVar8 = ((rwp_clip5)(*(void **)(param_4 + 0x48)))(
                (void *)param_4,param_5,&local_20[0],param_10,(void *)(param_1 + 0x44));
      if (iVar8 != 0) {
        pfVar6 = *(float **)(param_1 + 0x44);
        param_9 = (float *)0x0;
        param_10 = 0;
        if (*(int *)(param_1 + 0x48) == 0) {
          return 0;
        }
        do {
          if (((uint)*(uint *)(pfVar6 + 0x12) & 0x10) == 0) {
            uVar5 = *(uint *)(param_1 + 0x5c);
            if ((*(uint *)(param_1 + 0x60) <= uVar5) ||
               (*(uint *)(param_1 + 0x14) <= *(uint *)(param_1 + 0x10))) {
              *(undefined4 *)(param_1 + 0x78) = 1;
              return (uint)param_9;
            }
            pfVar10 = (float *)(uVar5 * 0x60 + *(int *)(param_1 + 0x58));
            *(uint *)(param_1 + 0x5c) = uVar5 + 1;
            pfVar13 = pfVar6;
            pfVar15 = pfVar10;
            for (iVar8 = 0x13; iVar8 != 0; iVar8 = iVar8 + -1) {
              *pfVar15 = *pfVar13;
              pfVar13 = pfVar13 + 1;
              pfVar15 = pfVar15 + 1;
            }
            *(uint *)(pfVar10 + 0x17) = 0x005e5db0u;     // MOV [.+0x5c],0x5e5db0
            pfVar10[0x13] = *(float *)(param_4 + 0x4c);
            pfVar10[0x14] = *(float *)(param_4 + 0x50);
            pfVar10[0x15] = *(float *)(param_4 + 0x54);
            *(undefined2 *)((int)pfVar10 + 0x5a) = *(undefined2 *)(param_4 + 0x5a);
            if ((*(byte *)(param_1 + 0x50) & 2) != 0) {
              pfVar13 = pfVar10 + 0xc;
              *pfVar13 = (pfVar6[10] - pfVar6[2]) * pfVar6[5] +
                         (pfVar6[2] - pfVar6[6]) * pfVar6[9] + (pfVar6[6] - pfVar6[10]) * pfVar6[1];
              pfVar10[0xd] = (*pfVar6 - pfVar6[4]) * pfVar6[10] +
                             (pfVar6[8] - *pfVar6) * pfVar6[6] + (pfVar6[4] - pfVar6[8]) * pfVar6[2];
              pfVar10[0xe] = (pfVar6[9] - pfVar6[1]) * pfVar6[4] +
                             (pfVar6[5] - pfVar6[9]) * *pfVar6 + (pfVar6[1] - pfVar6[5]) * pfVar6[8];
              FUN_005667c0(pfVar13,pfVar13);
            }
            pfVar10[7] = pfVar10[0xe] * pfVar6[2] +
                         pfVar10[0xc] * *pfVar6 + pfVar10[0xd] * pfVar6[1];
            iVar8 = *(int *)(param_1 + 0x10);
            iVar7 = *(int *)(param_1 + 0x24);
            puVar11 = (uint *)(*(int *)(param_1 + 0xc) + iVar8 * 0x14);
            *puVar11 = *param_2;
            puVar11[1] = param_2[1];
            *(undefined2 *)(puVar11 + 1) = (undefined2)param_3;
            puVar11[2] = (uint)pfVar10;
            puVar11[4] = (uint)param_5;
            FUN_0055bd70((int)pfVar10,param_5,0,(void *)(iVar8 * 0x20 + iVar7));
            puVar11[3] = (int)pfVar6[0x11] + param_6;
            param_9 = (float *)((int)param_9 + 1);
            *(int *)(param_1 + 0x10) = *(int *)(param_1 + 0x10) + 1;
          }
          param_10 = param_10 + 1;
          pfVar6 = pfVar6 + 0x13;
          if (*(uint *)(param_1 + 0x48) <= param_10) {
            return (uint)param_9;
          }
        } while( true );
      }
    }
  }
  else {
    *(undefined4 *)(param_1 + 0x78) = 1;
  }
  return 0;
}

// ---------------------------------------------------------------------------
// 0x00578ff0  Manifold merge/reduce: buckets the raw manifolds (param_1, stride
//   0xe0) by tag, coplanar-merges pairs (FUN_00579b50), then for each surviving
//   group builds the averaged normal (FUN_005667c0), and either copies the group
//   verbatim (<5 contacts) or reduces to a 4-point tetra by max/min projection on
//   two in-plane axes, writing results into param_4.
//   [+0xac contact counts load via FILD; list-walks via +0xdc are int pointer
//    chases; +0xa8 flag and the group index are ints — see header note.]
// ---------------------------------------------------------------------------
extern "C" uint __cdecl
FUN_00578ff0(uint *param_1,uint param_2,int param_3,uint *param_4,uint param_5,float param_6)
{
  undefined4 uVar1;
  float fVar2;
  uint *puVar3, *puVar12;
  uint **ppuVar4;
  uint uVar5, uVar8;
  int iVar6, iVar7;
  int *piVar9;
  float *pfVar10, *pfVar11, *pfVar14;
  uint *puVar13;
  uint *puVar15;
  float fVar16, fVar17;
  uint local_bc;
  float local_b4;                            // NOTE: int group index (INC EAX), see header
  float *local_b0;
  float local_ac[3];                         // normal accumulator (address-taken vec3)
  float local_a4_unused;                     // (padding placeholder; local_ac[2] is local_a4)
  uint local_a0;
  float *local_9c, *local_98, *local_90;
  uint local_94;
  float local_8c, local_88, local_84;
  float local_7c, local_78, local_74;
  int local_70;
  uint local_6c;
  float local_68, local_64;
  uint *local_60;
  float local_5c, local_58, local_54, local_50;
  undefined4 uStack_4c;
  uint local_48;
  undefined4 uStack_44;
  uint *local_40[16];
  (void)local_a4_unused; (void)local_70; (void)uStack_4c; (void)uStack_44; (void)local_48;

  local_b0 = (float *)0x0;
  local_90 = (float *)0x0;
  local_98 = (float *)0x0;
  local_9c = (float *)0x0;
  if (param_2 == 0) {
    return 0;
  }
  if (param_5 == 0) {
    return 0;
  }
  if (param_2 == 1) {
    puVar13 = param_1;
    if (param_1[0x2b] == 0) {
      return 0;
    }
    goto LAB_005793b0;
  }
  if (param_3 != 0) {
    iVar6 = 0x10;
    if (param_2 < 5) {
      puVar12 = (uint *)0x0;
      uVar5 = 0;
      local_bc = 1;
      local_40[0] = (uint *)0x0;
      if (param_2 != 0) {
        puVar3 = param_1 + 0x37;
        uVar8 = param_2;
        do {
          if (puVar3[-0xc] != 0) {
            *puVar3 = (uint)puVar12;
            puVar12 = puVar3 + -0x37;
            uVar5 = uVar5 + 1;
          }
          puVar3 = puVar3 + 0x38;
          uVar8 = uVar8 - 1;
        } while (uVar8 != 0);
        local_40[0] = puVar12;
        if (1 < uVar5) goto LAB_00579108;
      }
    }
    else {
      local_bc = 0x10;
      ppuVar4 = local_40;
      for (; iVar6 != 0; iVar6 = iVar6 + -1) {
        *ppuVar4 = (uint *)0x0;
        ppuVar4 = ppuVar4 + 1;
      }
      if (param_2 != 0) {
        puVar12 = param_1 + 0xb;
        uVar5 = param_2;
        do {
          if (puVar12[0x20] != 0) {
            puVar12[0x2c] = (uint)local_40[*puVar12 & 0xf];
            local_40[*puVar12 & 0xf] = puVar12 + -0xb;
          }
          puVar12 = puVar12 + 0x38;
          uVar5 = uVar5 - 1;
        } while (uVar5 != 0);
      }
LAB_00579108:
      puVar12 = local_40[0];
      uVar5 = 0;
      if (local_bc != 0) {
        do {
          for (puVar3 = local_40[uVar5]; puVar3 != (uint *)0x0; puVar3 = (uint *)puVar3[0x37]) {
            for (uVar8 = puVar3[0x37]; uVar8 != 0; uVar8 = *(uint *)(uVar8 + 0xdc)) {
              if ((*(byte *)((int)puVar3 + 0xcd) == 2) && (*(byte *)(uVar8 + 0xcd) < 2)) {
                if (((puVar3[0xb] == *(uint *)(uVar8 + 0x2c)) &&
                    ((float)puVar3[6] < *(float *)(uVar8 + 0x18))) &&
                   (iVar6 = (int)FUN_00579b50((float *)puVar3,(int)uVar8,param_6), iVar6 != 0)) {
                  *(undefined4 *)(uVar8 + 0xac) = 0;
                }
              }
              else if (((*(byte *)(uVar8 + 0xcd) == 2) &&
                       (((*(byte *)((int)puVar3 + 0xcd) < 2 &&
                         (puVar3[0xb] == *(uint *)(uVar8 + 0x2c))) &&
                        (*(float *)(uVar8 + 0x18) < (float)puVar3[6])))) &&
                      (iVar6 = (int)FUN_00579b50((float *)uVar8,(int)puVar3,param_6), iVar6 != 0)) {
                puVar3[0x2b] = 0;
                break;
              }
            }
          }
          uVar5 = uVar5 + 1;
        } while (uVar5 < local_bc);
      }
      if (local_bc == 1) {
        ppuVar4 = local_40;
        while (puVar12 != (uint *)0x0) {
          puVar12 = *ppuVar4;
          if (puVar12[0x2b] == 0) {
            *ppuVar4 = (uint *)puVar12[0x37];
          }
          else {
            ppuVar4 = (uint **)(puVar12 + 0x37);
          }
          puVar12 = *ppuVar4;
        }
      }
      else {
        ppuVar4 = local_40;
        for (iVar6 = 0x10; iVar6 != 0; iVar6 = iVar6 + -1) {
          *ppuVar4 = (uint *)0x0;
          ppuVar4 = ppuVar4 + 1;
        }
        if (param_2 != 0) {
          puVar12 = param_1 + 0xc;
          uVar5 = param_2;
          do {
            if (puVar12[0x1f] != 0) {
              puVar12[0x2b] = (uint)local_40[*puVar12 & 0xf];
              local_40[*puVar12 & 0xf] = puVar12 + -0xc;
            }
            puVar12 = puVar12 + 0x38;
            uVar5 = uVar5 - 1;
          } while (uVar5 != 0);
        }
      }
      uVar5 = 0;
      if (local_bc != 0) {
        do {
          for (puVar12 = local_40[uVar5]; puVar12 != (uint *)0x0; puVar12 = (uint *)puVar12[0x37]) {
            for (uVar8 = puVar12[0x37]; uVar8 != 0; uVar8 = *(uint *)(uVar8 + 0xdc)) {
              if (((byte)puVar12[0x33] == 2) && (*(byte *)(uVar8 + 0xcc) < 2)) {
                if (((puVar12[0xc] == *(uint *)(uVar8 + 0x30)) &&
                    ((float)puVar12[6] < *(float *)(uVar8 + 0x18))) &&
                   (iVar6 = (int)FUN_00579b50((float *)puVar12,(int)uVar8,param_6), iVar6 != 0)) {
                  *(undefined4 *)(uVar8 + 0xac) = 0;
                }
              }
              else if (((*(byte *)(uVar8 + 0xcc) == 2) &&
                       ((((byte)puVar12[0x33] < 2 && (puVar12[0xc] == *(uint *)(uVar8 + 0x30))) &&
                        (*(float *)(uVar8 + 0x18) < (float)puVar12[6])))) &&
                      (iVar6 = (int)FUN_00579b50((float *)uVar8,(int)puVar12,param_6), iVar6 != 0)) {
                puVar12[0x2b] = 0;
                break;
              }
            }
          }
          uVar5 = uVar5 + 1;
        } while (uVar5 < local_bc);
      }
    }
  }
  iVar6 = 0;
  uVar5 = 0;
  uVar8 = 0;
  if (param_2 != 0) {
    piVar9 = (int *)(param_1 + 0x2b);
    do {
      if (*piVar9 != 0) {
        iVar6 = iVar6 + *piVar9;
        uVar5 = uVar8;
      }
      uVar8 = uVar8 + 1;
      piVar9 = piVar9 + 0x38;
    } while (uVar8 < param_2);
    if (iVar6 != 0) {
      puVar13 = param_1 + uVar5 * 0x38;
      if ((uint)iVar6 != param_1[uVar5 * 0x38 + 0x2b]) {
        local_a0 = 0;
        pfVar10 = (float *)(param_1 + 1);
        local_b4 = 0.0f;
        do {
          if (*(int *)(pfVar10 + 0x2a) != 0) {           // +0xa8 int flag (TEST EAX)
            uVar5 = 0;
            if (local_a0 != 0) {
              pfVar11 = pfVar10 + -1;
              piVar9 = (int *)(param_4 + 0x36);
              do {
                pfVar14 = (float *)(param_1 + *piVar9 * 0x38);
                if (_DAT_005cc9b4 <
                    pfVar10[1] * pfVar14[2] + *pfVar11 * *pfVar14 + pfVar14[1] * *pfVar10) {
                  local_94 = (uint)(fabsf(pfVar10[1] * (pfVar10[4] - pfVar14[5]) +
                                        *pfVar11 * (pfVar10[2] - pfVar14[3]) +
                                        *pfVar10 * (pfVar10[3] - pfVar14[4])) <
                                   param_6 * _DAT_005cc328);
                  if ((local_94 != 0) ||
                     (fabsf(pfVar14[2] * (pfVar10[4] - pfVar14[5]) +
                          (pfVar10[2] - pfVar14[3]) * *pfVar14 +
                          pfVar14[1] * (pfVar10[3] - pfVar14[4])) < param_6 * _DAT_005cc328)) {
                    *(uint *)(pfVar10 + 0x36) = *(uint *)(pfVar14 + 0x37);   // list link (int)
                    *(uint *)(pfVar14 + 0x37) = (uint)pfVar11;
                    break;
                  }
                }
                uVar5 = uVar5 + 1;
                piVar9 = piVar9 + 0x38;
              } while (uVar5 < local_a0);
            }
            if ((uVar5 == local_a0) && (uVar5 < param_5)) {
              local_a0 = local_a0 + 1;
              param_4[uVar5 * 0x38 + 0x36] = (uint)(int)local_b4;     // int group index
              *(uint *)(pfVar10 + 0x36) = 0;
            }
          }
          local_b4 = (float)((int)local_b4 + 1);
          pfVar10 = pfVar10 + 0x38;
        } while ((uint)(int)local_b4 < param_2);
        if (local_a0 != 0) {
          param_4 = param_4 + 0x2c;
          local_6c = local_a0;
          do {
            pfVar10 = (float *)(param_1 + param_4[10] * 0x38);
            if (*(uint *)(pfVar10 + 0x37) == 0) {          // list-next (int) == 0
              uVar1 = *param_4;
              pfVar11 = (float *)(param_4 + -0x2c);
              for (iVar6 = 0x38; iVar6 != 0; iVar6 = iVar6 + -1) {
                *pfVar11 = *pfVar10;
                pfVar10 = pfVar10 + 1;
                pfVar11 = pfVar11 + 1;
              }
              *param_4 = uVar1;
            }
            else {
              uVar5 = 0;
              for (puVar13 = param_1 + param_4[10] * 0x38; puVar13 != (uint *)0x0;
                  puVar13 = (uint *)puVar13[0x37]) {
                uVar5 = uVar5 + puVar13[0x2b];
              }
              local_84 = DAT_005d757c;
              local_88 = DAT_005d757c;
              local_ac[2] = 0.0f;            // local_a4
              local_8c = DAT_005d757c;
              local_ac[1] = 0.0f;            // local_a8
              local_ac[0] = 0.0f;            // local_ac
              if (pfVar10 != (float *)0x0) {
                fVar17 = 3.4028235e+38f;
                pfVar11 = pfVar10;
                do {
                  uint cnt = *(uint *)(pfVar11 + 0x2b);          // +0xac count (FILD below)
                  pfVar14 = pfVar11 + 3;
                  fVar16 = (float)cnt;                           // FILD int->float
                  local_ac[0] = fVar16 * *pfVar11 + local_ac[0];
                  local_ac[1] = pfVar11[1] * fVar16 + local_ac[1];
                  local_ac[2] = pfVar11[2] * fVar16 + local_ac[2];
                  for (int i = (int)cnt; i != 0; i = i + -1) {
                    local_8c = local_8c + *pfVar14;
                    local_88 = pfVar14[1] + local_88;
                    local_84 = pfVar14[2] + local_84;
                    if (pfVar14[3] < fVar17) {
                      fVar17 = pfVar14[3];
                      local_b0 = pfVar14;
                    }
                    pfVar14 = pfVar14 + 10;
                  }
                  pfVar11 = *(float **)(pfVar11 + 0x37);         // list-next (pointer)
                } while (pfVar11 != (float *)0x0);
              }
              local_60 = param_4;
              FUN_005667c0(local_ac,local_ac);
              fVar17 = _DAT_005cc320 / (float)uVar5;             // FILD int->float
              local_48 = uVar5;
              if (uVar5 < 5) {
                uVar1 = *param_4;
                pfVar11 = pfVar10;
                pfVar14 = (float *)(param_4 + -0x2c);
                for (iVar6 = 0x38; iVar6 != 0; iVar6 = iVar6 + -1) {
                  *pfVar14 = *pfVar11;
                  pfVar11 = pfVar11 + 1;
                  pfVar14 = pfVar14 + 1;
                }
                *param_4 = uVar1;
                for (int nd = *(int *)(pfVar10 + 0x37); nd != 0; nd = *(int *)(nd + 0xdc)) {
                  float *vtx = (float *)(nd + 0xc);
                  int cnt2 = *(int *)(nd + 0xac);
                  while (cnt2 != 0) {
                    uVar5 = 0;
                    pfVar11 = (float *)(param_4 + -0x29);
                    if (param_4[-1] != 0) {
                      do {
                        if (fabsf(*vtx - *pfVar11) +
                            fabsf(vtx[1] - pfVar11[1]) + fabsf(vtx[2] - pfVar11[2]) <
                            param_6 * _DAT_005cc558) goto LAB_005797c4;
                        uVar5 = uVar5 + 1;
                        pfVar11 = pfVar11 + 10;
                      } while (uVar5 < (uint)param_4[-1]);
                    }
                    pfVar14 = vtx;
                    for (iVar7 = 10; iVar7 != 0; iVar7 = iVar7 + -1) {
                      *pfVar11 = *pfVar14;
                      pfVar14 = pfVar14 + 1;
                      pfVar11 = pfVar11 + 1;
                    }
                    param_4[-1] = param_4[-1] + 1;
LAB_005797c4:
                    vtx = vtx + 10;
                    cnt2 = cnt2 + -1;
                  }
                }
                ((float *)param_4)[-0x2c] = local_ac[0];
                ((float *)param_4)[-0x2b] = local_ac[1];
                ((float *)param_4)[-0x2a] = local_ac[2];
                param_4 = local_60;
              }
              else {
                fVar16 = local_8c * fVar17 - *local_b0;
                local_7c = local_88 * fVar17 - local_b0[1];
                local_78 = fVar17 * local_84 - local_b0[2];
                if (fabsf(fVar16) + fabsf(local_78) + fabsf(local_7c) < (float)PTR_DAT_005ceabc) {
                  fVar16 = _DAT_005cc320;
                }
                local_74 = -3.4028235e+38f;
                local_b4 = -3.4028235e+38f;
                local_68 = 3.4028235e+38f;
                local_5c = local_78 * local_ac[1] - local_7c * local_ac[2];
                local_64 = 3.4028235e+38f;
                local_58 = local_ac[2] * fVar16 - local_78 * local_ac[0];
                local_54 = local_7c * local_ac[0] - fVar16 * local_ac[1];
                for (pfVar11 = pfVar10; pfVar11 != (float *)0x0; pfVar11 = *(float **)(pfVar11 + 0x37)) {
                  pfVar14 = pfVar11 + 3;
                  for (int i = *(int *)(pfVar11 + 0x2b); i != 0; i = i + -1) {   // +0xac int count
                    fVar2 = pfVar14[2] * local_78 + fVar16 * *pfVar14 + pfVar14[1] * local_7c;
                    if (local_b4 < fVar2) {
                      local_b4 = fVar2;
                      local_b0 = pfVar14;
                    }
                    if (fVar2 < local_64) {
                      local_90 = pfVar14;
                      local_64 = fVar2;
                    }
                    fVar2 = pfVar14[2] * local_54 + local_5c * *pfVar14 + pfVar14[1] * local_58;
                    if (local_74 < fVar2) {
                      local_98 = pfVar14;
                      local_74 = fVar2;
                    }
                    if (fVar2 < local_68) {
                      local_9c = pfVar14;
                      local_68 = fVar2;
                    }
                    pfVar14 = pfVar14 + 10;
                  }
                }
                uVar1 = *param_4;
                pfVar11 = (float *)(param_4 + -0x29);
                pfVar14 = (float *)(param_4 + -0x2c);
                for (iVar6 = 0x38; iVar6 != 0; iVar6 = iVar6 + -1) {
                  *pfVar14 = *pfVar10;
                  pfVar10 = pfVar10 + 1;
                  pfVar14 = pfVar14 + 1;
                }
                *param_4 = uVar1;
                ((float *)param_4)[-0x2c] = local_ac[0];
                ((float *)param_4)[-0x2b] = local_ac[1];
                ((float *)param_4)[-0x2a] = local_ac[2];
                pfVar10 = local_b0;
                pfVar14 = pfVar11;
                for (iVar6 = 10; iVar6 != 0; iVar6 = iVar6 + -1) {
                  *pfVar14 = *pfVar10;
                  pfVar10 = pfVar10 + 1;
                  pfVar14 = pfVar14 + 1;
                }
                param_4[-1] = 1;
                if (local_98 != local_b0) {
                  uVar5 = 0;
                  pfVar10 = pfVar11;
                  if (param_4[-1] != 0) {
                    do {
                      if (fabsf(*local_98 - *pfVar10) +
                          fabsf(local_98[2] - pfVar10[2]) + fabsf(local_98[1] - pfVar10[1]) <
                          param_6 * _DAT_005cc558) goto LAB_00579a1b;
                      uVar5 = uVar5 + 1;
                      pfVar10 = pfVar10 + 10;
                    } while (uVar5 < (uint)param_4[-1]);
                  }
                  pfVar14 = local_98;
                  for (iVar6 = 10; iVar6 != 0; iVar6 = iVar6 + -1) {
                    *pfVar10 = *pfVar14;
                    pfVar14 = pfVar14 + 1;
                    pfVar10 = pfVar10 + 1;
                  }
                  param_4[-1] = param_4[-1] + 1;
                }
LAB_00579a1b:
                if ((local_90 != local_b0) && (local_90 != local_98)) {
                  uVar5 = 0;
                  pfVar10 = pfVar11;
                  if (param_4[-1] != 0) {
                    do {
                      if (fabsf(*local_90 - *pfVar10) +
                          fabsf(local_90[2] - pfVar10[2]) + fabsf(local_90[1] - pfVar10[1]) <
                          param_6 * _DAT_005cc558) goto LAB_00579a93;
                      uVar5 = uVar5 + 1;
                      pfVar10 = pfVar10 + 10;
                    } while (uVar5 < (uint)param_4[-1]);
                  }
                  pfVar14 = local_90;
                  for (iVar6 = 10; iVar6 != 0; iVar6 = iVar6 + -1) {
                    *pfVar10 = *pfVar14;
                    pfVar14 = pfVar14 + 1;
                    pfVar10 = pfVar10 + 1;
                  }
                  param_4[-1] = param_4[-1] + 1;
                }
LAB_00579a93:
                if (((local_9c != local_b0) && (local_9c != local_98)) && (local_9c != local_90)) {
                  uVar5 = 0;
                  if (param_4[-1] != 0) {
                    do {
                      if (fabsf(*local_9c - *pfVar11) +
                          fabsf(local_9c[2] - pfVar11[2]) + fabsf(local_9c[1] - pfVar11[1]) <
                          param_6 * _DAT_005cc558) goto LAB_00579b1b;
                      uVar5 = uVar5 + 1;
                      pfVar11 = pfVar11 + 10;
                    } while (uVar5 < (uint)param_4[-1]);
                  }
                  pfVar10 = local_9c;
                  for (iVar6 = 10; iVar6 != 0; iVar6 = iVar6 + -1) {
                    *pfVar11 = *pfVar10;
                    pfVar10 = pfVar10 + 1;
                    pfVar11 = pfVar11 + 1;
                  }
                  param_4[-1] = param_4[-1] + 1;
                }
              }
            }
LAB_00579b1b:
            param_4 = param_4 + 0x38;
            local_6c = local_6c - 1;
          } while (local_6c != 0);
        }
        return local_a0;
      }
LAB_005793b0:
      uVar1 = param_4[0x2c];
      puVar15 = param_4;
      for (iVar6 = 0x38; iVar6 != 0; iVar6 = iVar6 + -1) {
        *puVar15 = *puVar13;
        puVar13 = puVar13 + 1;
        puVar15 = puVar15 + 1;
      }
      param_4[0x2c] = uVar1;
      return 1;
    }
  }
  return 0;
}

// --- gta-reversed-style hook registration — CLUSTER 17. ---
RH_ScopedInstall(FUN_00575b60, 0x00575b60);
RH_ScopedInstall(FUN_00575fe0, 0x00575fe0);
RH_ScopedInstall(FUN_00578b20, 0x00578b20);
RH_ScopedInstall(FUN_00578bd0, 0x00578bd0);
RH_ScopedInstall(FUN_00578cb0, 0x00578cb0);
RH_ScopedInstall(FUN_00578d90, 0x00578d90);
RH_ScopedInstall(FUN_00578ff0, 0x00578ff0);
RH_ScopedInstall(FUN_0057a250, 0x0057a250);
RH_ScopedInstall(FUN_0057a660, 0x0057a660);

}  // namespace Collision
}  // namespace mashed_re
