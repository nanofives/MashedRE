// ============================================================================
//  RwpSolverCore20.cpp  —  B5e solver-island cluster K20
//  2 verbatim RWP-3.7 narrow-phase pair-driver helpers:
//    FUN_00575880 (0x00575880, 732 B)  — single-pair narrow-phase driver: reserves
//                                         a manifold slot in the world scratch pool,
//                                         runs the K19 SAT (FUN_00578e50) or TOI
//                                         (FUN_0057adb0) wrapper on the shape pair,
//                                         copies both shape blocks into the manifold
//                                         record, and (for shape-A) rotates the local
//                                         axis into world space + orients the normal
//                                         via FUN_0055c2d0. Shape-B fixup via
//                                         FUN_00575b60.
//    FUN_00575560 (0x00575560, 618 B)  — pair-batch driver: walks `param_3` pairs,
//                                         calls FUN_005752b0 per pair to build a
//                                         manifold (capped at param_5-1), reduces the
//                                         accumulated manifolds via FUN_00578ff0, then
//                                         threads the surviving contacts onto the two
//                                         bodies' contact lists (world+0x64 pool).
//  Deps: K3 (FUN_0055c2d0), K17 (FUN_00575b60, FUN_00578ff0), K19 (FUN_00578e50,
//        FUN_0057adb0, FUN_005752b0). All externed below; definitions live in the
//        sibling RwpSolverBroadphase3 (0055c2d0) / RwpSolverCore17 / RwpSolverCore19
//        TUs of the same .asi.
//  Version anchor: MASHED.exe 2,846,720 B / SHA-256 BDCAE093…3C0E.
//
//  All float math is x87 (FLD/FMUL/FADDP/FCOMP confirmed by disasm) — plain C float
//  compiles to x87 under /arch:IA32, so this is bit-faithful.
//
//  DISASM-RESOLVED CONVENTIONS (verified against listing, pool14 session):
//   * FUN_00575880: incoming param_1 is a POINTER (World*), NOT a float — Ghidra
//     mistyped it `float` because it is only ever `(int)param_1 + off` and one FLD
//     of [param_1+0x74]. Ported as `int world`. The Ghidra-reused `param_1 =
//     *(float*)(world+0x74)` (0x5758c1) is split off into the float local `local_p1`,
//     which then doubles as the first FUN_0055c2d0 out-float (its address is passed at
//     0x575a4f and read back in the `param_4 - fVar8 < fVar8 - local_p1` orient test).
//     The incoming float param_4 (depth) likewise doubles as the second out-float
//     (&param_4 at 0x575a4b) — this is the K17/K19 scratch-slot-reuse pattern.
//   * FUN_00578e50 / FUN_0057adb0 (K19) are called with FIVE __cdecl args (ADD ESP,
//     0x14 @ 0x575917). The 5th arg is the raw dword [world+0x74] (PUSH EDX @ 0x5758f3
//     / 0x5758eb, no FLD conversion) — externed here with a `float` 5th param so
//     `local_p1` is pushed as its raw bits (K19's body only forwards param_5, never
//     interprets it, so byte content is all that matters — same reuse-by-name trick as
//     K19's 7-vs-6 arg). param_2 is the int* pair pointer (raw dword push) → externed
//     `int *`.
//   * FUN_00575560 → FUN_005752b0 (K19): arg2 is the raw dword [world+0x70] (MOV EAX,
//     [ESI+0x70]; PUSH EAX @ 0x5755f1) fed to K19's `float` tolerance param → read as
//     `*(float*)(iVar3+0x70)` for a byte-exact push (no int→float conversion).
//   * FUN_00575560 → FUN_00578ff0 (K17): SIX __cdecl args (ADD ESP,0x18 @ 0x575656).
//     arg6 is the raw dword [world+0x74] (PUSH EDX @ 0x575640) fed to K17's `float`
//     param_6 → read `*(float*)(iVar3+0x74)`. arg1/arg4 are the manifold-pool base
//     (param_4[+0xe0]) cast to uint* to match the K17 signature.
//   * The two `< param_4` float compares (0x575928 FCOMP, 0x575a97 FSUB/FCOMPP) are
//     rendered by Ghidra as `a < b != (a == b)` (unordered-NaN modelling); the faithful
//     C source is a plain `<`, as in prior K-clusters.
// ============================================================================
#include "../Core/HookSystem.h"

namespace mashed_re {
namespace Collision {

typedef unsigned int   uint;
typedef unsigned int   undefined4;
typedef unsigned char  undefined1;
typedef unsigned short undefined2;
typedef unsigned short ushort;
typedef unsigned char  byte;

// --- extern callees (K3/K17/K19, defined in sibling TUs of same .asi) --------
// K3 rotate-into-world + orient (RwpSolverBroadphase3). void, 5-arg __cdecl.
extern "C" void       __cdecl FUN_0055c2d0(int param_1,float *param_2,float *param_3,float *param_4,float *param_5); // 0x0055c2d0 (K3)
// K17 shape-B face-normal orient. void, 4-arg __cdecl.
extern "C" void       __cdecl FUN_00575b60(int param_1,int param_2,float *param_3,byte *param_4); // 0x00575b60 (K17)
// K17 manifold coplanar-merge + tetra reduce. uint, 6-arg __cdecl (param_6 = float).
extern "C" uint       __cdecl FUN_00578ff0(uint *param_1,uint param_2,int param_3,uint *param_4,uint param_5,float param_6); // 0x00578ff0 (K17)
// K19 SAT-axis wrapper. 5-arg __cdecl; param_5 externed float for a raw-dword push.
extern "C" undefined4 __cdecl FUN_00578e50(undefined4 param_1,int *param_2,float param_3,int param_4,float param_5); // 0x00578e50 (K19)
// K19 TOI wrapper. same shape.
extern "C" undefined4 __cdecl FUN_0057adb0(undefined4 param_1,int *param_2,float param_3,int param_4,float param_5); // 0x0057adb0 (K19)
// K19 box/box narrow-phase driver. 3-arg __cdecl (param_2 = float tolerance).
extern "C" undefined4 __cdecl FUN_005752b0(int param_1,float param_2,float *param_3); // 0x005752b0 (K19)

// forward decls for RH_ScopedInstall
extern "C" undefined4 __cdecl FUN_00575880(int param_1,int param_2,int *param_3,float param_4);
extern "C" int        __cdecl FUN_00575560(int param_1,uint param_2,uint param_3,int param_4,int param_5);

// ---------------------------------------------------------------------------
// 0x00575880  Single-pair narrow-phase driver. param_1 = World*, param_2 = solver
//   context, param_3 = 2-entry body-pair pointer array, param_4 = penetration
//   tolerance. Reserves the next manifold record from the world pool (world+0x2c/
//   +0x30/+0x34), runs the K19 SAT or TOI wrapper (chosen by the 0x10 flag on either
//   body's shape), fills the two shape blocks (+0x58.. / +0xd8..), and for shape-A
//   whose 0x40 flag is set rotates its local axis into world space and orients it.
// ---------------------------------------------------------------------------
extern "C" undefined4 __cdecl
FUN_00575880(int param_1,int param_2,int *param_3,float param_4)
{
  float *pfVar1;
  short sVar2;
  undefined4 *puVar3;
  int iVar4;
  int iVar5;
  float *pfVar6;
  int iVar7;
  float fVar8;
  int iVar9;
  uint uVar10;
  int *piVar11;
  int iVar12;
  float local_p1;   // Ghidra-reused param_1: float [world+0x74], also 1st FUN_0055c2d0 out-float

  piVar11 = param_3;
  iVar4 = param_1;
  if ((*(uint *)(param_1 + 0x40) < *(int *)(param_1 + 0x3c) + 0x10U) ||
     (*(uint *)(param_1 + 0x34) <= *(uint *)(param_1 + 0x30))) {
    *(undefined4 *)(param_1 + 0x78) = 1;
  }
  else {
    iVar12 = *(uint *)(param_1 + 0x30) * 0x120 + *(int *)(param_1 + 0x2c);
    *(int *)(iVar12 + 0x50) = *(int *)(param_1 + 0x3c) * 0x10 + *(int *)(param_1 + 0x38);
    local_p1 = *(float *)(param_1 + 0x74);
    if (((*(uint *)(*(int *)(*(int *)(param_3[1] + 8) + 0x5c) + 0x40) |
         *(uint *)(*(int *)(*(int *)(*param_3 + 8) + 0x5c) + 0x40)) & 0x10) == 0) {
      iVar9 = FUN_00578e50(*(undefined4 *)(*(int *)(param_2 + 0x70) + 4),param_3,param_4,iVar12,
                           local_p1);
    }
    else {
      iVar9 = FUN_0057adb0(*(undefined4 *)(*(int *)(param_2 + 0x70) + 4),param_3,param_4,iVar12,
                           local_p1);
    }
    if ((iVar9 != 0) && (*(float *)(iVar12 + 0x110) < param_4)) {
      uVar10 = *(int *)(iVar4 + 0x30) + 1;
      *(uint *)(iVar4 + 0x30) = uVar10;
      if (*(uint *)(iVar4 + 0x34) <= uVar10) {
        *(undefined4 *)(iVar4 + 0x78) = 1;
      }
      if (4 < *(ushort *)(iVar12 + 0x54)) {
        *(uint *)(iVar4 + 0x3c) = *(int *)(iVar4 + 0x3c) + (uint)*(ushort *)(iVar12 + 0x54);
      }
      if (4 < *(ushort *)(iVar12 + 0xd4)) {
        *(uint *)(iVar4 + 0x3c) = *(int *)(iVar4 + 0x3c) + (uint)*(ushort *)(iVar12 + 0xd4);
      }
      puVar3 = (undefined4 *)*piVar11;
      *(undefined4 *)(iVar12 + 0x58) = *puVar3;
      *(undefined4 *)(iVar12 + 0x5c) = puVar3[1];
      *(undefined4 *)(iVar12 + 0x6c) = puVar3[3];
      uVar10 = *(uint *)(*(int *)(puVar3[2] + 0x5c) + 0x40);
      *(short *)(iVar12 + 0x7c) = (short)uVar10;
      *(undefined4 *)(iVar12 + 0x78) = *(undefined4 *)(puVar3[2] + 0x4c);
      *(undefined4 *)(iVar12 + 0x70) = *(undefined4 *)(puVar3[2] + 0x54);
      *(undefined4 *)(iVar12 + 0x74) = *(undefined4 *)(puVar3[2] + 0x50);
      *(undefined2 *)(iVar12 + 0x7e) = 0;
      if ((uVar10 & 0x40) != 0) {
        iVar4 = piVar11[1];
        iVar9 = *piVar11;
        pfVar1 = (float *)(iVar12 + 0x60);
        iVar5 = *(int *)(iVar9 + 8);
        sVar2 = *(short *)(iVar5 + 0x48);
        *(short *)(iVar12 + 0x7e) = sVar2;
        piVar11 = param_3;
        if (sVar2 != 0xe) {
          pfVar6 = *(float **)(iVar9 + 0x10);
          *pfVar1 = *pfVar6 * *(float *)(iVar5 + 0x30) +
                    pfVar6[4] * *(float *)(iVar5 + 0x34) + pfVar6[8] * *(float *)(iVar5 + 0x38);
          iVar7 = *(int *)(iVar9 + 0x10);
          *(float *)(iVar12 + 0x64) =
               *(float *)(iVar7 + 0x14) * *(float *)(iVar5 + 0x34) +
               *(float *)(iVar7 + 4) * *(float *)(iVar5 + 0x30) +
               *(float *)(iVar7 + 0x24) * *(float *)(iVar5 + 0x38);
          iVar7 = *(int *)(iVar9 + 0x10);
          *(float *)(iVar12 + 0x68) =
               *(float *)(iVar7 + 0x18) * *(float *)(iVar5 + 0x34) +
               *(float *)(iVar7 + 8) * *(float *)(iVar5 + 0x30) +
               *(float *)(iVar7 + 0x28) * *(float *)(iVar5 + 0x38);
          if ((*(byte *)(iVar12 + 0x7e) & 1) == 0) {
            FUN_0055c2d0(*(int *)(iVar4 + 8),*(float **)(iVar4 + 0x10),pfVar1,&local_p1,
                         &param_4);
            iVar4 = *(int *)(iVar9 + 0x10);
            fVar8 = *(float *)(iVar4 + 0x38) * *(float *)(iVar12 + 0x68) +
                    *(float *)(iVar4 + 0x30) * *pfVar1 +
                    *(float *)(iVar4 + 0x34) * *(float *)(iVar12 + 0x64) +
                    *(float *)(iVar5 + 0x18) * *(float *)(iVar5 + 0x38) +
                    *(float *)(iVar5 + 0x10) * *(float *)(iVar5 + 0x30) +
                    *(float *)(iVar5 + 0x14) * *(float *)(iVar5 + 0x34);
            piVar11 = param_3;
            if (param_4 - fVar8 < fVar8 - local_p1) {
              *pfVar1 = -*pfVar1;
              *(float *)(iVar12 + 0x64) = -*(float *)(iVar12 + 0x64);
              *(float *)(iVar12 + 0x68) = -*(float *)(iVar12 + 0x68);
            }
          }
        }
      }
      puVar3 = (undefined4 *)piVar11[1];
      *(undefined4 *)(iVar12 + 0xd8) = *puVar3;
      *(undefined4 *)(iVar12 + 0xdc) = puVar3[1];
      *(undefined4 *)(iVar12 + 0xec) = puVar3[3];
      uVar10 = *(uint *)(*(int *)(puVar3[2] + 0x5c) + 0x40);
      *(short *)(iVar12 + 0xfc) = (short)uVar10;
      *(undefined4 *)(iVar12 + 0xf8) = *(undefined4 *)(puVar3[2] + 0x4c);
      *(undefined4 *)(iVar12 + 0xf0) = *(undefined4 *)(puVar3[2] + 0x54);
      *(undefined4 *)(iVar12 + 0xf4) = *(undefined4 *)(puVar3[2] + 0x50);
      *(undefined2 *)(iVar12 + 0xfe) = 0;
      if ((uVar10 & 0x40) != 0) {
        FUN_00575b60(piVar11[1],*piVar11,(float *)(iVar12 + 0xe0),(byte *)(iVar12 + 0xfe));
      }
      return 1;
    }
  }
  return 0;
}

// ---------------------------------------------------------------------------
// 0x00575560  Pair-batch narrow-phase driver. param_1 = World*, param_2 = a body
//   pair whose two shape flag words gate the fast path, param_3 = pair count,
//   param_4 = manifold record pool base, param_5 = record cap. Walks up to param_3
//   pairs (starting at param_2, +0x120 stride) building at most param_5-1 manifolds
//   via FUN_005752b0, reduces them with FUN_00578ff0, then — if either body carries
//   the 0x40 (contact-list) flag — threads each surviving contact onto that body's
//   list from the world contact pool (world+0x64, count at world+0x68 / cap +0x6c).
//   When either shape sets the 0x20 (trigger) flag it skips solving and only records
//   the raw overlap onto the contact lists.
// ---------------------------------------------------------------------------
extern "C" int __cdecl
FUN_00575560(int param_1,uint param_2,uint param_3,int param_4,int param_5)
{
  int *piVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  uint uVar7;
  int local_8;
  uint local_4;

  iVar4 = param_2;
  iVar3 = param_1;
  uVar6 = 0;
  if (param_3 != 0) {
    if (*(short *)(param_2 + 0x5c) == -1) {
      local_4 = 0;
    }
    else {
      local_4 = *(uint *)(*(int *)(param_2 + 0x58) + 8);
    }
    if (*(short *)(param_2 + 0xdc) == -1) {
      param_2 = 0;
    }
    else {
      param_2 = *(uint *)(*(int *)(param_2 + 0xd8) + 8);
    }
    if (((local_4 & 0x20) == 0) && ((param_2 & 0x20) == 0)) {
      local_8 = param_4 + 0xe0;
      uVar7 = 0;
      if (param_3 != 0) {
        param_1 = iVar4;
        do {
          if (param_5 - 1U <= uVar7) break;
          iVar5 = FUN_005752b0(param_1,*(float *)(iVar3 + 0x70),(float *)local_8);
          if (iVar5 != 0) {
            uVar7 = uVar7 + 1;
            local_8 = local_8 + 0xe0;
          }
          uVar6 = uVar6 + 1;
          param_1 = param_1 + 0x120;
        } while (uVar6 < param_3);
      }
      param_3 = 0;
      if (uVar7 != 0) {
        param_3 = FUN_00578ff0((uint *)(param_4 + 0xe0),uVar7,*(int *)(iVar3 + 0x7c),(uint *)param_4,
                               param_5,*(float *)(iVar3 + 0x74));
      }
      if ((((param_2 | local_4) & 0x40) != 0) && (param_3 != 0)) {
        iVar5 = param_4;
        param_4 = param_3;
        do {
          if ((local_4 & 0x40) != 0) {
            uVar6 = *(uint *)(iVar3 + 0x68);
            iVar2 = *(int *)(iVar4 + 0x58);
            if (uVar6 < *(uint *)(iVar3 + 0x6c)) {
              piVar1 = (int *)(*(int *)(iVar3 + 0x64) + uVar6 * 0x14);
              *(uint *)(iVar3 + 0x68) = uVar6 + 1;
              *piVar1 = iVar2;
              piVar1[1] = *(int *)(iVar4 + 0xd8);
              piVar1[2] = *(int *)(iVar4 + 0xdc);
              piVar1[3] = iVar5;
              piVar1[4] = *(int *)(iVar2 + 0x2c);
              *(int **)(iVar2 + 0x2c) = piVar1;
            }
          }
          if ((param_2 & 0x40) != 0) {
            uVar6 = *(uint *)(iVar3 + 0x68);
            iVar2 = *(int *)(iVar4 + 0xd8);
            if (uVar6 < *(uint *)(iVar3 + 0x6c)) {
              piVar1 = (int *)(*(int *)(iVar3 + 0x64) + uVar6 * 0x14);
              *(uint *)(iVar3 + 0x68) = uVar6 + 1;
              *piVar1 = iVar2;
              piVar1[1] = *(int *)(iVar4 + 0x58);
              piVar1[2] = *(int *)(iVar4 + 0x5c);
              piVar1[3] = iVar5;
              piVar1[4] = *(int *)(iVar2 + 0x2c);
              *(int **)(iVar2 + 0x2c) = piVar1;
            }
          }
          iVar5 = iVar5 + 0xe0;
          param_4 = param_4 + -1;
        } while (param_4 != 0);
      }
      return param_3;
    }
    if ((local_4 & 0x40) != 0) {
      uVar6 = *(uint *)(param_1 + 0x68);
      iVar3 = *(int *)(iVar4 + 0x58);
      if (uVar6 < *(uint *)(param_1 + 0x6c)) {
        piVar1 = (int *)(*(int *)(param_1 + 0x64) + uVar6 * 0x14);
        *(uint *)(param_1 + 0x68) = uVar6 + 1;
        *piVar1 = iVar3;
        piVar1[1] = *(int *)(iVar4 + 0xd8);
        piVar1[2] = *(int *)(iVar4 + 0xdc);
        piVar1[3] = 0;
        piVar1[4] = *(int *)(iVar3 + 0x2c);
        *(int **)(iVar3 + 0x2c) = piVar1;
      }
    }
    if ((param_2 & 0x40) != 0) {
      uVar6 = *(uint *)(param_1 + 0x68);
      iVar3 = *(int *)(iVar4 + 0xd8);
      if (uVar6 < *(uint *)(param_1 + 0x6c)) {
        piVar1 = (int *)(*(int *)(param_1 + 0x64) + uVar6 * 0x14);
        *(uint *)(param_1 + 0x68) = uVar6 + 1;
        *piVar1 = iVar3;
        piVar1[1] = *(int *)(iVar4 + 0x58);
        piVar1[2] = *(int *)(iVar4 + 0x5c);
        piVar1[3] = 0;
        piVar1[4] = *(int *)(iVar3 + 0x2c);
        *(int **)(iVar3 + 0x2c) = piVar1;
      }
    }
  }
  return 0;
}

// --- gta-reversed-style hook registration — CLUSTER 20. ---
RH_ScopedInstall(FUN_00575880, 0x00575880);
RH_ScopedInstall(FUN_00575560, 0x00575560);

}  // namespace Collision
}  // namespace mashed_re
