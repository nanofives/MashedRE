// ============================================================================
//  RwpSolverCore18.cpp  —  B5e solver-island cluster K18
//  6 verbatim RWP-3.7 narrow-phase helpers (pair dispatch + manifold recursion
//  + SAT hull axis test + conservative-advancement TOI + poly clip):
//    FUN_005757d0 (0x005757d0, 170 B)  — 2-shape narrow-phase dispatch (support x2)
//    FUN_0056b7a0 (0x0056b7a0, 546 B)  — broad-phase pair emit w/ AABB overlap gate
//    FUN_00574ad0 (0x00574ad0,1615 B)  — polygon-vs-plane clip + fan reduce (float10)
//    FUN_00575c60 (0x00575c60, 890 B)  — recursive BVH manifold expansion (cdecl vtbl)
//    FUN_00578610 (0x00578610,1287 B)  — SAT convex-hull separating axis (float10)
//    FUN_0057a9a0 (0x0057a9a0,1035 B)  — conservative-advancement TOI iterate (float10)
//  Deps: K1 (FUN_005667c0), K2 (FUN_004c4600), K3 (FUN_0055abb0/FUN_0055ae50),
//  K5 (FUN_0055c230/FUN_0055bd80/FUN_0055c000), K16 (FUN_00575120/FUN_005751f0),
//  K17 (FUN_00575fe0 recursion, FUN_00578b20/bd0/cb0/d90, FUN_0057a250/FUN_0057a660).
//  All externed below; definitions live in the sibling K1..K17 TUs of the same .asi.
//  Version anchor: MASHED.exe 2,846,720 B / SHA-256 BDCAE093…3C0E.
//
//  All float math is x87 (FLD/FMUL/FSTP/FDIV/FILD confirmed by disasm) — plain C
//  float compiles to x87 under /arch:IA32, so this is bit-faithful. Ghidra's
//  float-compare idioms `(a<b)==(a==b)` (= a>=b) and `(a<b)!=(a==b)` (= a<=b) are
//  kept VERBATIM. float10 = x87 80-bit ST0 return (must be non-void).
//
//  DISASM-RESOLVED CONVENTIONS (verified against listing, pool0 session):
//   * FUN_005757d0: FUN_0055c230 is __cdecl 5-arg (ADD ESP,0x14 @0x575824/0x575869);
//     the 3 negated floats [ESP+0x10/14/18] are ONE contiguous vec3 (LEA ECX passed).
//   * FUN_0056b7a0: FUN_0055bd80 __cdecl 4-arg (ADD ESP,0x10), FUN_0055abb0 __cdecl
//     3-arg (ADD ESP,0xc), FUN_0055ae50 __cdecl 2-arg (ADD ESP,8). The two AABBs are
//     independent 7-float buffers (gap at [3]); overlap test pairs nodeAABB/candAABB.
//     `local_44` = `MOV word[ESP+0x20],0xffff` (@0x56b801) on an otherwise-untouched
//     dword; only low 16 bits are written (faithful partial-init — high half is the
//     original's leftover stack; consumer masks to 16 via +0x5a handle). [UNCERTAIN]
//     high-half value, non-load-bearing.
//   * FUN_00575c60: exact twin of K17 FUN_00575fe0 — CALL 0x00575fe0 is 10-arg __cdecl
//     (ADD ESP,0x28 @0x575d5c/0x575e62); vtable [EAX+0x10] is __cdecl 4-arg (ADD
//     ESP,0x10 @0x575ea7) = rwp_support4; FUN_004c4600 __cdecl 3-arg; 16-/8-dword
//     MOVSD.REP block copies; `MOV word[EDI-0x10],CX` = *(u16*)(puVar15+1)=local_18.
//   * FUN_00578610: vtable [EBP+0x1c] / [ECX+0x1c] are __cdecl 4-arg (ADD ESP,0x20 for
//     the pair @0x5787c0); FUN_005667c0 2-arg float10; FUN_00578bd0/cb0/d90 4-arg,
//     FUN_00578b20 3-arg. fStack_100+afStack_fc = ONE 12-float edge buffer (edgeB),
//     walk pointer starts at edgeB+1 (pfVar12[-1]=edgeB[0]). fStack_138/134/130 =
//     contiguous cross-product vec3 (&passed to FUN_00578b20). float10 fVar13 depth is
//     carried in ST0 across the branch JMPs to the shared tail (kept as long double).
//   * FUN_0057a9a0: FUN_0057a660 2-arg float10, FUN_0057a250 8-arg int, FUN_0055c000
//     4-arg. d0={local_1e0/1dc/1d8}, d1={local_1d4/1d0/1cc} are two vec3s (no single &
//     spans them). cp={local_1bc/1b8/1b4} = contact-point vec3 out of FUN_0057a250.
//     local_1b0[432] = scratch buffer passed as int.
//  CONSTS (memory_read @pool0): 005cc328=0.01f 005cc32c=0.5f 005cc33c=-1.0f
//    005d757c=0.0f 005cc320=1.0f 005ce54c=1.00099994e-6f 005cc9a0=0.05f 005cc9dc=0.95f
//    005ceac0=3.4028235e38f(FLT_MAX) 005e5050=-3.4028235e38f(-FLT_MAX).
// ============================================================================
#include "../Core/HookSystem.h"
#include <cmath>       // fabsf (ABS)

namespace mashed_re {
namespace Collision {

typedef unsigned int   uint;
typedef unsigned int   undefined4;
typedef unsigned char  undefined1;
typedef unsigned short undefined2;
typedef unsigned short ushort;
typedef unsigned char  byte;
typedef long double    float10;      // x87 80-bit ST0 return

#define DAT_005d757c   (*(const float*)0x005d757cu)   // 0.0f
#define _DAT_005cc320  (*(const float*)0x005cc320u)   // 1.0f
#define _DAT_005cc328  (*(const float*)0x005cc328u)   // 0.00999999978f
#define _DAT_005cc32c  (*(const float*)0x005cc32cu)   // 0.5f
#define _DAT_005cc33c  (*(const float*)0x005cc33cu)   // -1.0f
#define _DAT_005ce54c  (*(const float*)0x005ce54cu)   // 1.00099994e-06f
#define _DAT_005cc9a0  (*(const float*)0x005cc9a0u)   // 0.0500000007f
#define _DAT_005cc9dc  (*(const float*)0x005cc9dcu)   // 0.949999988f
#define _DAT_005ceac0  (*(const float*)0x005ceac0u)   // 3.40282347e+38f (FLT_MAX)
#define _DAT_005e5050  (*(const float*)0x005e5050u)   // -3.40282347e+38f (-FLT_MAX)

// --- extern callees (K1/K2/K3/K5/K16/K17, defined in sibling TUs of same .asi) ----
extern "C" int     __cdecl FUN_0055c230(int param_1,float *param_2,undefined4 param_3,float *param_4,undefined4 param_5); // 0x0055c230 (K5)
extern "C" void    __cdecl FUN_0055bd80(int param_1,int param_2,undefined4 param_3,undefined4 param_4); // 0x0055bd80 (K5)
extern "C" undefined2 __cdecl FUN_0055ae50(int *param_1,int param_2);                              // 0x0055ae50 (K3)
extern "C" undefined4 __cdecl FUN_0055abb0(undefined4 *param_1,int param_2,undefined4 param_3);    // 0x0055abb0 (K3)
extern "C" void *  __cdecl FUN_0055c000(int shape,float *mtx,float *dir,void *out);                // 0x0055c000 (K5)
extern "C" float10 __cdecl FUN_005667c0(float *param_1,float *param_2);                            // 0x005667c0 (K1)
extern "C" uint *  __cdecl FUN_004c4600(uint *param_1,uint *param_2,uint *param_3);                // 0x004c4600 (K2)
extern "C" undefined4 __cdecl FUN_00575120(float *param_1,float param_2,int param_3,float param_4); // 0x00575120 (K16) — arg2 is a float value (Ghidra mistyped u4)
extern "C" float10 __cdecl FUN_005751f0(float *param_1,float *param_2,float *param_3);             // 0x005751f0 (K16)
extern "C" int     __cdecl FUN_00575fe0(int param_1,uint *param_2,undefined4 param_3,int param_4,uint *param_5,int param_6,uint *param_7,float *param_8,float *param_9,uint param_10); // 0x00575fe0 (K17)
extern "C" int     __cdecl FUN_0057a250(int *param_1,float *param_2,float param_3,int param_4,float *param_5,float *param_6,float *param_7,float param_8); // 0x0057a250 (K17)
extern "C" float10 __cdecl FUN_0057a660(int *param_1,float *param_2);                              // 0x0057a660 (K17)
extern "C" float10 __cdecl FUN_00578b20(int *param_1,float *param_2,float *param_3);               // 0x00578b20 (K17)
extern "C" float10 __cdecl FUN_00578bd0(int *param_1,float *param_2,float *param_3,float *param_4);// 0x00578bd0 (K17)
extern "C" float10 __cdecl FUN_00578cb0(int *param_1,float *param_2,float *param_3,float *param_4);// 0x00578cb0 (K17)
extern "C" void    __cdecl FUN_00578d90(float *param_1,float *param_2,float *param_3,float *param_4); // 0x00578d90 (K17)

// __cdecl indirect-call prototype for the shape method-table support slot [.+0x10]
typedef void (__cdecl *rwp_support4)(void*,void*,int,void*);
// __cdecl indirect-call prototype for the hull-vertex-fetch method slot [.+0x1c]
typedef void (__cdecl *rwp_hull4)(int,undefined4,void*,void*);

// forward decls for RH_ScopedInstall
extern "C" bool       __cdecl FUN_005757d0(int *param_1,int param_2);
extern "C" int        __cdecl FUN_0056b7a0(undefined4 *param_1,int param_2,uint param_3,int param_4);
extern "C" undefined4 __cdecl FUN_00574ad0(float *param_1,float *param_2,uint param_3,float param_4,float param_5,undefined4 param_6);
extern "C" int        __cdecl FUN_00575c60(undefined4 *param_1,uint *param_2,float *param_3,float *param_4,float *param_5,undefined4 param_6);
extern "C" undefined4 __cdecl FUN_00578610(undefined4 param_1,int *param_2,float param_3,float *param_4,float *param_5,float *param_6);
extern "C" undefined4 __cdecl FUN_0057a9a0(undefined4 param_1,int *param_2,float param_3,float *param_4,float *param_5,float *param_6,float param_7);

// ---------------------------------------------------------------------------
// 0x005757d0  Two-shape narrow-phase dispatch: build the negated relative
//   translation (vec3) and run the support map twice (per body). Returns
//   nonzero only if both support calls succeed.
// ---------------------------------------------------------------------------
extern "C" bool __cdecl
FUN_005757d0(int *param_1,int param_2)
{
  float v[3];                                  // [ESP+0x10/14/18] contiguous vec3
  v[0] = -*(float *)(param_2 + 0x100);
  v[1] = -*(float *)(param_2 + 0x104);
  int iVar2 = *(int *)(param_2 + 0x50);
  v[2] = -*(float *)(param_2 + 0x108);
  int iVar1 = FUN_0055c230(*(int *)(*param_1 + 8), *(float **)(*param_1 + 0x10), 0, v,
                           (undefined4)param_2);
  bool bVar3 = false;
  if (iVar1 != 0) {
    *(int *)(param_2 + 0xd0) = iVar2;
    if (4 < *(ushort *)(param_2 + 0x54)) {
      *(uint *)(param_2 + 0xd0) = (uint)*(ushort *)(param_2 + 0x54) * 0x10 + iVar2;
    }
    iVar2 = FUN_0055c230(*(int *)(param_1[1] + 8), *(float **)(param_1[1] + 0x10), 1,
                         (float *)(param_2 + 0x100), (undefined4)(param_2 + 0x80));
    bVar3 = iVar2 != 0;
  }
  return bVar3;
}

// ---------------------------------------------------------------------------
// 0x0056b7a0  Broad-phase pair emit: for each candidate body in param_1[2..],
//   AABB-overlap-test against the node AABB and (optionally) a bit-filter mask,
//   then append a 0x14-byte pair record to the pair list at param_1[1]+0x80.
//   Returns the number of records appended.
// ---------------------------------------------------------------------------
extern "C" int __cdecl
FUN_0056b7a0(undefined4 *param_1,int param_2,uint param_3,int param_4)
{
  int *piVar1;
  int iVar4;
  uint uVar5;
  int iVar6;
  uint uVar7;
  int *local_54;
  uint local_50;
  int local_44;                                // [ESP+0x20] partial-init (low16=0xffff)
  float nodeAABB[7];                           // [ESP+0x44] FUN_0055bd80 out (gap [3])
  float candAABB[7];                           // [ESP+0x24] FUN_0055abb0 out (gap [3])

  int iVar2 = param_1[1];
  uint uVar3 = *(uint *)(iVar2 + 0x84);
  if (*(uint *)(iVar2 + 0x88) <= uVar3) {
    return 0;
  }
  local_54 = (int *)param_1[2];
  local_50 = 0;
  if (param_1[3] != 0) {
    do {
      iVar4 = *local_54;
      if (iVar4 != 0) {
        *(unsigned short *)&local_44 = 0xffffu;   // MOV word[ESP+0x20],0xffff — low16 only
        FUN_0055bd80(iVar4, 0, 0, (undefined4)nodeAABB);
        uVar7 = 0;
        if (param_3 != 0) {
          do {
            if ((((((param_4 == 0) ||
                   (iVar6 = *(int *)(param_2 + uVar7 * 4),
                   uVar5 = FUN_0055ae50(*(int **)(iVar6 + 0x24), iVar6),
                   uVar5 = *(int *)(param_4 + 4) * (uVar5 & 0xffff) +
                           (uint)*(ushort *)(*local_54 + 0x5a),
                   (*(uint *)(param_4 + 0xc + (uVar5 >> 5) * 4) & 1 << ((byte)uVar5 & 0x1f)) == 0))
                  && (iVar6 = *(int *)(param_2 + uVar7 * 4),
                     iVar6 = FUN_0055abb0(*(undefined4 **)(iVar6 + 0x24), iVar6, (undefined4)candAABB),
                     iVar6 != 0)) &&
                 ((iVar6 = *(int *)*param_1, nodeAABB[4] - candAABB[0] <= *(float *)(iVar6 + 0xc014) &&
                  (candAABB[4] - nodeAABB[0] <= *(float *)(iVar6 + 0xc014))))) &&
                ((nodeAABB[5] - candAABB[1] <= *(float *)(iVar6 + 0xc014) &&
                 ((candAABB[5] - nodeAABB[1] <= *(float *)(iVar6 + 0xc014) &&
                  (nodeAABB[6] - candAABB[2] <= *(float *)(iVar6 + 0xc014))))))) &&
               (candAABB[6] - nodeAABB[2] <= *(float *)(iVar6 + 0xc014))) {
              iVar6 = *(int *)(iVar2 + 0x84);
              *(int *)(iVar2 + 0x84) = iVar6 + 1;
              piVar1 = (int *)(*(int *)(iVar2 + 0x80) + iVar6 * 0x14);
              *piVar1 = iVar4;
              piVar1[1] = local_44;
              *(undefined2 *)(piVar1 + 3) = 0x8000;
              piVar1[2] = *(int *)(param_2 + uVar7 * 4);
              piVar1[4] = 0;
              if (*(int *)(iVar2 + 0x84) == *(int *)(iVar2 + 0x88)) goto LAB_0056b9ae;
            }
            uVar7 = uVar7 + 1;
          } while (uVar7 < param_3);
        }
      }
      local_50 = local_50 + 1;
      local_54 = local_54 + 1;
    } while (local_50 < (uint)param_1[3]);
  }
LAB_0056b9ae:
  return *(int *)(iVar2 + 0x84) - uVar3;
}

// ---------------------------------------------------------------------------
// 0x00574ad0  Polygon-vs-plane clip + convex fan reduce. Clips the input
//   polygon (param_2, param_3 verts) against the plane-pair described by
//   param_1, emitting contact points via FUN_00575120; when >4 clip verts
//   survive, iteratively drops the least-area vertex (FUN_005751f0 area, x87
//   float10) until 4 remain.
// ---------------------------------------------------------------------------
extern "C" undefined4 __cdecl
FUN_00574ad0(float *param_1,float *param_2,uint param_3,float param_4,float param_5,
            undefined4 param_6)
{
  float *pfVar1;
  float *pfVar2;
  float fVar3;
  float fVar4;
  uint uVar16;
  float fVar17;
  int iVar18;
  undefined4 uVar19;
  uint uVar20;
  float *pfVar21;
  float *pfVar22;
  uint uVar23;
  float *pfVar24;
  int iVar25;
  int iVar26;
  int iVar27;
  uint uVar28;
  uint uVar29;
  float10 fVar30;
  uint local_1e8;
  uint local_1e0;
  float *local_1dc;
  uint local_1d8;
  float *local_1d4;
  uint local_1d0;
  float afStack_1c0 [16];
  float afStack_180 [24];
  float local_120 [72];

  iVar25 = 0;
  if (param_3 != 0) {
    if (param_3 == 1) {
      uVar19 = FUN_00575120(param_2,(((param_1[0x22] * param_2[2] +
                                      param_1[0x20] * *param_2 + param_1[0x21] * param_2[1]) -
                                     param_1[0x23]) /
                                     (param_1[0x42] * param_1[0x22] +
                                     param_1[0x40] * param_1[0x20] + param_1[0x21] * param_1[0x41])
                                    - ((param_1[2] * param_2[2] +
                                       *param_1 * *param_2 + param_2[1] * param_1[1]) - param_1[3])
                                      / (param_1[2] * param_1[0x42] +
                                        param_1[0x40] * *param_1 + param_1[0x41] * param_1[1])) -
                                    param_5,(int)param_6,0.0f);
      return uVar19;
    }
    uVar28 = 0;
    local_1e0 = 0;
    fVar17 = DAT_005d757c;
    if (param_3 != 0) {
      pfVar24 = param_2 + 1;
      do {
        float fVar15 = ((((pfVar24[1] * param_1[0x22] +
                    *pfVar24 * param_1[0x21] + pfVar24[-1] * param_1[0x20]) - param_1[0x23]) /
                   (param_1[0x42] * param_1[0x22] + param_1[0x40] * param_1[0x20] + param_1[0x21] * param_1[0x41]) -
                  ((param_1[2] * pfVar24[1] + pfVar24[-1] * *param_1 + *pfVar24 * param_1[1]) -
                  param_1[3]) / (param_1[2] * param_1[0x42] + param_1[0x40] * *param_1 + param_1[1] * param_1[0x41])) - param_4) -
                 param_5;
        afStack_1c0[uVar28] = fVar15;
        if (fVar15 < *(float *)((int)afStack_1c0 + iVar25)) {
          iVar25 = uVar28 * 4;
          local_1e0 = uVar28;
        }
        if (uVar28 != 0) {
          fVar17 = fabsf(*param_2 - pfVar24[-1]) +
                   fabsf(param_2[2] - pfVar24[1]) + fabsf(param_2[1] - *pfVar24) + fVar17;
        }
        uVar28 = uVar28 + 1;
        pfVar24 = pfVar24 + 4;
      } while (uVar28 < param_3);
    }
    if (DAT_005d757c < afStack_1c0[local_1e0]) {
      return 1;
    }
    uVar28 = param_3 - 1;
    uVar29 = 0;
    local_1d8 = 0x10;
    local_1d0 = 0;
    fVar17 = (_DAT_005cc328 / (float)uVar28) * fVar17;
    if (param_3 != 0) {
      local_1d4 = local_120 + 1;
      local_1dc = local_120 + 2;
      iVar25 = uVar28 * 0x10;
      pfVar22 = local_120;
      pfVar24 = param_2 + 2;
      uVar20 = local_1d0;
      do {
        local_1d0 = uVar20;
        pfVar1 = afStack_1c0 + local_1d0;
        fVar3 = afStack_1c0[uVar28];
        pfVar2 = afStack_1c0 + uVar28;
        pfVar21 = pfVar22;
        if (afStack_1c0[local_1d0] < DAT_005d757c == (afStack_1c0[local_1d0] == DAT_005d757c)) {
          if (fVar3 < DAT_005d757c != (fVar3 == DAT_005d757c)) {
            afStack_180[uVar29] = 0.0;
            fVar3 = *pfVar2 / (*pfVar2 - *pfVar1);
            *pfVar22 = (pfVar24[-2] - *(float *)(iVar25 + (int)param_2)) * fVar3 +
                       *(float *)(iVar25 + (int)param_2);
            pfVar22 = (float *)(iVar25 + 8 + (int)param_2);
            *local_1d4 = (pfVar24[-1] - *(float *)(iVar25 + 4 + (int)param_2)) * fVar3 +
                         *(float *)(iVar25 + 4 + (int)param_2);
            *local_1dc = (*pfVar24 - *pfVar22) * fVar3 + *pfVar22;
            goto LAB_00574ea7;
          }
        }
        else {
          if (DAT_005d757c < fVar3) {
            afStack_180[uVar29] = 0.0;
            uVar29 = uVar29 + 1;
            pfVar21 = pfVar22 + 3;
            fVar3 = *pfVar2 / (*pfVar2 - *pfVar1);
            *pfVar22 = (pfVar24[-2] - *(float *)(iVar25 + (int)param_2)) * fVar3 +
                       *(float *)(iVar25 + (int)param_2);
            pfVar22 = (float *)(iVar25 + 8 + (int)param_2);
            *local_1d4 = (pfVar24[-1] - *(float *)(iVar25 + 4 + (int)param_2)) * fVar3 +
                         *(float *)(iVar25 + 4 + (int)param_2);
            *local_1dc = (*pfVar24 - *pfVar22) * fVar3 + *pfVar22;
            local_1dc = local_1dc + 3;
            local_1d4 = local_1d4 + 3;
          }
          *pfVar21 = pfVar24[-2];
          pfVar21[1] = pfVar24[-1];
          pfVar21[2] = *pfVar24;
          afStack_180[uVar29] = *pfVar1;
          if (local_1e0 == local_1d0) {
            local_1d8 = uVar29;
          }
LAB_00574ea7:
          local_1dc = local_1dc + 3;
          local_1d4 = local_1d4 + 3;
          uVar29 = uVar29 + 1;
          pfVar22 = pfVar21 + 3;
        }
        iVar25 = (-8 - (int)param_2) + (int)pfVar24;
        pfVar24 = pfVar24 + 4;
        uVar20 = local_1d0 + 1;
        uVar28 = local_1d0;
      } while (local_1d0 + 1 < param_3);
      if (4 < uVar29) {
        local_1e8 = 0;
        if (uVar29 != 0) {
          iVar25 = (uVar29 * 3 + -6) * 4;
          uVar28 = uVar29 - 1;
          iVar18 = 0;
          iVar27 = (uVar29 - 1) * 0xc;
          do {
            iVar26 = iVar18;
            if (uVar28 == local_1d8) {
              afStack_1c0[uVar28] = 3.4028235e+38f;
            }
            else {
              fVar30 = (float10)FUN_005751f0((float *)((int)local_120 + iVar25),
                                             (float *)((int)local_120 + iVar27),
                                             (float *)((int)local_120 + iVar26));
              afStack_1c0[uVar28] = (float)fVar30;
            }
            uVar20 = local_1e8 + 1;
            iVar25 = iVar27;
            uVar28 = local_1e8;
            iVar18 = iVar26 + 0xc;
            iVar27 = iVar26;
            local_1e8 = uVar20;
          } while (uVar20 < uVar29);
        }
        if (4 < uVar29) {
          local_1d8 = uVar29 - 4;
          do {
            uVar20 = 0;
            uVar28 = uVar29;
            fVar3 = _DAT_005ceac0;
            if (uVar29 != 0) {
              do {
                fVar4 = afStack_1c0[uVar20];
                if ((fVar4 != _DAT_005cc33c) && (fVar4 < fVar3)) {
                  uVar28 = uVar20;
                  fVar3 = fVar4;
                }
                uVar20 = uVar20 + 1;
              } while (uVar20 < uVar29);
              if (uVar28 != uVar29) {
                uVar23 = 0;
                afStack_1c0[uVar28] = -1.0;
                uVar20 = 0;
                local_1e0 = uVar28;
                do {
                  local_1e0 = local_1e0 + 1;
                  if (uVar29 <= local_1e0) {
                    local_1e0 = 0;
                  }
                  local_1e8 = local_1e0;
                } while ((afStack_1c0[local_1e0] == -1.0) && (uVar20 = uVar20 + 1, uVar20 < uVar29));
                do {
                  local_1e8 = local_1e8 + 1;
                  if (uVar29 <= local_1e8) {
                    local_1e8 = 0;
                  }
                } while ((afStack_1c0[local_1e8] == -1.0) && (uVar23 = uVar23 + 1, uVar23 < uVar29));
                uVar20 = 0;
                do {
                  uVar23 = uVar29;
                  if (uVar28 != 0) {
                    uVar23 = uVar28;
                  }
                  uVar28 = uVar23 - 1;
                } while ((afStack_1c0[uVar28] == -1.0) && (uVar20 = uVar20 + 1, uVar20 < uVar29));
                uVar23 = 0;
                uVar20 = uVar28;
                do {
                  uVar16 = uVar29;
                  if (uVar20 != 0) {
                    uVar16 = uVar20;
                  }
                  uVar20 = uVar16 - 1;
                } while ((afStack_1c0[uVar20] == -1.0) && (uVar23 = uVar23 + 1, uVar23 < uVar29));
                fVar30 = (float10)FUN_005751f0(local_120 + uVar20 * 3, local_120 + uVar28 * 3,
                                               local_120 + local_1e0 * 3);
                afStack_1c0[uVar28] = (float)fVar30;
                fVar30 = (float10)FUN_005751f0(local_120 + uVar28 * 3, local_120 + local_1e0 * 3,
                                               local_120 + local_1e8 * 3);
                afStack_1c0[local_1e0] = (float)fVar30;
              }
            }
            local_1d8 = local_1d8 + -1;
          } while (local_1d8 != 0);
        }
        uVar28 = 0;
        if (uVar29 == 0) {
          return 1;
        }
        pfVar24 = local_120;
        do {
          if (afStack_1c0[uVar28] != -1.0) {
            FUN_00575120(pfVar24, param_4 + afStack_180[uVar28], (int)param_6, fVar17);
          }
          uVar28 = uVar28 + 1;
          pfVar24 = pfVar24 + 3;
        } while (uVar28 < uVar29);
        return 1;
      }
    }
    uVar28 = 0;
    if (uVar29 != 0) {
      pfVar24 = local_120;
      do {
        FUN_00575120(pfVar24, param_4 + afStack_180[uVar28], (int)param_6, fVar17);
        uVar28 = uVar28 + 1;
        pfVar24 = pfVar24 + 3;
      } while (uVar28 < uVar29);
      return 1;
    }
  }
  return 1;
}

// ---------------------------------------------------------------------------
// 0x00575c60  Recursive BVH manifold expansion — exact structural twin of K17
//   FUN_00575fe0. Walks the contact-node tree; leaf nodes get their transform
//   composed (FUN_004c4600) and a pair record emitted or recursed
//   (FUN_00575fe0); the [.+0x10] shape support vtable is __cdecl 4-arg.
// ---------------------------------------------------------------------------
extern "C" int __cdecl
FUN_00575c60(undefined4 *param_1,uint *param_2,float *param_3,float *param_4,float *param_5,
            undefined4 param_6)
{
  int iVar10;
  int iVar11;
  undefined4 *puVar13;
  undefined4 *puVar14;
  undefined4 *puVar15;
  float *pfVar16;
  float *pfVar17;
  undefined4 *puVar18;
  float *local_20;
  float *local_1c;
  uint local_18;

  int iVar8 = (int)param_1;
  int iVar1 = *(int *)((int)param_1 + 0x10);
  iVar10 = *(int *)((int)param_1 + 0xc);
  local_1c = param_3;
  float fVar2 = *(float *)((int)param_1 + 0x70);
  puVar18 = (undefined4 *)(iVar10 + iVar1 * 0x14);
  int iVar3 = *(int *)((int)param_1 + 0x14);
  int iVar4 = *(int *)((int)param_1 + 0x18);
  int iVar5 = *(int *)((int)param_1 + 0x20);
  puVar13 = (undefined4 *)(*(int *)((int)param_1 + 0x1c) * 0x40 + iVar4);
  float *pfVar12 = (float *)(*(int *)((int)param_1 + 0x24) + iVar1 * 0x20);
  puVar15 = (undefined4 *)*param_2;
  if (*(short *)(param_2 + 1) == -1) {
    if ((*(byte *)(puVar15[0x17] + 0x40) & 2) == 0) {
      puVar14 = puVar13;
      for (iVar10 = 0x10; iVar10 != 0; iVar10 = iVar10 + -1) {
        *puVar14 = *puVar15;
        puVar15 = puVar15 + 1;
        puVar14 = puVar14 + 1;
      }
    }
    else {
      puVar13[4] = 0;
      puVar13[10] = 0x3f800000;
      puVar13[5] = 0x3f800000;
      *puVar13 = 0x3f800000;
      puVar13[2] = 0;
      puVar13[1] = 0;
      puVar13[9] = 0;
      puVar13[8] = 0;
      puVar13[6] = 0;
      puVar13[0xe] = 0;
      puVar13[0xd] = 0;
      puVar13[0xc] = 0;
      puVar13[3] = puVar13[3] | 0x20003;
    }
    pfVar12 = (float *)(*(int *)((int)param_1 + 0x24) + iVar1 * 0x20);
    for (iVar10 = 8; iVar10 != 0; iVar10 = iVar10 + -1) {
      *pfVar12 = *param_4;
      param_4 = param_4 + 1;
      pfVar12 = pfVar12 + 1;
    }
    FUN_00575fe0((int)param_1, param_2,
                 (undefined4)(((uint)puVar18 & 0xffff0000u) | (uint)*(ushort *)(param_2 + 1)),
                 (int)*param_2, puVar13, 0, puVar18, param_3, param_5, (uint)param_6);
    return *(int *)((int)param_1 + 0x10) - iVar1;
  }
  param_3 = (float *)puVar15[4];
  local_18 = 0;
  uint uVar9 = (uint)*(ushort *)(puVar15 + 3);
  param_1 = puVar18;
  local_20 = pfVar12;
  if (uVar9 != 0) {
    do {
      if (((undefined4 *)(iVar5 * 0x40 + iVar4) <= puVar13) ||
         ((undefined4 *)(iVar10 + iVar3 * 0x14) <= puVar18)) {
        *(undefined4 *)(iVar8 + 0x78) = 1;
        break;
      }
      puVar15 = *(undefined4 **)param_3;             // param_3[0] is a pointer (Ghidra mistyped float*)
      puVar14 = *(undefined4 **)(param_3 + 2);       // param_3[2] is a pointer
      int iVar6 = puVar15[0x17];
      if ((*(byte *)(iVar6 + 0x40) & 2) == 0) {
        if (puVar14 == (undefined4 *)0x0) {
          puVar14 = puVar13;
          for (iVar11 = 0x10; puVar18 = param_1, iVar11 != 0; iVar11 = iVar11 + -1) {
            *puVar14 = *puVar15;
            puVar15 = puVar15 + 1;
            puVar14 = puVar14 + 1;
          }
        }
        else {
          FUN_004c4600(puVar13, puVar15, puVar14);
          pfVar12 = local_20;
        }
      }
      else {
        puVar15 = puVar13;
        for (iVar11 = 0x10; puVar18 = param_1, iVar11 != 0; iVar11 = iVar11 + -1) {
          *puVar15 = *puVar14;
          puVar14 = puVar14 + 1;
          puVar15 = puVar15 + 1;
        }
      }
      if (local_1c == (float *)0x0) {
        if (uVar9 != 1) {
          ((rwp_support4)(*(void **)(iVar6 + 0x10)))(*(void **)param_3, puVar13, 0, pfVar12);
          pfVar12 = local_20;
          goto LAB_00575eaa;
        }
        pfVar16 = param_4;
        pfVar17 = pfVar12;
        for (iVar11 = 8; puVar15 = param_1, iVar11 != 0; iVar11 = iVar11 + -1) {
          *pfVar17 = *pfVar16;
          pfVar16 = pfVar16 + 1;
          pfVar17 = pfVar17 + 1;
        }
LAB_00575e25:
        if ((*(byte *)(iVar6 + 0x40) & 1) == 0) {
          puVar15[4] = (undefined4)puVar13;
          puVar18 = puVar15 + 5;
          pfVar12 = pfVar12 + 8;
          *puVar15 = *param_2;
          puVar15[1] = param_2[1];
          *(undefined2 *)(puVar15 + 1) = (undefined2)local_18;
          puVar13 = puVar13 + 0x10;
          undefined4 uVar7 = *(undefined4 *)param_3;
          puVar15[3] = 0;
          puVar15[2] = uVar7;
          param_1 = puVar18;
          local_20 = pfVar12;
        }
        else {
          FUN_00575fe0(iVar8, param_2, local_18, *(int *)param_3, puVar13, 0, puVar15, local_1c,
                       param_5, (uint)param_6);
          puVar18 = (undefined4 *)(*(int *)(iVar8 + 0xc) + *(int *)(iVar8 + 0x10) * 0x14);
          puVar13 = (undefined4 *)(*(int *)(iVar8 + 0x1c) * 0x40 + *(int *)(iVar8 + 0x18));
          pfVar12 = (float *)(*(int *)(iVar8 + 0x10) * 0x20 + *(int *)(iVar8 + 0x24));
          param_1 = puVar18;
          local_20 = pfVar12;
        }
      }
      else {
        pfVar16 = local_1c;
        pfVar17 = pfVar12;
        for (iVar11 = 8; puVar18 = param_1, iVar11 != 0; iVar11 = iVar11 + -1) {
          *pfVar17 = *pfVar16;
          pfVar16 = pfVar16 + 1;
          pfVar17 = pfVar17 + 1;
        }
LAB_00575eaa:
        puVar15 = puVar18;
        if (((uVar9 == 1) || (pfVar12 == param_5)) ||
           (((pfVar12[4] - *param_5 < fVar2 != (pfVar12[4] - *param_5 == fVar2) &&
             (((param_5[4] - *pfVar12 < fVar2 != (param_5[4] - *pfVar12 == fVar2) &&
               (pfVar12[5] - param_5[1] < fVar2 != (pfVar12[5] - param_5[1] == fVar2))) &&
              (param_5[5] - pfVar12[1] < fVar2 != (param_5[5] - pfVar12[1] == fVar2))))) &&
            ((pfVar12[6] - param_5[2] < fVar2 != (pfVar12[6] - param_5[2] == fVar2) &&
             (param_5[6] - pfVar12[2] < fVar2 != (param_5[6] - pfVar12[2] == fVar2)))))))
        goto LAB_00575e25;
      }
      if (local_1c != (float *)0x0) {
        local_1c = local_1c + 8;
      }
      local_18 = local_18 + 1;
      param_3 = param_3 + 3;
    } while (local_18 < uVar9);
  }
  iVar10 = ((int)puVar18 - *(int *)(iVar8 + 0xc)) / 0x14;
  *(int *)(iVar8 + 0x10) = iVar10;
  *(int *)(iVar8 + 0x1c) = ((int)puVar13 - *(int *)(iVar8 + 0x18)) >> 6;
  return iVar10 - iVar1;
}

// ---------------------------------------------------------------------------
// 0x00578610  SAT (separating-axis) between two convex hulls. Handles the
//   sphere/point fast paths, then queries each hull's face + edge lists via the
//   [.+0x1c] vtable, tests all face normals (FUN_00578bd0/cb0) and edge-edge
//   cross products (FUN_00578b20), keeping the max penetration (float10 depth
//   carried in ST0). Writes separating axis to param_4, contact scalar/depth to
//   param_5/param_6.
// ---------------------------------------------------------------------------
extern "C" undefined4 __cdecl
FUN_00578610(undefined4 param_1,int *param_2,float param_3,float *param_4,float *param_5,
            float *param_6)
{
  int iVar5;
  int iVar6;
  float *pfVar10;
  uint uVar11;
  float *pfVar12;
  float10 fVar13;
  undefined1 *puStack_144;
  float local_140;
  float fStack_13c;
  float crossN[3];                 // fStack_138/134/130 — contiguous cross-product vec3
  uint uStack_128;
  float faceN[3];                  // fStack_124/120/11c — FUN_00578bd0/cb0 CONTIGUOUS vec3 out (&faceN)
  float fStack_12c;                // FUN_00578bd0/cb0 depth-scalar out (separate slot)
  undefined4 planeA[5];            // local_118/114/110/10c/108 — 5-dword plane descriptor
  int local_104;
  float edgeB[12];                 // fStack_100 + afStack_fc[11] — shape-B edge/vert buffer
  float edgeA[12];                 // local_d0
  undefined1 hullA[80];            // local_a0
  undefined1 hullB[80];            // auStack_50

  iVar5 = *param_2;
  iVar6 = param_2[1];
  int iVar7 = *(int *)(*(int *)(iVar5 + 8) + 0x5c);
  int iVar8 = *(int *)(*(int *)(iVar6 + 8) + 0x5c);
  if ((uint)*(byte *)(iVar8 + 0x39) + (uint)*(byte *)(iVar7 + 0x39) == 0) {
    *param_4 = *(float *)(*(int *)(iVar5 + 0x10) + 0x30) - *(float *)(*(int *)(iVar6 + 0x10) + 0x30);
    param_4[1] = *(float *)(*(int *)(*param_2 + 0x10) + 0x34) -
                 *(float *)(*(int *)(param_2[1] + 0x10) + 0x34);
    param_4[2] = *(float *)(*(int *)(*param_2 + 0x10) + 0x38) -
                 *(float *)(*(int *)(param_2[1] + 0x10) + 0x38);
    fVar13 = (float10)FUN_005667c0(param_4, param_4);
    iVar5 = *(int *)(param_2[1] + 0x10);
    local_140 = *(float *)(iVar5 + 0x38) * param_4[2] +
                *(float *)(iVar5 + 0x30) * *param_4 + *(float *)(iVar5 + 0x34) * param_4[1];
  }
  else if (*(char *)(iVar7 + 0x38) == '\x02') {
    iVar5 = *(int *)(iVar5 + 0x10);
    planeA[0] = *(undefined4 *)(iVar5 + 0x20);
    planeA[1] = *(undefined4 *)(iVar5 + 0x24);
    planeA[2] = *(undefined4 *)(iVar5 + 0x28);
    planeA[3] = 0xff7fffff;
    planeA[4] = 0;
    fVar13 = (float10)FUN_00578bd0(param_2, (float *)planeA, param_4, &local_140);
  }
  else if (*(char *)(iVar8 + 0x38) == '\x02') {
    iVar5 = *(int *)(iVar6 + 0x10);
    planeA[0] = *(undefined4 *)(iVar5 + 0x20);
    planeA[1] = *(undefined4 *)(iVar5 + 0x24);
    planeA[2] = *(undefined4 *)(iVar5 + 0x28);
    planeA[3] = 0xff7fffff;
    planeA[4] = 0;
    fVar13 = (float10)FUN_00578cb0(param_2, (float *)planeA, param_4, &local_140);
  }
  else {
    iVar6 = *(int *)(*(int *)(iVar6 + 8) + 0x5c);
    iVar7 = *(int *)(*(int *)(iVar5 + 8) + 0x5c);
    ushort uVar1 = *(ushort *)(iVar7 + 0x3a);
    ushort uVar2 = *(ushort *)(iVar6 + 0x3a);
    ushort uVar3 = *(ushort *)(iVar6 + 0x3c);
    ushort uVar4 = *(ushort *)(iVar7 + 0x3c);
    local_104 = iVar7;
    ((rwp_hull4)(*(void **)(iVar7 + 0x1c)))(*(int *)(iVar5 + 8), *(undefined4 *)(iVar5 + 0x10),
                                            hullA, edgeA);
    uStack_128 = *(int *)(param_2[1] + 8);
    ((rwp_hull4)(*(void **)(*(int *)(uStack_128 + 0x5c) + 0x1c)))
              (uStack_128, *(undefined4 *)(param_2[1] + 0x10), hullB, edgeB);
    if ((uint)uVar2 * (uint)uVar1 + (uint)uVar3 + (uint)uVar4 == 0) {
      if (*(short *)(iVar7 + 0x3a) == 0) {
        pfVar10 = edgeB;
        iVar5 = *(int *)(param_2[1] + 0x10);
        iVar6 = *(int *)(*param_2 + 0x10);
      }
      else {
        pfVar10 = edgeA;
        iVar5 = *(int *)(*param_2 + 0x10);
        iVar6 = *(int *)(param_2[1] + 0x10);
      }
      FUN_00578d90((float *)(iVar6 + 0x30), (float *)(iVar5 + 0x30), pfVar10, param_4);
      fVar13 = (float10)FUN_00578b20(param_2, param_4, &local_140);
    }
    else {
      uVar11 = 0;
      local_140 = 0.0;
      fStack_13c = -3.4028235e+38f;
      if (*(short *)(iVar7 + 0x3c) != 0) {
        puStack_144 = hullA;
        do {
          fVar13 = (float10)FUN_00578bd0(param_2, (float *)puStack_144, faceN, &fStack_12c);
          if ((float10)fStack_13c < fVar13) {
            local_140 = fStack_12c;
            *param_4 = faceN[0];
            fStack_13c = (float)fVar13;
            param_4[1] = faceN[1];
            param_4[2] = faceN[2];
          }
          uVar11 = uVar11 + 1;
          puStack_144 = puStack_144 + 0x14;
        } while (uVar11 < *(ushort *)(iVar7 + 0x3c));
      }
      uVar11 = 0;
      if (*(short *)(iVar6 + 0x3c) != 0) {
        puStack_144 = hullB;
        do {
          fVar13 = (float10)FUN_00578cb0(param_2, (float *)puStack_144, faceN, &fStack_12c);
          if ((float10)fStack_13c < fVar13) {
            local_140 = fStack_12c;
            *param_4 = faceN[0];
            fStack_13c = (float)fVar13;
            param_4[1] = faceN[1];
            param_4[2] = faceN[2];
          }
          uVar11 = uVar11 + 1;
          puStack_144 = puStack_144 + 0x14;
        } while (uVar11 < *(ushort *)(iVar6 + 0x3c));
      }
      uStack_128 = 0;
      if (*(short *)(iVar7 + 0x3a) != 0) {
        pfVar10 = edgeA;
        do {
          puStack_144 = (undefined1 *)0x0;
          if (*(short *)(iVar6 + 0x3a) != 0) {
            pfVar12 = edgeB + 1;
            do {
              crossN[0] = pfVar12[1] * pfVar10[1] - *pfVar12 * pfVar10[2];
              crossN[1] = pfVar12[-1] * pfVar10[2] - *pfVar10 * pfVar12[1];
              crossN[2] = *pfVar12 * *pfVar10 - pfVar12[-1] * pfVar10[1];
              float fVar9 = crossN[2] * crossN[2] + crossN[0] * crossN[0] + crossN[1] * crossN[1];
              if (_DAT_005ce54c < fVar9) {
                fVar9 = _DAT_005cc320 / sqrtf(fVar9);
                crossN[0] = crossN[0] * fVar9;
                crossN[1] = crossN[1] * fVar9;
                crossN[2] = fVar9 * crossN[2];
                fVar13 = (float10)FUN_00578b20(param_2, crossN, &fStack_12c);
                if ((float10)fStack_13c < fVar13) {
                  local_140 = fStack_12c;
                  *param_4 = crossN[0];
                  fStack_13c = (float)fVar13;
                  param_4[1] = crossN[1];
                  param_4[2] = crossN[2];
                }
              }
              puStack_144 = (undefined1 *)((int)puStack_144 + 1);
              pfVar12 = pfVar12 + 3;
            } while ((uint)puStack_144 < (uint)*(ushort *)(iVar6 + 0x3a));
          }
          uStack_128 = uStack_128 + 1;
          pfVar10 = pfVar10 + 3;
        } while (uStack_128 < *(ushort *)(local_104 + 0x3a));
      }
      if (fStack_13c <= _DAT_005e5050) {
        FUN_00578d90((float *)(*(int *)(*param_2 + 0x10) + 0x30),
                     (float *)(*(int *)(param_2[1] + 0x10) + 0x30), edgeB, param_4);
        fVar13 = (float10)FUN_00578b20(param_2, param_4, &local_140);
      }
      else {
        fVar13 = (float10)fStack_13c;
      }
    }
  }
  float10 fVar14 = (float10)*(float *)(*(int *)(*param_2 + 8) + 0x4c);
  float10 fVar15 = (float10)*(float *)(*(int *)(param_2[1] + 8) + 0x4c);
  float10 fVar16 = (fVar13 - fVar14) - fVar15;
  *param_6 = (float)fVar16;
  if (fVar16 <= (float10)param_3) {
    *param_5 = (float)(((fVar15 - fVar14) + fVar13) * (float10)_DAT_005cc32c + (float10)local_140);
    return 1;
  }
  return 1;
}

// ---------------------------------------------------------------------------
// 0x0057a9a0  Conservative-advancement TOI iteration. Handles the point-shape
//   fast paths (FUN_0055c000 support), else runs up to 4 CA steps: query the
//   min-distance simplex (FUN_0057a250) along the swept direction, advance the
//   fraction, and converge (float10 support extents from FUN_0057a660/FUN_0057a250).
// ---------------------------------------------------------------------------
extern "C" undefined4 __cdecl
FUN_0057a9a0(undefined4 param_1,int *param_2,float param_3,float *param_4,float *param_5,
            float *param_6,float param_7)
{
  int iVar1;
  bool bVar2;
  float fVar3;
  int iVar4;
  float10 fVar5;
  float local_200;
  float local_1fc;
  float local_1f8;
  float local_1f4;
  float local_1f0;
  float local_1ec;
  float local_1e8;
  int local_1e4;
  float d0[3];                     // local_1e0/1dc/1d8 — FUN_0055c000 support / loc scratch
  float d1[3];                     // local_1d4/1d0/1cc — FUN_0057a660 swept direction
  uint local_1c8;
  float local_1c4;
  float local_1c0;
  float cp[3];                     // local_1bc/1b8/1b4 — FUN_0057a250 contact point out
  undefined1 local_1b0 [432];

  iVar4 = *(int *)(*param_2 + 8);
  iVar1 = *(int *)(param_2[1] + 8);
  local_1f0 = *(float *)(iVar4 + 0x4c);
  local_1e8 = *(float *)(iVar1 + 0x4c);
  if (**(short **)(iVar4 + 0x5c) == 1) {
    *param_4 = -*(float *)(*(int *)(*param_2 + 0x10) + 0x20);
    param_4[1] = -*(float *)(*(int *)(*param_2 + 0x10) + 0x24);
    param_4[2] = -*(float *)(*(int *)(*param_2 + 0x10) + 0x28);
    FUN_0055c000(*(int *)(param_2[1] + 8), *(float **)(param_2[1] + 0x10), param_4, d0);
    iVar4 = *(int *)(*param_2 + 0x10);
    fVar3 = param_4[2] * d0[2] + d0[0] * *param_4 + param_4[1] * d0[1] + local_1e8;
    local_1e8 = (*(float *)(iVar4 + 0x38) * param_4[2] +
                *(float *)(iVar4 + 0x30) * *param_4 + *(float *)(iVar4 + 0x34) * param_4[1]) -
                local_1f0;
  }
  else {
    if (**(short **)(iVar1 + 0x5c) != 1) {
      fVar5 = (float10)FUN_0057a660(param_2, d1);
      local_1f8 = (float)fVar5;
      local_1fc = param_7 * _DAT_005cc328;
      if (local_1fc < local_1f8) {
        local_1fc = local_1fc + local_1f8;
      }
      local_1c4 = local_1e8 + local_1f0;
      local_1ec = 0.0;
      local_200 = 0.0;
      local_1c0 = local_1c4 + param_3;
      local_1e4 = 0;
      local_1c8 = 0;
      do {
        d0[0] = d1[0] * local_200;
        d0[1] = d1[1] * local_200;
        d0[2] = d1[2] * local_200;
        iVar4 = FUN_0057a250(param_2, d0, local_1c0 + local_200, (int)local_1b0, cp,
                             &local_1f8, &local_1f4, param_7);
        if ((iVar4 == 0) || (local_1f4 <= DAT_005d757c)) {
          local_1ec = local_200;
          if (local_1e4 == 0) {
            if (local_200 == local_1fc) {
              local_1fc = local_1fc + param_7;
            }
            local_200 = local_1fc;
          }
          else {
            local_200 = (local_200 + local_1fc) * _DAT_005cc32c;
          }
        }
        else {
          local_1fc = local_200;
          local_1e4 = 1;
          bVar2 = local_200 == DAT_005d757c;
          *param_4 = cp[0];
          param_4[1] = cp[1];
          param_4[2] = cp[2];
          *param_5 = local_1f8;
          *param_6 = local_1f4;
          if (bVar2) goto LAB_0057ad7f;
          if (local_1f4 < param_3) goto LAB_0057ad3c;
          local_1f8 = param_4[2] * d1[2] + d1[0] * *param_4 + param_4[1] * d1[1];
          if (local_200 <= local_1f4 / local_1f8) {
            local_200 = local_1ec * _DAT_005cc9dc + local_200 * _DAT_005cc9a0;
          }
          else {
            local_200 = local_200 - (local_1f4 / local_1f8) * _DAT_005cc9dc;
          }
          if (local_200 < local_1ec) {
            local_1ec = local_200;
          }
        }
        local_1c8 = local_1c8 + 1;
      } while (local_1c8 < 4);
      if (local_1e4 == 0) {
        return 0;
      }
LAB_0057ad3c:
      if (local_1fc != DAT_005d757c) {
        local_1fc = (param_4[2] * d1[2] + d1[0] * *param_4 + param_4[1] * d1[1]) * local_1fc;
        *param_5 = *param_5 - _DAT_005cc32c * local_1fc;
        *param_6 = *param_6 - local_1fc;
      }
LAB_0057ad7f:
      *param_5 = *param_5 - (local_1f0 - local_1e8) * _DAT_005cc32c;
      *param_6 = *param_6 - local_1c4;
      return 1;
    }
    iVar4 = *(int *)(param_2[1] + 0x10);
    *param_4 = *(float *)(iVar4 + 0x20);
    param_4[1] = *(float *)(iVar4 + 0x24);
    param_4[2] = *(float *)(iVar4 + 0x28);
    FUN_0055c000(*(int *)(*param_2 + 8), *(float **)(*param_2 + 0x10), param_4, d0);
    iVar4 = *(int *)(param_2[1] + 0x10);
    fVar3 = param_4[2] * d0[2] + d0[0] * *param_4 + param_4[1] * d0[1] + local_1f0;
    local_1e8 = (*(float *)(iVar4 + 0x38) * param_4[2] +
                *(float *)(iVar4 + 0x30) * *param_4 + *(float *)(iVar4 + 0x34) * param_4[1]) -
                local_1e8;
  }
  *param_5 = (local_1e8 + fVar3) * _DAT_005cc32c;
  *param_6 = local_1e8 - fVar3;
  return 1;
}

// --- gta-reversed-style hook registration — CLUSTER 18. ---
RH_ScopedInstall(FUN_005757d0, 0x005757d0);
RH_ScopedInstall(FUN_0056b7a0, 0x0056b7a0);
RH_ScopedInstall(FUN_00574ad0, 0x00574ad0);
RH_ScopedInstall(FUN_00575c60, 0x00575c60);
RH_ScopedInstall(FUN_00578610, 0x00578610);
RH_ScopedInstall(FUN_0057a9a0, 0x0057a9a0);

}  // namespace Collision
}  // namespace mashed_re
